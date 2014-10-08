// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Sequence values
//

#include "type/Builtin.h"
#include "type/Generator.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Sequence.h"
#include "type/define.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
void seqConvertSliceArgs(zint *startPtr, zint *endPtr, bool inclusive,
        zint size, zvalue startArg, zvalue endArg) {
    zint start = zintFromInt(startArg);
    zint end = (endArg != NULL) ? zintFromInt(endArg) : (size - 1);

    if (inclusive) {
        end++;
    }

    if (end < start) {
        *startPtr = -1;
        *endPtr = -1;
        return;
    }

    if (start < 0) {
        start = 0;
    }

    if (end > size) {
        end = size;
    }

    if (end > start) {
        *startPtr = start;
        *endPtr = end;
    } else {
        *startPtr = 0;
        *endPtr = 0;
    }
}

// Documented in header.
zint seqNthIndexLenient(zvalue key) {
    if (hasClass(key, CLS_Int)) {
        zint index = zintFromInt(key);
        return (index >= 0) ? index : -1;
    } else {
        return -1;
    }
}

// Documented in header.
zint seqNthIndexStrict(zint size, zvalue n) {
    if (hasClass(n, CLS_Int)) {
        zint index = zintFromInt(n);
        return ((index >= 0) && (index < size)) ? index : -1;
    } else {
        die("Invalid class for `nth` (non-int).");
    }
}

// Documented in header.
zint seqPutIndexStrict(zint size, zvalue n) {
    if (hasClass(n, CLS_Int)) {
        zint index = zintFromInt(n);
        if (index < 0) {
            die("Invalid index for `put` (negative).");
        } else if (index > size) {
            die("Invalid index for `put`: %lld, size %lld", index, size);
        }
        return index;
    } else {
        die("Invalid class for `put` (non-int).");
    }
}


//
// Class(ish) Definition: `Sequence`
//
// **Note:** This isn't the usual form of class definition, since these
// methods are bound on a couple classes.
//

// Documented in spec.
METH_IMPL_1(Sequence, get, key) {
    if (seqNthIndexLenient(key) >= 0) {
        return METH_CALL(nth, ths, key);
    } else {
        return NULL;
    }
}

// Documented in spec.
METH_IMPL_0(Sequence, keyList) {
    zint size = get_size(ths);
    zvalue elems[size];

    for (zint i = 0; i < size; i++) {
        elems[i] = intFromZint(i);
    }

    return listFromArray(size, elems);
}

// Documented in spec.
METH_IMPL_1(Sequence, nthMapping, n) {
    zvalue value = METH_CALL(nth, ths, n);

    if (value == NULL) {
        return NULL;
    } else {
        zmapping mapping = {n, value};
        return mapFromArray(1, &mapping);
    }
}

/** Initializes the module. */
MOD_INIT(Sequence) {
    MOD_USE_NEXT(Generator);

    FUN_Sequence_get        = datImmortalize(FUNC_VALUE(Sequence_get));
    FUN_Sequence_keyList    = datImmortalize(FUNC_VALUE(Sequence_keyList));
    FUN_Sequence_nthMapping = datImmortalize(FUNC_VALUE(Sequence_nthMapping));
}

// Documented in header.
zvalue FUN_Sequence_get = NULL;

// Documented in header.
zvalue FUN_Sequence_keyList = NULL;

// Documented in header.
zvalue FUN_Sequence_nthMapping = NULL;
