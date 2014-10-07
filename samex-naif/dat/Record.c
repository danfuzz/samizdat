// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

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
zvalue makeRecord(zvalue name, zvalue data) {
    zint index = symbolIndex(name);  // Do this early, to catch non-symbols.

    if (data == NULL) {
        data = EMPTY_SYMBOL_TABLE;
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

// Documented in header.
FUNC_IMPL_1_2(Record_makeRecord, cls, value) {
    return makeRecord(cls, value);
}

// Documented in header.
METH_IMPL_0(Record, dataOf) {
    return getInfo(ths)->data;
}

// Documented in header.
METH_IMPL_0(Record, debugString) {
    RecordInfo *info = getInfo(ths);

    if (valEq(info->data, EMPTY_SYMBOL_TABLE)) {
        return METH_CALL(cat,
            METH_CALL(debugString, info->name),
            stringFromUtf8(-1, "{}"));
    } else {
        return METH_CALL(cat,
            METH_CALL(debugString, info->name),
            stringFromUtf8(-1, "{...}"));
    }
}

// Documented in header.
METH_IMPL_0(Record, gcMark) {
    RecordInfo *info = getInfo(ths);

    datMark(info->name);
    datMark(info->data);
    return NULL;
}

// Documented in header.
METH_IMPL_1(Record, get, key) {
    return get(getInfo(ths)->data, key);
}

// Documented in header.
METH_IMPL_0(Record, get_name) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_1(Record, hasName, name) {
    return symbolEq(getInfo(ths)->name, name) ? ths : NULL;
}

// Documented in header.
METH_IMPL_1(Record, totalEq, other) {
    assertHasClass(other, CLS_Record);  // Note: Might not be a `Record`.

    RecordInfo *info1 = getInfo(ths);
    RecordInfo *info2 = getInfo(other);

    if (info1->nameIndex != info2->nameIndex) {
        return NULL;
    } else {
        return valEq(info1->data, info2->data);
    }
}

// Documented in header.
METH_IMPL_1(Record, totalOrder, other) {
    assertHasClass(other, CLS_Record);  // Note: Might not be a `Record`.

    RecordInfo *info1 = getInfo(ths);
    RecordInfo *info2 = getInfo(other);

    if (info1->nameIndex != info2->nameIndex) {
        return valOrder(info1->name, info2->name);
    }

    return valOrder(info1->data, info2->data);
}

/** Initializes the module. */
MOD_INIT(Record) {
    MOD_USE(Core);

    CLS_Record = makeCoreClass(SYM(Record), CLS_Core,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Record, dataOf),
            METH_BIND(Record, debugString),
            METH_BIND(Record, gcMark),
            METH_BIND(Record, get),
            METH_BIND(Record, get_name),
            METH_BIND(Record, hasName),
            METH_BIND(Record, totalEq),
            METH_BIND(Record, totalOrder),
            NULL));

    FUN_Record_makeRecord = datImmortalize(FUNC_VALUE(Record_makeRecord));
}

// Documented in header.
zvalue CLS_Record = NULL;

// Documented in header.
zvalue FUN_Record_makeRecord = NULL;
