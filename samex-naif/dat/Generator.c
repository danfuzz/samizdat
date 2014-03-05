/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Generator protocol
 */

#include "impl.h"
#include "type/Box.h"
#include "type/Builtin.h"
#include "type/Generator.h"
#include "type/Generic.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"


/*
 * Type Definition: `Generator`
 */

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
        zvalue nextGen = GFN_CALL(nextValue, generator, box);

        if (nextGen == NULL) {
            break;
        }

        zvalue one = GFN_CALL(fetch, box);
        generator = nextGen;

        // Ideally, we wouldn't reuse the box (we'd use N yield boxes), but
        // for the sake of efficiency, we use the same box but reset it for
        // each iteration.
        boxStore(box, NULL);

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
                memcpy(arr, stackArr, at * sizeof(zvalue));
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

/** Initializes the module. */
MOD_INIT(Generator) {
    MOD_USE(Box);
    MOD_USE_NEXT(List);

    GFN_collect = makeGeneric(1, 2, GFN_NONE, stringFromUtf8(-1, "collect"));
    datImmortalize(GFN_collect);

    GFN_nextValue = makeGeneric(2, 2, GFN_NONE,
        stringFromUtf8(-1, "nextValue"));
    datImmortalize(GFN_nextValue);

    FUN_Generator_stdCollect = makeBuiltin(1, 2,
        METH_NAME(Generator, stdCollect), 0,
        stringFromUtf8(-1, "Generator.stdCollect"));
    datImmortalize(FUN_Generator_stdCollect);
}

/* Documented in header. */
zvalue GFN_collect = NULL;

/* Documented in header. */
zvalue GFN_nextValue = NULL;

/* Documented in header. */
zvalue FUN_Generator_stdCollect = NULL;
