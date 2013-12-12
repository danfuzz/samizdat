/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Nonlocal exit handling
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"

#include <setjmp.h>


/*
 * Private Definitions
 */

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
    sigjmp_buf jumpBuf;
} NonlocalExitInfo;

/**
 * Gets a pointer to the info of a nonlocal exit.
 */
static NonlocalExitInfo *getInfo(zvalue nle) {
    return pbPayload(nle);
}

/**
 * Constructs and returns a nonlocal exit function.
 */
static zvalue makeNonlocalExit(void) {
    zvalue result = pbAllocValue(TYPE_NonlocalExit, sizeof(NonlocalExitInfo));

    NonlocalExitInfo *info = getInfo(result);
    info->active = true;
    info->result = NULL;

    return result;
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue nleCall(znleFunction function, void *state) {
    UTIL_TRACE_START(NULL, NULL);

    zstackPointer save = pbFrameStart();
    zvalue result;

    zvalue exitFunction = makeNonlocalExit();
    NonlocalExitInfo *info = getInfo(exitFunction);

    if (sigsetjmp(info->jumpBuf, 0) == 0) {
        // Here is where end up the first time `setjmp` returns.
        result = function(state, exitFunction);
    } else {
        // Here is where we land if and when `longjmp` is called.
        result = info->result;
    }

    info->active = false;
    pbFrameReturn(save, result);
    UTIL_TRACE_END();

    return result;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(NonlocalExit, call) {
    zvalue nle = args[0];
    NonlocalExitInfo *info = getInfo(nle);

    if (!info->active) {
        die("Attempt to use out-of-scope nonlocal exit.");
    }

    switch (argCount) {
        case 1: {
            // Result is `NULL`. Nothing to do.
            break;
        }
        case 2: {
            // First argument is the nonlocal exit function itself. Second
            // argument, when present, is the value to yield.
            info->result = args[1];
            break;
        }
        default: {
            die("Attempt to yield multiple values from nonlocal exit.");
        }
    }

    siglongjmp(info->jumpBuf, 1);
}

/* Documented in header. */
METH_IMPL(NonlocalExit, canCall) {
    zvalue nle = args[0];
    zvalue value = args[1];

    // Okay to call this with any first argument.
    return value;
}

/* Documented in header. */
METH_IMPL(NonlocalExit, gcMark) {
    zvalue nle = args[0];

    pbMark(getInfo(nle)->result);
    return NULL;
}

/** Initializes the module. */
MOD_INIT(NonlocalExit) {
    MOD_USE(Function);

    TYPE_NonlocalExit =
        coreTypeFromName(stringFromUtf8(-1, "NonlocalExit"), true);

    METH_BIND(NonlocalExit, call);
    METH_BIND(NonlocalExit, canCall);
    METH_BIND(NonlocalExit, gcMark);
}

/* Documented in header. */
zvalue TYPE_NonlocalExit = NULL;
