// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Nonlocal jump (exit / yield) functions
//

#include "type/Jump.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Gets a pointer to the value's info.
 */
static JumpInfo *getInfo(zvalue jump) {
    return datPayload(jump);
}


//
// Module Definitions
//

// Documented in header.
zvalue jumpCall(zvalue jump, zint argCount, const zvalue *args) {
    JumpInfo *info = getInfo(jump);

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


//
// Exported Definitions
//

// Documented in header.
zvalue makeJump(void) {
    zvalue result = datAllocValue(CLS_Jump, sizeof(JumpInfo));
    JumpInfo *info = getInfo(result);

    info->valid = false;
    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Jump, call) {
    // The first argument is the jump per se, and the rest are the
    // arguments to call it with.
    return jumpCall(args[0], argCount - 1, &args[1]);
}

// Documented in header.
METH_IMPL(Jump, debugString) {
    zvalue jump = args[0];
    JumpInfo *info = getInfo(jump);
    zvalue validStr = info->valid ? EMPTY_STRING : stringFromUtf8(-1, "in");

    return METH_CALL(cat,
        stringFromUtf8(-1, "@(Jump "),
        validStr,
        stringFromUtf8(-1, "valid)"));
}

// Documented in header.
METH_IMPL(Jump, gcMark) {
    zvalue jump = args[0];
    JumpInfo *info = getInfo(jump);

    datMark(info->result);
    return NULL;
}

/** Initializes the module. */
MOD_INIT(Jump) {
    MOD_USE(Function);

    // Note: The `objectModel` module initializes `CLS_Jump`.
    classBindMethods(CLS_Jump,
        NULL,
        selectorTableFromArgs(
            SEL_METH(Jump, call),
            SEL_METH(Jump, debugString),
            SEL_METH(Jump, gcMark),
            NULL));
}

// Documented in header.
zvalue CLS_Jump = NULL;
