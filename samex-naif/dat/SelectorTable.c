// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>

#include "type/Builtin.h"
#include "type/SelectorTable.h"
#include "type/define.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * SelectorTable structure.
 */
typedef struct {
    /**
     * Bindings from selectors to values, keyed off of selector index number.
     */
    zvalue table[DAT_MAX_SELECTORS];
} SelectorTableInfo;

/**
 * Gets a pointer to the value's info.
 */
static SelectorTableInfo *getInfo(zvalue selectorTable) {
    return datPayload(selectorTable);
}

/**
 * Allocates an instance.
 */
static zvalue allocInstance(void) {
    return datAllocValue(CLS_SelectorTable, 0);
}


//
// Exported Definitions
//

// Documented in header.
void arrayFromSelectorTable(zvalue *result, zvalue selectorTable) {
    assertHasClass(selectorTable, CLS_SelectorTable);
    SelectorTableInfo *info = getInfo(selectorTable);

    utilCpy(zvalue, result, info->table, DAT_MAX_SELECTORS);
}

// Documented in header.
zvalue selectorTableFromArgs(zvalue first, ...) {
    if (first == NULL) {
        return EMPTY_SELECTOR_TABLE;
    }

    zvalue result = allocInstance();
    SelectorTableInfo *info = getInfo(result);
    bool any = false;
    va_list rest;

    va_start(rest, first);
    for (;;) {
        zvalue sel = any ? va_arg(rest, zvalue) : first;

        if (sel == NULL) {
            break;
        }

        zvalue value = va_arg(rest, zvalue);
        if (value == NULL) {
            die("Odd argument count for selector table construction.");
        }

        info->table[selectorIndex(sel)] = value;
        any = true;
    }
    va_end(rest);

    return result;
}

// Documented in header.
zvalue selectorTableFromArray(zint size, zmapping *mappings) {
    if (size == 0) {
        return EMPTY_SELECTOR_TABLE;
    }

    zvalue result = allocInstance();
    SelectorTableInfo *info = getInfo(result);

    for (zint i = 0; i < size; i++) {
        zvalue key = mappings[i].key;
        zvalue value = mappings[i].value;
        info->table[selectorIndex(key)] = value;
    }

    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(SelectorTable, get) {
    zvalue table = args[0];
    zvalue sel = args[1];
    zint index = selectorIndex(sel);

    return getInfo(table)->table[index];
}

/** Function (not method) `makeSelectorTable`. Documented in spec. */
METH_IMPL(SelectorTable, makeSelectorTable) {
    if ((argCount & 1) != 0) {
        die("Odd argument count for selector table construction.");
    }

    zint size = argCount >> 1;
    zmapping mappings[size];

    for (zint i = 0, at = 0; i < size; i++, at += 2) {
        mappings[i].key = args[at];
        mappings[i].value = args[at + 1];
    }

    return selectorTableFromArray(size, mappings);
}

// Documented in header.
METH_IMPL(SelectorTable, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `SelectorTable`.

    assertHasClass(other, CLS_SelectorTable);
    SelectorTableInfo *info1 = getInfo(value);
    SelectorTableInfo *info2 = getInfo(other);

    for (zint i = 0; i < DAT_MAX_SELECTORS; i++) {
        zvalue value1 = info1->table[i];
        zvalue value2 = info2->table[i];
        if (!valEqNullOk(value1, value2)) {
            return NULL;
        }
    }

    return value;
}

/** Initializes the module. */
MOD_INIT(SelectorTable) {
    MOD_USE(Selector);
    MOD_USE(OneOff);

    CLS_SelectorTable = makeCoreClass("SelectorTable", CLS_Data,
        NULL,
        NULL);

    METH_BIND(SelectorTable, get);
    METH_BIND(SelectorTable, totalEq);

    FUN_SelectorTable_makeSelectorTable = makeBuiltin(0, -1,
        METH_NAME(SelectorTable, makeSelectorTable), 0,
        stringFromUtf8(-1, "SelectorTable.makeSelectorTable"));
    datImmortalize(FUN_SelectorTable_makeSelectorTable);

    EMPTY_SELECTOR_TABLE = allocInstance();
    datImmortalize(EMPTY_SELECTOR_TABLE);
}

// Documented in header.
zvalue CLS_SelectorTable = NULL;

// Documented in header.
zvalue EMPTY_SELECTOR_TABLE = NULL;

// Documented in header.
zvalue FUN_SelectorTable_makeSelectorTable;
