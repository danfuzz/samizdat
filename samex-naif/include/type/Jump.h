/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Jump` data type
 */

#ifndef _TYPE_JUMP_H_
#define _TYPE_JUMP_H_

#include "dat.h"
#include "type/Function.h"

#include <setjmp.h>


/** Type value for in-model type `Jump`. */
extern zvalue TYPE_Jump;

/**
 * Constructs and returns a nonlocal jump, which is initially invalid for
 * use. It becomes valid when `JumpArm()` is called.
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
