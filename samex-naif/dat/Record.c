// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Cmp.h"
#include "type/Int.h"
#include "type/Record.h"
#include "type/SymbolTable.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Payload data for all records.
 */
typedef struct {
    /** Record name. */
    zvalue name;

    /** Name's symbol index. */
    zint nameIndex;

    /** Data payload. */
    zvalue data;
} RecordInfo;

/**
 * Gets the info of a record.
 */
static RecordInfo *getInfo(zvalue value) {
    return (RecordInfo *) datPayload(value);
}


//
// Exported Definitions
//

// Documented in header.
zvalue recFromZarray(zvalue name, zarray arr) {
    return cm_new(Record, name, symtabFromZarray(arr));
}

// Documented in header.
bool recGet1(zvalue record, zvalue key, zvalue *got) {
    assertHasClass(record, CLS_Record);
    zvalue data = getInfo(record)->data;

    *got = symtabGetUnchecked(data, key);

    return (*got != NULL);
};

// Documented in header.
bool recGet2(zvalue record,
        zvalue key1, zvalue *got1,
        zvalue key2, zvalue *got2) {
    assertHasClass(record, CLS_Record);
    zvalue data = getInfo(record)->data;

    *got1 = symtabGetUnchecked(data, key1);
    *got2 = symtabGetUnchecked(data, key2);

    return (*got1 != NULL) && (*got2 != NULL);
};

// Documented in header.
bool recGet3(zvalue record,
        zvalue key1, zvalue *got1,
        zvalue key2, zvalue *got2,
        zvalue key3, zvalue *got3) {
    assertHasClass(record, CLS_Record);
    zvalue data = getInfo(record)->data;

    *got1 = symtabGetUnchecked(data, key1);
    *got2 = symtabGetUnchecked(data, key2);
    *got3 = symtabGetUnchecked(data, key3);

    return (*got1 != NULL) && (*got2 != NULL) && (*got3 != NULL);
};

// Documented in header.
bool recGet4(zvalue record,
        zvalue key1, zvalue *got1,
        zvalue key2, zvalue *got2,
        zvalue key3, zvalue *got3,
        zvalue key4, zvalue *got4) {
    assertHasClass(record, CLS_Record);
    zvalue data = getInfo(record)->data;

    *got1 = symtabGetUnchecked(data, key1);
    *got2 = symtabGetUnchecked(data, key2);
    *got3 = symtabGetUnchecked(data, key3);
    *got4 = symtabGetUnchecked(data, key4);

    return (*got1 != NULL) && (*got2 != NULL) && (*got3 != NULL)
        && (*got4 != NULL);
};

// Documented in header.
bool recHasName(zvalue record, zvalue name) {
    assertHasClass(record, CLS_Record);
    return symbolEq(getInfo(record)->name, name);
}

// Documented in header.
zint recNameIndex(zvalue record) {
    assertHasClass(record, CLS_Record);
    return getInfo(record)->nameIndex;
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_1_opt(Record, new, name, data) {
    zint index = symbolIndex(name);  // Do this early, to catch non-symbols.

    if (data == NULL) {
        data = EMPTY_SYMBOL_TABLE;
    } else if (typeAccepts(CLS_Record, data)) {
        // Extract the data out of the given record.
        data = getInfo(data)->data;
    } else {
        assertHasClass(data, CLS_SymbolTable);
    }

    zvalue result = datAllocValue(CLS_Record, sizeof(RecordInfo));
    RecordInfo *info = getInfo(result);

    info->name = name;
    info->nameIndex = index;
    info->data = data;

    return result;
}

// Documented in spec.
METH_IMPL_1(Record, castToward, cls) {
    if (cmpEq(cls, CLS_SymbolTable)) {
        return getInfo(ths)->data;
    } else if (typeAccepts(cls, ths)) {
        return ths;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_rest(Record, cat, args) {
    if (args.size == 0) {
        return ths;
    }

    // What this does is ask `SymbolTable` to concat `ths`'s data with
    // all the given arguments, and then use that result to construct a
    // new instance with the same `name` as `ths`.

    RecordInfo *info = getInfo(ths);
    zvalue data = methCall(info->data, SYM(cat), args);

    return cm_new(Record, info->name, data);
}

// Documented in spec.
METH_IMPL_1(Record, crossEq, other) {
    assertHasClass(other, CLS_Record);  // Note: Might not be a `Record`.

    RecordInfo *info1 = getInfo(ths);
    RecordInfo *info2 = getInfo(other);

    if (info1->nameIndex != info2->nameIndex) {
        return NULL;
    } else {
        return cmpEq(info1->data, info2->data);
    }
}

// Documented in spec.
METH_IMPL_1(Record, crossOrder, other) {
    assertHasClass(other, CLS_Record);  // Note: Might not be a `Record`.

    RecordInfo *info1 = getInfo(ths);
    RecordInfo *info2 = getInfo(other);

    if (info1->nameIndex != info2->nameIndex) {
        return cmpOrder(info1->name, info2->name);
    }

    return cmpOrder(info1->data, info2->data);
}

// Documented in spec.
METH_IMPL_0(Record, debugString) {
    RecordInfo *info = getInfo(ths);

    if (cmpEq(info->data, EMPTY_SYMBOL_TABLE)) {
        return cm_cat(
            METH_CALL(info->name, debugString),
            stringFromUtf8(-1, "{}"));
    } else {
        return cm_cat(
            METH_CALL(info->name, debugString),
            stringFromUtf8(-1, "{...}"));
    }
}

// Documented in spec.
METH_IMPL_rest(Record, del, keys) {
    RecordInfo *info = getInfo(ths);
    zvalue data = info->data;
    zvalue newData = methCall(data, SYM(del), keys);

    return (newData == data)
        ? ths
        : cm_new(Record, info->name, newData);
}

// Documented in header.
METH_IMPL_0(Record, gcMark) {
    RecordInfo *info = getInfo(ths);

    datMark(info->name);
    datMark(info->data);
    return NULL;
}

// Documented in spec.
METH_IMPL_1(Record, get, key) {
    return symtabGetUnchecked(getInfo(ths)->data, key);
}

// Documented in spec.
METH_IMPL_0(Record, get_data) {
    return getInfo(ths)->data;
}

// Documented in spec.
METH_IMPL_0(Record, get_name) {
    return getInfo(ths)->name;
}

// Documented in spec.
METH_IMPL_1(Record, hasName, name) {
    return symbolEq(getInfo(ths)->name, name) ? ths : NULL;
}

/** Initializes the module. */
MOD_INIT(Record) {
    MOD_USE(Core);

    CLS_Record = makeCoreClass(SYM(Record), CLS_Core,
        METH_TABLE(
            CMETH_BIND(Record, new)),
        METH_TABLE(
            METH_BIND(Record, castToward),
            METH_BIND(Record, cat),
            METH_BIND(Record, crossEq),
            METH_BIND(Record, crossOrder),
            METH_BIND(Record, debugString),
            METH_BIND(Record, del),
            METH_BIND(Record, gcMark),
            METH_BIND(Record, get),
            METH_BIND(Record, get_data),
            METH_BIND(Record, get_name),
            METH_BIND(Record, hasName)));
}

// Documented in header.
zvalue CLS_Record = NULL;
