/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <setjmp.h>
#include <stddef.h>


/*
 * Execution frames
 */

enum {
    MAX_LOCALS = 50
};

/**
 * Active execution frame. These are passed around within this file
 * only, as code executes.
 */
typedef struct Frame {
    /** Base variable maplet. */
    zvalue base;

    /** Number of additional local variables. */
    zint size;

    /** Additional local variables. */
    zmapping locals[MAX_LOCALS];
} Frame;

static void frameAdd(Frame *frame, zvalue name, zvalue value) {
    zint size = frame->size;

    if (size >= MAX_LOCALS) {
        die("Too many local variables!");
    }

    frame->locals[size].key = name;
    frame->locals[size].value = value;
    frame->size = size + 1;
}

static zvalue frameGet(Frame *frame, zvalue name) {
    zmapping *locals = frame->locals;

    // Scan in reverse, because we don't prevent duplicate names from
    // being defined, and lookup should find the latest definition.
    for (zint i = frame->size - 1; i >= 0; i--) {
        if (datEq(name, locals[i].key)) {
            return locals[i].value;
        }
    }

    return datMapGet(frame->base, name);
}

static zvalue frameCollapse(Frame *frame) {
    return datMapAddArray(frame->base, frame->size, frame->locals);
}


/*
 * Evaluation
 */

/**
 * Closure, that is, a function and its associated immutable bindings.
 * Instances of this structure are bound as the closure state as part of
 * function registration in `execFunction()`.
 */
typedef struct {
    /**
     * Parent variable context (which was the current context when
     * the function was defined).
     */
    zvalue context;

    /**
     * Function definition, which includes a list of formals and the
     * block to run.
     */
    zvalue function;
} Closure;

/**
 * Nonlocal exit pending state. Instances of this structure are bound as
 * the closure state as part of call setup for closures that have a
 * `yieldDef`.
 */
typedef struct {
    /**
     * Whether this state is active, in that there is a valid nonlocal exit
     * that could land here.
     */
    bool active;

    /** Return value thrown here via nonlocal exit. `NULL` for void. */
    zvalue result;

    /** Jump buffer, used for nonlocal exit. */
    jmp_buf jumpBuf;
} YieldState;

/**
 * Marks a closure state for garbage collection.
 */
static void closureMark(void *state) {
    Closure *closure = state;

    datMark(closure->context);
    datMark(closure->function);
}

/**
 * Frees a closure state.
 */
static void closureFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table for closures. */
static DatUniqletDispatch CLOSURE_DISPATCH = {
    closureMark,
    closureFree
};

/**
 * Marks a yield state for garbage collection.
 */
static void yieldMark(void *state) {
    YieldState *yield = state;

    datMark(yield->result);
}

/**
 * Frees a yield state.
 */
static void yieldFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table for yield states. */
static DatUniqletDispatch YIELD_DISPATCH = {
    yieldMark,
    yieldFree
};


/* Defined below. */
static zvalue execExpression(Frame *frame, zvalue expression);
static zvalue execExpressionVoidOk(Frame *frame, zvalue expression);

/**
 * The C function that is bound to in order to perform nonlocal exit.
 */
static zvalue nonlocalExit(zvalue state, zint argCount, const zvalue *args) {
    YieldState *yield = datUniqletGetState(state, &YIELD_DISPATCH);

    if (yield->active) {
        yield->active = false;
    } else {
        die("Attempt to use out-of-scope nonlocal exit.");
    }

    switch (argCount) {
        case 0: {
            // Nothing to do.
            break;
        }
        case 1: {
            yield->result = args[0];
            break;
        }
        default: {
            die("Too many arguments for nonlocal exit: %lld", argCount);
        }
    }

    longjmp(yield->jumpBuf, 1);
}

/**
 * Binds variables for all the formal arguments of the given
 * function (if any), into the given execution frame.
 */
static void bindArguments(Frame *frame, zvalue functionNode,
                          zint argCount, const zvalue *args) {
    zvalue formals = datMapGet(functionNode, STR_FORMALS);

    if (formals == NULL) {
        return;
    }

    zint formalsSize = datSize(formals);

    for (zint i = 0, argAt = 0; i < formalsSize; i++) {
        zvalue formal = datListNth(formals, i);
        zvalue name = datMapGet(formal, STR_NAME);
        zvalue repeat = datMapGet(formal, STR_REPEAT);
        bool ignore = (name == NULL);
        zvalue value = NULL;

        if (repeat != NULL) {
            if (datEq(repeat, STR_CH_STAR)) {
                if (argAt >= argCount) {
                    value = EMPTY_LIST;
                } else if (!ignore) {
                    value = datListFromArray(argCount - argAt, &args[argAt]);
                }
                argAt = argCount;
            } else if (datEq(repeat, STR_CH_PLUS)) {
                if (argAt >= argCount) {
                    die("Too few arguments to function: %lld", argCount);
                } else if (!ignore) {
                    value = datListFromArray(argCount - argAt, &args[argAt]);
                }
                argAt = argCount;
            } else if (datEq(repeat, STR_CH_QMARK)) {
                if (argAt >= argCount) {
                    value = EMPTY_LIST;
                } else {
                    if (!ignore) {
                        value = datListFromArray(1, &args[argAt]);
                    }
                    argAt++;
                }
            } else {
                die("Unknown repeat modifier.");
            }
        } else if (argAt >= argCount) {
            die("Too few arguments to function: %lld", argCount);
        } else {
            value = args[argAt];
            argAt++;
        }

        if (!ignore) {
            frameAdd(frame, name, value);
        }
    }
}

/**
 * Executes a variable definition, by updating the given execution frame,
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = datHighletValue(varDef);
    zvalue name = datMapGet(nameValue, STR_NAME);
    zvalue valueExpression = datMapGet(nameValue, STR_VALUE);
    zvalue value = execExpression(frame, valueExpression);

    frameAdd(frame, name, value);
}

/**
 * The C function that is bound to in order to execute interpreted code.
 */
static zvalue execClosure(zvalue state, zint argCount, const zvalue *args) {
    Closure *closure = datUniqletGetState(state, &CLOSURE_DISPATCH);
    zvalue functionNode = closure->function;
    zvalue parentContext = closure->context;

    zvalue yieldDef = datMapGet(functionNode, STR_YIELD_DEF);
    zvalue statements = datMapGet(functionNode, STR_STATEMENTS);
    zvalue yield = datMapGet(functionNode, STR_YIELD);
    YieldState *yieldState = NULL;

    // Take the parent context as a base, and bind the formals and
    // yieldDef (if present), creating an execution frame.

    Frame frame;
    frame.base = parentContext;
    frame.size = 0;
    bindArguments(&frame, functionNode, argCount, args);

    if (yieldDef != NULL) {
        yieldState = utilAlloc(sizeof(YieldState));
        yieldState->active = true;
        yieldState->result = NULL;

        if (setjmp(yieldState->jumpBuf) != 0) {
            // Here is where we land if and when `longjmp` is called.
            return yieldState->result;
        }

        zvalue exitFunction =
            langDefineFunction(nonlocalExit,
                               datUniqletWith(&YIELD_DISPATCH, yieldState));
        frameAdd(&frame, yieldDef, exitFunction);
    }

    // Evaluate the statements, updating the frame as needed.

    zint statementsSize = datSize(statements);
    for (zint i = 0; i < statementsSize; i++) {
        zvalue one = datListNth(statements, i);

        if (datHighletTypeIs(one, STR_VAR_DEF)) {
            execVarDef(&frame, one);
        } else {
            execExpressionVoidOk(&frame, one);
        }
    }

    // Evaluate the yield expression if present, and return the final
    // result.

    zvalue result = NULL;

    if (yield != NULL) {
        result = execExpressionVoidOk(&frame, yield);
    }

    if (yieldState != NULL) {
        yieldState->active = false;
    }

    return result;
}

/**
 * Executes a `function` form.
 */
static zvalue execFunction(Frame *frame, zvalue function) {
    datHighletAssertType(function, STR_FUNCTION);

    Closure *closure = utilAlloc(sizeof(Closure));
    closure->context = frameCollapse(frame);
    closure->function = datHighletValue(function);

    return langDefineFunction(execClosure,
                              datUniqletWith(&CLOSURE_DISPATCH, closure));
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    datHighletAssertType(call, STR_CALL);
    call = datHighletValue(call);

    zvalue function = datMapGet(call, STR_FUNCTION);
    zvalue actuals = datMapGet(call, STR_ACTUALS);
    zvalue functionId = execExpression(frame, function);

    zint argCount = datSize(actuals);
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        zvalue one = datListNth(actuals, i);
        args[i] = execExpression(frame, one);
    }

    return langCall(functionId, argCount, args);
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    datHighletAssertType(varRef, STR_VAR_REF);

    zvalue name = datHighletValue(varRef);
    zvalue found = frameGet(frame, name);

    if (found != NULL) {
        return found;
    }

    if (datTypeIs(name, DAT_STRING)) {
        zint nameSize = datUtf8SizeFromString(name);
        char nameStr[nameSize + 1];
        datUtf8FromString(nameSize + 1, nameStr, name);
        die("No such variable: %s", nameStr);
    }

    die("No such variable: (strange name)");
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    if (datHighletTypeIs(e, STR_LITERAL))
        return datHighletValue(e);
    else if (datHighletTypeIs(e, STR_VAR_REF))
        return execVarRef(frame, e);
    else if (datHighletTypeIs(e, STR_CALL))
        return execCall(frame, e);
    else if (datHighletTypeIs(e, STR_FUNCTION))
        return execFunction(frame, e);
    else {
        die("Invalid expression type.");
    }
}

/**
 * Executes an `expression` form, with the result never allowed to be
 * `void`.
 */
static zvalue execExpression(Frame *frame, zvalue expression) {
    zvalue result = execExpressionVoidOk(frame, expression);

    if (result == NULL) {
        die("Invalid use of void expression result.");
    }

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue langEvalExpressionNode(zvalue context, zvalue node) {
    Frame frame;
    frame.base = context;
    frame.size = 0;

    return execExpressionVoidOk(&frame, node);
}
