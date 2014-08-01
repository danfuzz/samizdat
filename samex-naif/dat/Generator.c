// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Generator protocol
//

#include "type/Box.h"
#include "type/Builtin.h"
#include "type/Generator.h"
#include "type/List.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Class Definition: `Generator`
//

/** "Standard" `collect` implementation. Documented in spec. */
METH_IMPL(Generator, stdCollect) {
    zvalue generator = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;
    zvalue stackArr[DAT_MAX_GENERATOR_ITEMS_SOFT];
    zvalue *arr = stackArr;
    zint maxSize = DAT_MAX_GENERATOR_ITEMS_SOFT;
    zint at = 0;

    zstackPointer save = datFrameStart();
    zvalue box = makeCell(NULL);

    for (;;) {
        zvalue nextGen = METH_CALL(nextValue, generator, box);

        if (nextGen == NULL) {
            break;
        }

        zvalue one = METH_CALL(fetch, box);
        generator = nextGen;

        if (function != NULL) {
            one = FUN_CALL(function, one);
            if (one == NULL) {
                continue;
            }
        } else if (one == NULL) {
            die("Unexpected lack of result.");
        }

        if (at == maxSize) {
            if (arr == stackArr) {
                maxSize = DAT_MAX_GENERATOR_ITEMS_HARD;
                arr = utilAlloc(maxSize * sizeof(zvalue));
                utilCpy(zvalue, arr, stackArr, at);
            } else {
                die("Generator produced way too many items.");
            }
        }

        arr[at] = one;
        at++;
    }

    zvalue result = listFromArray(at, arr);
    datFrameReturn(save, result);

    if (arr != stackArr) {
        utilFree(arr);
    }

    return result;
}

/** "Standard" `fetch` implementation. Documented in spec. */
METH_IMPL(Generator, stdFetch) {
    zvalue generator = args[0];
    zvalue result;

    zvalue box = makeCell(NULL);
    zvalue nextGen = METH_CALL(nextValue, generator, box);

    if (nextGen == NULL) {
        // We were given a voided generator.
        result = NULL;
    } else {
        // We got a value out of the generator. Now need to make sure it's
        // voided.
        result = METH_CALL(fetch, box);
        if (METH_CALL(nextValue, nextGen, box) != NULL) {
            die("Generator produced second item in `fetch`.");
        }
    }

    return result;
}

/** Initializes the module. */
MOD_INIT(Generator) {
    MOD_USE_NEXT(Box);
    MOD_USE_NEXT(List);

    SEL_INIT(collect);
    SEL_INIT(fetch);
    SEL_INIT(nextValue);

    FUN_Generator_stdCollect = makeBuiltin(1, 2,
        METH_NAME(Generator, stdCollect), 0,
        stringFromUtf8(-1, "Generator.stdCollect"));
    datImmortalize(FUN_Generator_stdCollect);

    FUN_Generator_stdFetch = makeBuiltin(1, 1,
        METH_NAME(Generator, stdFetch), 0,
        stringFromUtf8(-1, "Generator.stdFetch"));
    datImmortalize(FUN_Generator_stdFetch);
}

// Documented in header.
SEL_DEF(collect);

// Documented in header.
SEL_DEF(fetch);

// Documented in header.
SEL_DEF(nextValue);

// Documented in header.
zvalue FUN_Generator_stdCollect = NULL;

// Documented in header.
zvalue FUN_Generator_stdFetch = NULL;
