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

#include "impl.h"


//
// Class Definition: `Generator`
//

// Documented in spec.
FUNC_IMPL_1_2(Generator_stdCollect, generator, function) {
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

// Documented in spec.
FUNC_IMPL_1(Generator_stdFetch, generator) {
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

    FUN_Generator_stdCollect =
        datImmortalize(FUNC_VALUE(Generator_stdCollect));

    FUN_Generator_stdFetch = datImmortalize(FUNC_VALUE(Generator_stdFetch));
}

// Documented in header.
zvalue FUN_Generator_stdCollect = NULL;

// Documented in header.
zvalue FUN_Generator_stdFetch = NULL;
