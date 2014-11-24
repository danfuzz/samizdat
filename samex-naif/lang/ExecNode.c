// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `ExecNode` class
//
// Instances are translated from the (per spec) executable tree node form.
// These are what are used when actually running the interpreter.

#include "const.h"
#include "type/define.h"
#include "type/Box.h"
#include "type/List.h"
#include "type/SymbolTable.h"

#include "impl.h"
#include "langnode.h"


//
// Private Definitions
//

/**
 * Payload data. This is approximately a union (in the math sense, not the C
 * sense) of all the possible named bindings of any executable node type,
 * *except* for `closure` (which gets its own class). Bound values which are
 * themselves executable nodes are recursively translated. Other values are in
 * some cases left alone and in others translated to a more convenient form
 * for execution.
 */
typedef struct {
    /** Type of the node. */
    zevalType type;

    /** `node::box`. */
    zvalue box;

    /** `node::name`. */
    zvalue name;

    /** `node::target`. */
    zvalue target;

    /** `node::value`. Also used to hold `ClosureNode`s. */
    zvalue value;

    /** `node::values`. Also used to hold the `statements` of an `import*`. */
    zvalue values;

    /** `zarray` pointer into `values`, when useful. */
    zarray valuesArr;
} ExecNodeInfo;

/**
 * Gets the info of a record.
 */
static ExecNodeInfo *getInfo(zvalue value) {
    return (ExecNodeInfo *) datPayload(value);
}

/**
 * Identifies the variant of execution.
 */
typedef enum {
    EX_statement,  // Yield ignored; also allows `varDef` and `import*`.
    EX_value,      // Must yield a value (not void).
    EX_maybe,      // This allows both `maybe` and `void`.
    EX_voidOk      // Allowed to yield void.
} zexecOperation;

/**
 * Executes a single `ExecNode`. `op` identifies the variant. Can return
 * `NULL`.
 */
static zvalue execute(zvalue node, Frame *frame, zexecOperation op) {
    ExecNodeInfo *info = getInfo(node);

    zvalue result;
    switch (info->type) {
        case EVAL_apply: {
            zvalue target = execute(info->target, frame, EX_value);
            zvalue name = execute(info->name, frame, EX_value);
            zvalue values = execute(info->values, frame, EX_maybe);

            result = methApply(target, name, values);
            break;
        }

        case EVAL_call: {
            zvalue target = execute(info->target, frame, EX_value);
            zvalue name = execute(info->name, frame, EX_value);
            zarray values = info->valuesArr;

            // Evaluate each argument expression.
            zvalue args[values.size];
            for (zint i = 0; i < values.size; i++) {
                args[i] = execute(values.elems[i], frame, EX_value);
            }

            result = methCall(target, name, (zarray) {values.size, args});
            break;
        }

        case EVAL_closure: {
            result = exnoBuildClosure(info->value, frame);
            break;
        }

        case EVAL_fetch: {
            zvalue target = execute(info->target, frame, EX_value);

            result = cm_fetch(target);
            break;
        }

        case EVAL_importModule:
        case EVAL_importModuleSelection:
        case EVAL_importResource: {
            if (op != EX_statement) {
                die("Invalid use of `import*` node.");
            }

            exnoExecuteStatements(info->valuesArr, frame);
            return NULL;
        }

        case EVAL_literal: {
            result = info->value;
            break;
        }

        case EVAL_maybe: {
            if (op != EX_maybe) {
                die("Invalid use of `maybe` node.");
            }

            // Return directly, to avoid the non-void check.
            return execute(info->value, frame, EX_voidOk);
        }

        case EVAL_noYield: {
            mustNotYield(execute(info->value, frame, EX_voidOk));
            // `mustNotYield` will `die` before trying to return here.
        }

        case EVAL_store: {
            zvalue target = execute(info->target, frame, EX_value);
            zvalue value = execute(info->value, frame, EX_maybe);

            result = cm_store(target, value);
            break;
        }

        case EVAL_varDef: {
            if (op != EX_statement) {
                die("Invalid use of `varDef` node.");
            }

            zvalue value = execute(info->value, frame, EX_maybe);
            zvalue boxInstance = (value == NULL)
                ? METH_CALL(info->box, new)
                : METH_CALL(info->box, new, value);

            frameDef(frame, info->name, boxInstance);
            return NULL;
        }

        case EVAL_varRef: {
            result = frameGet(frame, info->name);
            break;
        }

        case EVAL_void: {
            if (op != EX_maybe) {
                die("Invalid use of `void` node.");
            }

            return NULL;
        }

        default: {
            die("Invalid type (shouldn't happen): %d", info->type);
        }
    }

    // Note: Some cases above return directly, so as to properly avoid this
    // check.
    switch (op) {
        case EX_statement: { return NULL;   }
        case EX_voidOk:    { return result; }
        default: {
            if (result == NULL) {
                die("Invalid use of void expression result.");
            }
            return result;
        }
    }
}


//
// Module Definitions
//

// Documented in header.
void exnoConvert(zvalue *orig) {
    if (*orig == NULL) {
        // Nothing to do.
    } else if (classAccepts(CLS_Record, *orig)) {
        *orig = cm_new(ExecNode, *orig);
    } else {
        // Assumed to be a list.
        zarray arr = zarrayFromList(*orig);
        zvalue result[arr.size];

        for (zint i = 0; i < arr.size; i++) {
            result[i] = arr.elems[i];
            exnoConvert(&result[i]);
        }

        *orig = listFromZarray((zarray) {arr.size, result});
    }
}

// Documented in header.
zvalue exnoExecute(zvalue node, Frame *frame) {
    assertHasClass(node, CLS_ExecNode);
    return execute(node, frame, EX_maybe);
}

// Documented in header.
void exnoExecuteStatements(zarray statements, Frame *frame) {
    for (zint i = 0; i < statements.size; i++) {
        execute(statements.elems[i], frame, EX_statement);
    }
}

// Documented in header.
zvalue exnoVarDefName(zvalue node) {
    ExecNodeInfo *info = getInfo(node);

    return (info->type == EVAL_varDef)
        ? info->name
        : NULL;
}


//
// Exported Definitions
//

// Documented in header.
zvalue langEval0(zvalue env, zvalue node) {
    zint size = get_size(env);
    zmapping mappings[size];

    arrayFromSymtab(mappings, env);
    for (zint i = 0; i < size; i++) {
        mappings[i].value = cm_new(Result, mappings[i].value);
    }
    env = symtabFromZassoc((zassoc) {size, mappings});

    Frame frame;
    frameInit(&frame, NULL, NULL, env);

    exnoConvert(&node);
    return exnoExecute(node, &frame);
}


//
// Class Definition
//

/**
 * Constructs an instance from the given (per spec) executable tree node.
 */
CMETH_IMPL_1(ExecNode, new, orig) {
    zevalType type = recordEvalType(orig);
    zvalue result = datAllocValue(CLS_ExecNode, sizeof(ExecNodeInfo));
    ExecNodeInfo *info = getInfo(result);

    info->type = type;

    switch (type) {
        case EVAL_apply:
        case EVAL_call: {
            if (!recGet3(orig,
                    SYM(target), &info->target,
                    SYM(name),   &info->name,
                    SYM(values), &info->values)) {
                die("Invalid `apply` or `call` node.");
            }

            exnoConvert(&info->target);
            exnoConvert(&info->name);
            exnoConvert(&info->values);

            if (type == EVAL_call) {
                info->valuesArr = zarrayFromList(info->values);
            }

            break;
        }

        case EVAL_closure: {
            info->value = cm_new(ClosureNode, orig);
            break;
        }

        case EVAL_fetch: {
            if (!recGet1(orig, SYM(target), &info->target)) {
                die("Invalid `fetch` node.");
            }

            exnoConvert(&info->target);
            break;
        }

        case EVAL_importModule:
        case EVAL_importModuleSelection:
        case EVAL_importResource: {
            info->values = makeDynamicImport(orig);
            exnoConvert(&info->values);
            info->valuesArr = zarrayFromList(info->values);
            break;
        }

        case EVAL_literal:
        case EVAL_maybe:
        case EVAL_noYield: {
            if (!recGet1(orig, SYM(value), &info->value)) {
                die("Invalid `literal`, `maybe`, or `noYield` node.");
            }

            if (type != EVAL_literal) {
                exnoConvert(&info->value);
            }

            break;
        }

        case EVAL_store: {
            if (!recGet2(orig,
                    SYM(target), &info->target,
                    SYM(value),  &info->value)) {
                die("Invalid `store` node.");
            }

            exnoConvert(&info->target);
            exnoConvert(&info->value);
            break;
        }

        case EVAL_varDef: {
            if (!recGet3(orig,
                    SYM(box),   &info->box,
                    SYM(name),  &info->name,
                    SYM(value), &info->value)) {
                die("Invalid `varDef` node.");
            }

            switch (symbolEvalType(info->box)) {
                case EVAL_cell:    { info->box = CLS_Cell;    break; }
                case EVAL_lazy:    { info->box = CLS_Lazy;    break; }
                case EVAL_promise: { info->box = CLS_Promise; break; }
                case EVAL_result:  { info->box = CLS_Result;  break; }
                default: {
                    die("Invalid `box` spec: %s", cm_debugString(info->box));
                }
            }

            exnoConvert(&info->value);
            break;
        }

        case EVAL_varRef: {
            if (!recGet1(orig, SYM(name), &info->name)) {
                die("Invalid `varRef` node.");
            }
            break;
        }

        case EVAL_void: {
            // Nothing to do.
            break;
        }

        default: {
            die("Invalid node: %s", cm_debugString(orig));
        }
    }

    return result;
}

// Documented in spec.
METH_IMPL_0(ExecNode, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(ExecNode, gcMark) {
    ExecNodeInfo *info = getInfo(ths);

    datMark(info->box);
    datMark(info->name);
    datMark(info->target);
    datMark(info->value);
    datMark(info->values);

    return NULL;
}

/** Initializes the module. */
MOD_INIT(ExecNode) {
    MOD_USE(cls);
    MOD_USE(Closure);
    MOD_USE(ClosureNode);

    CLS_ExecNode = makeCoreClass(SYM(ExecNode), CLS_Core,
        METH_TABLE(
            CMETH_BIND(ExecNode, new)),
        METH_TABLE(
            METH_BIND(ExecNode, debugSymbol),
            METH_BIND(ExecNode, gcMark)));
}

// Documented in header.
zvalue CLS_ExecNode = NULL;
