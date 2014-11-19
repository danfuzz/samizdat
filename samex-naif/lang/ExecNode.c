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
#include "type/List.h"

#include "impl.h"
#include "langnode.h"


//
// Private Definitions
//

/**
 * Payload data. This is approximately a union (in the math sense, not the C
 * sense) of all the possible named bindings of any executable node type.
 * Bound values which are themselves executable nodes are recursively
 * translated. Other values are in some cases left alone and in others
 * translated to a more convenient form for execution.
 */
typedef struct {
    /** Original node. */
    zvalue orig;

    /** Type of the node. */
    zevalType type;

    /** `node::box`. */
    zevalType box;

    /** `node::formals`. */
    zvalue formals;

    /** `node::name`. */
    zvalue name;

    /** `node::statements`. */
    zvalue statements;

    /** `node::target`. */
    zvalue target;

    /** `node::value`. */
    zvalue value;

    /** `node::values`. */
    zvalue values;

    /** `node::yield`. */
    zvalue yield;

    /** `node::yieldDef`. */
    zvalue yieldDef;
} ExecNodeInfo;

/**
 * Gets the info of a record.
 */
static ExecNodeInfo *getInfo(zvalue value) {
    return (ExecNodeInfo *) datPayload(value);
}

/**
 * Converts a single per-spec executable tree node into an `ExecNode`,
 * storing the converted result back through the given pointer. As a
 * convenience, if `*orig` is `NULL`, this function does nothing.
 */
static void convertNode(zvalue *orig) {
    if (*orig != NULL) {
        *orig = cm_new(ExecNode, *orig);
    }
}

/**
 * Converts each of the elements of the given list into an `ExecNode`,
 * storing the converted list of all the conversions back through the given
 * pointer.
 */
static void convertList(zvalue *orig) {
    zarray arr = zarrayFromList(*orig);
    zvalue result[arr.size];

    for (zint i = 0; i < arr.size; i++) {
        result[i] = arr.elems[i];
        convertNode(&result[i]);
    }

    *orig = listFromZarray((zarray) {arr.size, result});
}


//
// Module Definitions
//

// TODO


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
    info->orig = orig;

    switch (type) {
        case EVAL_apply:
        case EVAL_call: {
            if (!recGet3(orig,
                    SYM(target), &info->target,
                    SYM(name),   &info->name,
                    SYM(values), &info->values)) {
                die("Invalid `apply` or `call` node.");
            }

            convertNode(&info->target);
            convertNode(&info->name);
            if (type == EVAL_call) {
                convertList(&info->values);
            }

            break;
        }

        case EVAL_closure: {
            if (!recGet3(orig,
                    SYM(formals),    &info->formals,
                    SYM(statements), &info->statements,
                    SYM(yield),      &info->yield)) {
                die("Invalid `closure` node.");
            }

            // These are both optional.
            recGet2(orig,
                SYM(name),     &info->name,
                SYM(yieldDef), &info->yieldDef);

            convertList(&info->statements);
            convertNode(&info->yield);
            break;
        }

        case EVAL_fetch: {
            if (!recGet1(orig, SYM(target), &info->target)) {
                die("Invalid `fetch` node.");
            }

            convertNode(&info->target);
            break;
        }

        case EVAL_importModule:
        case EVAL_importModuleSelection:
        case EVAL_importResource: {
            info->statements = makeDynamicImport(orig);
            convertNode(&info->statements);
            break;
        }

        case EVAL_literal:
        case EVAL_maybe:
        case EVAL_noYield: {
            if (!recGet1(orig, SYM(value), &info->value)) {
                die("Invalid `literal`, `maybe`, or `noYield` node.");
            }

            if (type != EVAL_literal) {
                convertNode(&info->value);
            }

            break;
        }

        case EVAL_store: {
            if (!recGet2(orig,
                    SYM(target), &info->target,
                    SYM(value),  &info->value)) {
                die("Invalid `store` node.");
            }

            convertNode(&info->target);
            convertNode(&info->value);
            break;
        }

        case EVAL_varDef: {
            zvalue box;
            if (!recGet3(orig,
                    SYM(box),   &box,
                    SYM(name),  &info->name,
                    SYM(value), &info->value)) {
                die("Invalid `varDef` node.");
            }

            info->box = symbolEvalType(box);
            switch (info->box) {
                case EVAL_cell:
                case EVAL_lazy:
                case EVAL_promise:
                case EVAL_result: {
                    // These are all valid.
                    break;
                }
                default: {
                    die("Invalid `box` spec: %s", cm_debugString(box));
                }
            }

            convertNode(&info->value);
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

    datMark(info->orig);
    datMark(info->formals);
    datMark(info->name);
    datMark(info->statements);
    datMark(info->target);
    datMark(info->value);
    datMark(info->values);
    datMark(info->yield);
    datMark(info->yieldDef);

    return NULL;
}

/** Initializes the module. */
MOD_INIT(ExecNode) {
    MOD_USE(cls);

    CLS_ExecNode = makeCoreClass(symbolFromUtf8(-1, "ExecNode"), CLS_Core,
        METH_TABLE(
            CMETH_BIND(ExecNode, new)),
        METH_TABLE(
            METH_BIND(ExecNode, debugSymbol),
            METH_BIND(ExecNode, gcMark)));
}

// Documented in header.
zvalue CLS_ExecNode = NULL;
