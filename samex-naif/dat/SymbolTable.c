// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>

#include "type/Builtin.h"
#include "type/SymbolTable.h"
#include "type/define.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * SymbolTable structure.
 */
typedef struct {
    /**
     * Bindings from symbols to values, keyed off of symbol index number.
     */
    zvalue table[DAT_MAX_SYMBOLS];
} SymbolTableInfo;

/**
 * Gets a pointer to the value's info.
 */
static SymbolTableInfo *getInfo(zvalue symbolTable) {
    return datPayload(symbolTable);
}

/**
 * Allocates an instance.
 */
static zvalue allocInstance(void) {
    return datAllocValue(CLS_SymbolTable, sizeof(SymbolTableInfo));
}


//
// Exported Definitions
//

// Documented in header.
void arrayFromSymbolTable(zvalue *result, zvalue symbolTable) {
    assertHasClass(symbolTable, CLS_SymbolTable);
    SymbolTableInfo *info = getInfo(symbolTable);

    utilCpy(zvalue, result, info->table, DAT_MAX_SYMBOLS);
}

// Documented in header.
zvalue symbolTableFromArgs(zvalue first, ...) {
    if (first == NULL) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue result = allocInstance();
    SymbolTableInfo *info = getInfo(result);
    bool any = false;
    va_list rest;

    va_start(rest, first);
    for (;;) {
        zvalue symbol = any ? va_arg(rest, zvalue) : first;

        if (symbol == NULL) {
            break;
        }

        zvalue value = va_arg(rest, zvalue);
        if (value == NULL) {
            die("Odd argument count for symbol table construction.");
        }

        info->table[symbolIndex(symbol)] = value;
        any = true;
    }
    va_end(rest);

    return result;
}

// Documented in header.
zvalue symbolTableFromArray(zint size, zmapping *mappings) {
    if (size == 0) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue result = allocInstance();
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < size; i++) {
        zvalue key = mappings[i].key;
        zvalue value = mappings[i].value;
        info->table[symbolIndex(key)] = value;
    }

    return result;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(SymbolTable, gcMark) {
    zvalue table = args[0];
    SymbolTableInfo *info = getInfo(table);

    for (zint i = 0; i < DAT_MAX_SYMBOLS; i++) {
        datMark(info->table[i]);
    }

    return NULL;
}

// Documented in header.
METH_IMPL(SymbolTable, get) {
    zvalue table = args[0];
    zvalue symbol = args[1];
    zint index = symbolIndex(symbol);

    return getInfo(table)->table[index];
}

/** Function (not method) `makeSymbolTable`. Documented in spec. */
METH_IMPL(SymbolTable, makeSymbolTable) {
    if ((argCount & 1) != 0) {
        die("Odd argument count for symbol table construction.");
    }

    zint size = argCount >> 1;
    zmapping mappings[size];

    for (zint i = 0, at = 0; i < size; i++, at += 2) {
        mappings[i].key = args[at];
        mappings[i].value = args[at + 1];
    }

    return symbolTableFromArray(size, mappings);
}

// Documented in header.
METH_IMPL(SymbolTable, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `SymbolTable`.

    assertHasClass(other, CLS_SymbolTable);
    SymbolTableInfo *info1 = getInfo(value);
    SymbolTableInfo *info2 = getInfo(other);

    for (zint i = 0; i < DAT_MAX_SYMBOLS; i++) {
        zvalue value1 = info1->table[i];
        zvalue value2 = info2->table[i];
        if (!valEqNullOk(value1, value2)) {
            return NULL;
        }
    }

    return value;
}

/** Initializes the module. */
MOD_INIT(SymbolTable) {
    MOD_USE(Symbol);
    MOD_USE(OneOff);

    // Note: The `objectModel` module initializes `CLS_SymbolTable`.
    classBindMethods(CLS_SymbolTable,
        NULL,
        symbolTableFromArgs(
            SYM_METH(SymbolTable, gcMark),
            SYM_METH(SymbolTable, get),
            SYM_METH(SymbolTable, totalEq),
            NULL));

    FUN_SymbolTable_makeSymbolTable = makeBuiltin(0, -1,
        METH_NAME(SymbolTable, makeSymbolTable), 0,
        stringFromUtf8(-1, "SymbolTable.makeSymbolTable"));
    datImmortalize(FUN_SymbolTable_makeSymbolTable);

    EMPTY_SYMBOL_TABLE = allocInstance();
    datImmortalize(EMPTY_SYMBOL_TABLE);
}

// Documented in header.
zvalue CLS_SymbolTable = NULL;

// Documented in header.
zvalue EMPTY_SYMBOL_TABLE = NULL;

// Documented in header.
zvalue FUN_SymbolTable_makeSymbolTable;