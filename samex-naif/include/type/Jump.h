// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Jump` class
//

#ifndef _TYPE_JUMP_H_
#define _TYPE_JUMP_H_

// Needed for `sigjmp_buf` when using glibc.
//
// Note: Rather than define this directly here, the following setup causes the
// compiler to bail if the source that `#include`s this file fails to define
// the constant appropriately. This avoids having the feature macro defined
// inconsistently within a single file.
#ifndef _XOPEN_SOURCE
#error Jump.h requires: `#define _XOPEN_SOURCE 700`
#elif _XOPEN_SOURCE != 700
#error Jump.h requires: `#define _XOPEN_SOURCE 700`
#endif

#include <setjmp.h>
#include <stdbool.h>

#include "type/Value.h"


/** Class value for in-model class `Jump`. */
extern zvalue CLS_Jump;

/**
 * Constructs and returns a nonlocal jump, which is initially invalid for
 * use. It becomes valid when `jumpArm()` is called.
 */
zvalue makeJump(void);

/**
 * Jump function structure. This is defined here so that the exported macros
 * can access it.
 */
typedef struct {
    /** Environment struct for use with `sigsetjmp` et al. */
    sigjmp_buf env;

    /** Whether the function is valid / usable (in scope, dynamically). */
    bool valid;

    /** What to return when jumped to. */
    zvalue result;
} JumpInfo;

/**
 * Sets the return point for the given nonlocal jump.
 */
#define jumpArm(jump) \
    do { \
        JumpInfo *info = datPayload((jump)); \
        zstackPointer save = datFrameStart(); \
        if (sigsetjmp(info->env, 0)) { \
            zvalue result = info->result; \
            datFrameReturn(save, result); \
            return result; \
        } \
        info->valid = true; \
    } while (0)

/**
 * Retires (invalidates) the given nonlocal jump.
 */
#define jumpRetire(jump) \
    do { \
        JumpInfo *info = datPayload((jump)); \
        info->valid = false; \
    } while (0)


#endif
