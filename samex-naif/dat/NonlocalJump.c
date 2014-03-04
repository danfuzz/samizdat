/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Nonlocal jump (exit / yield) functions
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/NonlocalJump.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"


/*
 * Private Definitions
 */

/**
 * Gets a pointer to the value's info.
 */
static NonlocalJumpInfo *getInfo(zvalue nljump) {
    return datPayload(nljump);
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue nljumpCall(zvalue nljump, zint argCount, const zvalue *args) {
    NonlocalJumpInfo *info = getInfo(nljump);

    if (!info->valid) {
        die("Out-of-scope nonlocal jump.");
    }

    switch (argCount) {
        case 0:  { info->result = NULL;    break;      }
        case 1:  { info->result = args[0]; break;      }
        default: { die("Out-of-scope nonlocal jump."); }
    }

    info->valid = false;
    siglongjmp(info->env, 1);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue makeNonlocalJump(void) {
    zvalue result = datAllocValue(TYPE_NonlocalJump, sizeof(NonlocalJumpInfo));
    NonlocalJumpInfo *info = getInfo(result);

    info->valid = false;
    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(NonlocalJump, call) {
    // The first argument is the nljump per se, and the rest are the
    // arguments to call it with.
    return nljumpCall(args[0], argCount - 1, &args[1]);
}

/* Documented in header. */
METH_IMPL(NonlocalJump, canCall) {
    zvalue nljump = args[0];
    zvalue value = args[1];

    // A nonlocal jump accepts any first argument.
    return value;
}

/* Documented in header. */
METH_IMPL(NonlocalJump, debugString) {
    zvalue nljump = args[0];
    NonlocalJumpInfo *info = getInfo(nljump);
    zvalue validStr = info->valid ? EMPTY_STRING : stringFromUtf8(-1, "in");

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@(NonlocalJump "),
        validStr,
        stringFromUtf8(-1, "valid)"));
}

/* Documented in header. */
METH_IMPL(NonlocalJump, gcMark) {
    zvalue nljump = args[0];
    NonlocalJumpInfo *info = getInfo(nljump);

    datMark(info->result);
    return NULL;
}

/** Initializes the module. */
MOD_INIT(NonlocalJump) {
    MOD_USE(Function);

    TYPE_NonlocalJump =
        coreTypeFromName(stringFromUtf8(-1, "NonlocalJump"), true);

    METH_BIND(NonlocalJump, call);
    METH_BIND(NonlocalJump, canCall);
    METH_BIND(NonlocalJump, debugString);
    METH_BIND(NonlocalJump, gcMark);
}

/* Documented in header. */
zvalue TYPE_NonlocalJump = NULL;
