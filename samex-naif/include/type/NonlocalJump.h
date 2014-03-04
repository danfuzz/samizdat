/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `NonlocalJump` data type
 */

#ifndef _TYPE_NONLOCAL_JUMP_H_
#define _TYPE_NONLOCAL_JUMP_H_

#include "dat.h"
#include "type/Function.h"

#include <setjmp.h>


/** Type value for in-model type `NonlocalJump`. */
extern zvalue TYPE_NonlocalJump;

/**
 * Constructs and returns a nonlocal jump, which is initially invalid for
 * use. It becomes valid when `nonlocalJumpTarget()` is called.
 */
zvalue makeNonlocalJump(void);

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
} NonlocalJumpInfo;

/**
 * Sets the return point for the given nonlocal jump.
 */
#define nonlocalJumpTarget(nljump) \
    do { \
        NonlocalJumpInfo *info = datPayload((nljump)); \
        if (sigsetjmp(info->env, 0)) { \
            return info->result; \
        } \
        info->valid = true; \
    } while (0)

/**
 * Retires (invalidates) the given nonlocal jump.
 */
#define nonlocalJumpRetire(nljump) \
    do { \
        NonlocalJumpInfo *info = datPayload((nljump)); \
        info->valid = false; \
    } while (0)


#endif
