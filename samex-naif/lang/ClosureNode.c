// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `ClosureNode` class
//
// Translation of the main info of a `closure` node.

// Needed by `Jump.h`. See that file for further info.
#define _XOPEN_SOURCE 700

#include "langnode.h"
#include "type/define.h"
#include "type/Box.h"
#include "type/Jump.h"
#include "type/List.h"
#include "type/SymbolTable.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Repetition style of a formal argument.
 */
typedef enum {
    REP_NONE,
    REP_QMARK,
    REP_STAR,
    REP_PLUS
} zrepeat;

/**
 * Formal argument.
 */
typedef struct {
    /** Name (optional). */
    zvalue name;

    /** Repetition style. */
    zrepeat repeat;
} zformal;

/**
 * Payload data. This corresponds with the payload of a `closure` node, but
 * with `formals` reworked to be easier to digest.
 */
typedef struct {
    /** The `node::formals`, converted for easier use. */
    zformal formals[LANG_MAX_FORMALS];

    /** The result of `get_size(formals)`. */
    zint formalsSize;

    /** The number of actual names in `formals`, plus one for a `yieldDef`. */
    zint formalsNameCount;

    /** `node::name`. */
    zvalue name;

    /** `node::statements`. */
    zvalue statements;

    /** `zarray` pointer into `statements`. */
    zarray statementsArr;

    /** `node::yield`. */
    zvalue yield;

    /** `node::yieldDef`. */
    zvalue yieldDef;
} ClosureNodeInfo;

/**
 * Gets the info of a record.
 */
static ClosureNodeInfo *getInfo(zvalue value) {
    return (ClosureNodeInfo *) datPayload(value);
}

/**
 * Helper for `convertFormals` which does name duplication detection.
 */
static void detectDuplicates(zint size, zvalue *arr, const char *kind) {
    if (size <= 1) {
        return;
    }

    symbolSort(size, arr);
    for (zint i = 1; i < size; i++) {
        if (arr[i - 1] == arr[i]) {
            zvalue nameStr = cm_castFrom(CLS_String, arr[i]);
            die("Duplicate %s name: %s", kind, cm_debugString(nameStr));
        }
    }
}

/**
 * Converts the given `formals`, storing the result in the given `info`.
 */
static void convertFormals(ClosureNodeInfo *info, zvalue formalsList) {
    zarray formals = zarrayFromList(formalsList);
    zarray statements = zarrayFromList(info->statements);

    if (formals.size > LANG_MAX_FORMALS) {
        die("Too many formals: %d", formals.size);
    }

    // The `names` array is for detecting duplicates.
    zvalue names[formals.size + statements.size + 1];
    zint nameCount = 0;

    if (info->yieldDef != NULL) {
        names[0] = info->yieldDef;
        nameCount = 1;
    }

    for (zint i = 0; i < formals.size; i++) {
        zvalue formal = formals.elems[i];
        zvalue name, repeat;
        zrepeat rep;

        recGet2(formal, SYM(name), &name, SYM(repeat), &repeat);

        if (name != NULL) {
            names[nameCount] = name;
            nameCount++;
        }

        if (repeat == NULL) {
            rep = REP_NONE;
        } else {
            switch (nodeSymbolType(repeat)) {
                case NODE_CH_STAR:  { rep = REP_STAR;  break; }
                case NODE_CH_PLUS:  { rep = REP_PLUS;  break; }
                case NODE_CH_QMARK: { rep = REP_QMARK; break; }
                default: {
                    die("Invalid repeat modifier: %s", cm_debugString(repeat));
                }
            }
        }

        info->formals[i] = (zformal) {.name = name, .repeat = rep};
    }

    // Detect duplicate formal argument names.

    detectDuplicates(nameCount, names, "formal argument");
    info->formalsSize = formals.size;
    info->formalsNameCount = nameCount;

    // Detect duplicate variable names.

    for (zint i = 0; i < statements.size; i++) {
        zvalue name = exnoVarDefName(statements.elems[i]);
        if (name != NULL) {
            names[nameCount] = name;
            nameCount++;
        }
    }

    detectDuplicates(nameCount, names, "variable");
}

/**
 * Creates a variable table for all the formal arguments of the given
 * function.
 */
static zvalue bindArguments(ClosureNodeInfo *info, zvalue exitFunction,
        zarray args) {
    zmapping elems[info->formalsNameCount];
    zformal *formals = info->formals;
    zint formalsSize = info->formalsSize;
    zint elemAt = 0;
    zint argAt = 0;

    for (zint i = 0; i < formalsSize; i++) {
        zvalue name = formals[i].name;
        zrepeat repeat = formals[i].repeat;
        bool ignore = (name == NULL);
        zvalue value;

        if (repeat != REP_NONE) {
            zint count;

            switch (repeat) {
                case REP_STAR: {
                    count = args.size - argAt;
                    break;
                }
                case REP_PLUS: {
                    if (argAt >= args.size) {
                        die("Function called with too few arguments "
                            "(plus argument): %d",
                            args.size);
                    }
                    count = args.size - argAt;
                    break;
                }
                case REP_QMARK: {
                    count = (argAt >= args.size) ? 0 : 1;
                    break;
                }
                default: {
                    die("Invalid repeat enum (shouldn't happen).");
                }
            }

            value = ignore
                ? NULL
                : listFromZarray((zarray) {count, &args.elems[argAt]});
            argAt += count;
        } else if (argAt >= args.size) {
            die("Function called with too few arguments: %d", args.size);
        } else {
            value = args.elems[argAt];
            argAt++;
        }

        if (!ignore) {
            elems[elemAt].key = name;
            elems[elemAt].value = cm_new(Result, value);
            elemAt++;
        }
    }

    if (argAt != args.size) {
        die("Function called with too many arguments: %d > %d",
            args.size, argAt);
    }

    if (exitFunction != NULL) {
        elems[elemAt].key = info->yieldDef;
        elems[elemAt].value = cm_new(Result, exitFunction);
        elemAt++;
    }

    return symtabFromZassoc((zassoc) {elemAt, elems});
}

/**
 * Helper that does the main work of `exnoCallClosure`, including nonlocal
 * exit binding when appropriate.
 */
static zvalue callClosureMain(zvalue node, Frame *parentFrame,
        zvalue parentClosure, zvalue exitFunction, zarray args) {
    ClosureNodeInfo *info = getInfo(node);

    // With the closure's frame as the parent, bind the formals and
    // nonlocal exit (if present), creating a new execution frame.

    Frame frame;
    zvalue argTable = bindArguments(info, exitFunction, args);
    frameInit(&frame, parentFrame, parentClosure, argTable);

    // Execute the statements, updating the frame as needed.
    exnoExecuteStatements(info->statementsArr, &frame);

    // Execute the yield expression, and return the final result.
    return exnoExecute(info->yield, &frame);
}


//
// Module Definitions
//

// Documented in header.
zvalue exnoCallClosure(zvalue node, Frame *parentFrame, zvalue parentClosure,
        zarray args) {
    if (getInfo(node)->yieldDef == NULL) {
        return callClosureMain(node, parentFrame, parentClosure, NULL, args);
    }

    zvalue jump = makeJump();
    jumpArm(jump);

    zvalue result =
        callClosureMain(node, parentFrame, parentClosure, jump, args);
    jumpRetire(jump);

    return result;
}


//
// Class Definition
//

/**
 * Constructs an instance from the given (per spec) `closure` tree node.
 */
CMETH_IMPL_1(ClosureNode, new, orig) {
    zvalue result = datAllocValue(CLS_ClosureNode, sizeof(ClosureNodeInfo));
    ClosureNodeInfo *info = getInfo(result);
    zvalue formals;

    if (!recGet3(orig,
            SYM(formals),    &formals,
            SYM(statements), &info->statements,
            SYM(yield),      &info->yield)) {
        die("Invalid `closure` node.");
    }

    // These are both optional.
    recGet2(orig,
        SYM(name),     &info->name,
        SYM(yieldDef), &info->yieldDef);

    exnoConvert(&info->statements);
    exnoConvert(&info->yield);
    convertFormals(info, formals);

    info->statementsArr = zarrayFromList(info->statements);

    return result;
}

// Documented in spec.
METH_IMPL_0(ClosureNode, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(ClosureNode, gcMark) {
    ClosureNodeInfo *info = getInfo(ths);

    datMark(info->name);
    datMark(info->statements);
    datMark(info->yield);
    datMark(info->yieldDef);

    for (zint i = 0; i < info->formalsSize; i++) {
        datMark(info->formals[i].name);
    }

    return NULL;
}

/** Initializes the module. */
MOD_INIT(ClosureNode) {
    MOD_USE(cls);
    MOD_USE(Jump);

    CLS_ClosureNode = makeCoreClass(SYM(ClosureNode), CLS_Core,
        METH_TABLE(
            CMETH_BIND(ClosureNode, new)),
        METH_TABLE(
            METH_BIND(ClosureNode, debugSymbol),
            METH_BIND(ClosureNode, gcMark)));
}

// Documented in header.
zvalue CLS_ClosureNode = NULL;
