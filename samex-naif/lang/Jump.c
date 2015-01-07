// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Nonlocal jump (exit / yield) functions
//

// Needed by `Jump.h`. See that file for further info.
#define _XOPEN_SOURCE 700

#include "type/Jump.h"
#include "type/String.h"
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

// Documented in spec.
METH_IMPL_rest(Jump, call, args) {
    JumpInfo *info = getInfo(ths);

    if (!info->valid) {
        die("Out-of-scope nonlocal jump.");
    }

    switch (args.size) {
        case 0:  { info->result = NULL;          break;              }
        case 1:  { info->result = args.elems[0]; break;              }
        default: { die("Invalid argument count for nonlocal jump."); }
    }

    info->valid = false;
    siglongjmp(info->env, 1);
}

// Documented in spec.
METH_IMPL_0(Jump, debugString) {
    JumpInfo *info = getInfo(ths);
    zvalue validStr = info->valid ? EMPTY_STRING : stringFromUtf8(-1, "in");

    return cm_cat(
        stringFromUtf8(-1, "@<Jump "),
        validStr,
        stringFromUtf8(-1, "valid>"));
}

// Documented in header.
METH_IMPL_0(Jump, gcMark) {
    JumpInfo *info = getInfo(ths);

    datMark(info->result);
    return NULL;
}

/** Initializes the module. */
MOD_INIT(Jump) {
    MOD_USE(Value);

    CLS_Jump = makeCoreClass(SYM(Jump), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(Jump, call),
            METH_BIND(Jump, debugString),
            METH_BIND(Jump, gcMark)));
}

// Documented in header.
zvalue CLS_Jump = NULL;
