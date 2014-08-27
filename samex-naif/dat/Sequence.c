// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Sequence values
//

#include "const.h"
#include "type/Builtin.h"
#include "type/DerivedData.h"
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

// Documented in header.
METH_IMPL(Sequence, get) {
    zvalue seq = args[0];
    zvalue key = args[1];

    if (seqNthIndexLenient(key) >= 0) {
        return METH_CALL(nth, seq, key);
    } else {
        return NULL;
    }
}

// Documented in header.
METH_IMPL(Sequence, keyList) {
    zvalue seq = args[0];

    zint size = get_size(seq);
    zvalue elems[size];

    for (zint i = 0; i < size; i++) {
        elems[i] = intFromZint(i);
    }

    return listFromArray(size, elems);
}

// Documented in header.
METH_IMPL(Sequence, nthMapping) {
    zvalue seq = args[0];
    zvalue n = args[1];
    zvalue value = METH_CALL(nth, seq, n);

    if (value == NULL) {
        return NULL;
    } else {
        zmapping mapping = {n, value};
        return mapFromArray(1, &mapping);
    }
}

/** Initializes the module. */
MOD_INIT(Sequence) {
    MOD_USE(Collection);
    MOD_USE_NEXT(Generator);

    SYM_INIT(reverse);
    SYM_INIT(sliceExclusive);
    SYM_INIT(sliceInclusive);

    FUN_Sequence_get = makeBuiltin(2, 2,
        METH_NAME(Sequence, get), 0,
        stringFromUtf8(-1, "Sequence.get"));
    datImmortalize(FUN_Sequence_get);

    FUN_Sequence_keyList = makeBuiltin(1, 1,
        METH_NAME(Sequence, keyList), 0,
        stringFromUtf8(-1, "Sequence.keyList"));
    datImmortalize(FUN_Sequence_keyList);

    FUN_Sequence_nthMapping = makeBuiltin(1, 1,
        METH_NAME(Sequence, nthMapping),
        0, stringFromUtf8(-1, "Sequence.nthMapping"));
    datImmortalize(FUN_Sequence_nthMapping);
}

// Documented in header.
SYM_DEF(reverse);

// Documented in header.
SYM_DEF(sliceExclusive);

// Documented in header.
SYM_DEF(sliceInclusive);

// Documented in header.
zvalue FUN_Sequence_get = NULL;

// Documented in header.
zvalue FUN_Sequence_keyList = NULL;

// Documented in header.
zvalue FUN_Sequence_nthMapping = NULL;
