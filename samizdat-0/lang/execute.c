/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "lang.h"
#include "util.h"

#include <setjmp.h>
#include <stddef.h>
#include <string.h>


/*
 * Execution frames
 */

/**
 * Active execution frame. These are passed around within this file,
 * as code executes, and can become referenced by closures that are
 * released "in the wild".
 */
typedef struct Frame {
    /** Parent closure value. May be `NULL`. */
    zvalue parentClosure;

    /** Parent frame. May be `NULL`. */
    struct Frame *parentFrame;

    /** Variables defined in this frame, as a map from names to values. */
    zvalue vars;
} Frame;

static void frameMark(Frame *frame) {
    datMark(frame->vars);
    datMark(frame->parentClosure);
}

/**
 * Dies with a message, citing a variable name.
 */
static void dieForVariable(const char *message, zvalue name) {
    if (datTypeIs(name, DAT_STRING)) {
        zint nameSize = datUtf8SizeFromString(name);
        char nameStr[nameSize + 1];
        datUtf8FromString(nameSize + 1, nameStr, name);
        die("%s: %s", message, nameStr);
    }

    die("%s: (strange name)", message);
}

/**
 * Adds a new variable to the given frame.
 */
static void frameAdd(Frame *frame, zvalue name, zvalue value) {
    if (datMapGet(frame->vars, name) != NULL) {
        dieForVariable("Variable already defined", name);
    }

    frame->vars = datMapPut(frame->vars, name, value);
}

/**
 * Gets a variable's value out of the given frame.
 */
static zvalue frameGet(Frame *frame, zvalue name) {
    while (frame != NULL) {
        zvalue result = datMapGet(frame->vars, name);

        if (result != NULL) {
            return result;
        }

        frame = frame->parentFrame;
    }

    dieForVariable("Variable not defined", name);
    return NULL; // Keeps the compiler happy.
}

/**
 * Snapshots the given frame into the given target.
 */
static void frameSnap(Frame *target, Frame *source) {
    *target = *source;
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
     * Snapshot of the frame that was active at the moment the closure was
     * constructed.
     */
    Frame frame;

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

    frameMark(&closure->frame);
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
    zint argAt = 0;

    for (zint i = 0; i < formalsSize; i++) {
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
            die("Function called with too few arguments: %lld", argCount);
        } else {
            value = args[argAt];
            argAt++;
        }

        if (!ignore) {
            frameAdd(frame, name, value);
        }
    }

    if (argAt != argCount) {
        die("Function called with too many arguments: %lld", argCount);
    }
}

/**
 * Executes a variable definition, by updating the given execution frame,
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = datTokenValue(varDef);
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
    Frame *parentFrame = &closure->frame;

    zvalue yieldDef = datMapGet(functionNode, STR_YIELD_DEF);
    zvalue statements = datMapGet(functionNode, STR_STATEMENTS);
    zvalue yield = datMapGet(functionNode, STR_YIELD);
    YieldState *yieldState = NULL;

    // With the closure's frame as the parent, bind the formals and
    // yieldDef (if present), creating a new execution frame.

    Frame frame;
    frame.parentClosure = state;
    frame.parentFrame = parentFrame;
    frame.vars = EMPTY_MAP;
    bindArguments(&frame, functionNode, argCount, args);

    if (yieldDef != NULL) {
        yieldState = utilAlloc(sizeof(YieldState));
        yieldState->active = true;
        yieldState->result = NULL;

        zint mark = debugMark();

        if (setjmp(yieldState->jumpBuf) != 0) {
            // Here is where we land if and when `longjmp` is called.
            debugReset(mark);
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

        if (datTokenTypeIs(one, STR_VAR_DEF)) {
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
    datTokenAssertType(function, STR_FUNCTION);

    Closure *closure = utilAlloc(sizeof(Closure));

    frameSnap(&closure->frame, frame);
    closure->function = datTokenValue(function);

    return langDefineFunction(execClosure,
                              datUniqletWith(&CLOSURE_DISPATCH, closure));
}

/**
 * Helper for `execCall`. This is the function that handles emitting
 * a context string for a call, when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue expressionNode = state;

    if (datTokenTypeIs(expressionNode, STR_VAR_REF)) {
        zvalue name = datTokenValue(expressionNode);
        zint nameSize = datUtf8SizeFromString(name);
        char nameStr[nameSize + 1];
        datUtf8FromString(nameSize + 1, nameStr, name);
        return strdup(nameStr);
    } else {
        return "(unknown)";
    }
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    datTokenAssertType(call, STR_CALL);
    call = datTokenValue(call);

    zvalue function = datMapGet(call, STR_FUNCTION);
    zvalue actuals = datMapGet(call, STR_ACTUALS);
    zvalue functionId = execExpression(frame, function);

    zint argCount = datSize(actuals);
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        zvalue one = datListNth(actuals, i);
        args[i] = execExpression(frame, one);
    }

    debugPush(callReporter, function);
    zvalue result = langCall(functionId, argCount, args);
    debugPop();

    return result;
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    datTokenAssertType(varRef, STR_VAR_REF);

    zvalue name = datTokenValue(varRef);
    return frameGet(frame, name);
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    if (datTokenTypeIs(e, STR_LITERAL))
        return datTokenValue(e);
    else if (datTokenTypeIs(e, STR_VAR_REF))
        return execVarRef(frame, e);
    else if (datTokenTypeIs(e, STR_CALL))
        return execCall(frame, e);
    else if (datTokenTypeIs(e, STR_FUNCTION))
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
zvalue langEval0(zvalue context, zvalue node) {
    Frame frame;
    frame.parentClosure = NULL;
    frame.parentFrame = NULL;
    frame.vars = context;

    return execExpressionVoidOk(&frame, node);
}
