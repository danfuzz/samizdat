/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Nonlocal exit handling
 */

#include "impl.h"
#include "util.h"

#include <setjmp.h>
#include <stddef.h>


/*
 * Helper definitions
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
    jmp_buf jumpBuf;
} NleState;

/**
 * Marks a nonlocal exit state for garbage collection.
 */
static void nleMark(void *state) {
    NleState *nleState = state;
    datMark(nleState->result);
}

/**
 * Frees a nonlocal exit state.
 */
static void nleFree(void *state) {
    utilFree(state);
}

/** Uniqlet dispatch table for nonlocal exit states. */
static DatUniqletDispatch NLE_DISPATCH = {
    nleMark,
    nleFree
};

/**
 * The C function that is bound to in order to perform nonlocal exit.
 */
static zvalue nonlocalExit(zvalue state, zint argCount, const zvalue *args) {
    NleState *nleState = datUniqletGetState(state, &NLE_DISPATCH);

    if (nleState->active) {
        nleState->active = false;
    } else {
        die("Attempt to use out-of-scope nonlocal exit.");
    }

    switch (argCount) {
        case 0: {
            // Nothing to do.
            break;
        }
        case 1: {
            nleState->result = args[0];
            break;
        }
        default: {
            die("Too many arguments for nonlocal exit: %lld", argCount);
        }
    }

    longjmp(nleState->jumpBuf, 1);
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue nleCall(znleFunction function, void *state) {
    NleState *nleState = utilAlloc(sizeof(NleState));
    nleState->active = true;
    nleState->result = NULL;

    zint mark = debugMark();
    zstackPointer save = datFrameStart();

    if (setjmp(nleState->jumpBuf) != 0) {
        // Here is where we land if and when `longjmp` is called.
        zvalue result = nleState->result;
        debugReset(mark);
        datFrameReturn(save, result);
        return result;
    }

    zvalue exitFunction = langDefineFunction(nonlocalExit,
        datUniqletWith(&NLE_DISPATCH, nleState));
    zvalue result = function(state, exitFunction);

    nleState->active = false;
    datFrameReturn(save, result);
    return result;
}
