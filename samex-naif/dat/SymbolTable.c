// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>

#include "type/Builtin.h"
#include "type/Int.h"
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
    /** Number of bindings in this table. */
    zint size;

    /** Size of the backing array. */
    zint arraySize;

    /** Bindings from symbols to values. */
    zmapping array[/*arraySize*/];
} SymbolTableInfo;

/**
 * Gets a pointer to the value's info.
 */
static SymbolTableInfo *getInfo(zvalue symbolTable) {
    return datPayload(symbolTable);
}

/**
 * Allocates an instance with the given `arraySize`.
 */
static zvalue allocWithArraySize(zint arraySize) {
    zvalue result = datAllocValue(CLS_SymbolTable,
        sizeof(SymbolTableInfo) + (arraySize * sizeof(zmapping)));
    SymbolTableInfo *info = datPayload(result);

    info->arraySize = arraySize;
    return result;
}

/**
 * Allocates an instance, for the given presumed content size.
 */
static zvalue allocInstance(zint size) {
    zint arraySize = DAT_SYMTAB_MIN_SIZE + (size * DAT_SYMTAB_SCALE_FACTOR);
    return allocWithArraySize(arraySize);
}

/**
 * Allocates an instance that is a clone of another.
 */
static zvalue allocClone(zvalue orig) {
    SymbolTableInfo *origInfo = datPayload(orig);
    zint arraySize = origInfo->arraySize;
    zvalue result = allocWithArraySize(arraySize);
    SymbolTableInfo *info = datPayload(result);

    info->size = origInfo->size;
    utilCpy(zmapping, info->array, origInfo->array, arraySize);
    return result;
}

/**
 * The main guts of the `get` operation.
 */
static zvalue infoGet(SymbolTableInfo *info, zvalue key) {
    zint arraySize = info->arraySize;
    zmapping *array = info->array;
    zint index = symbolIndex(key) % arraySize;

    for (int i = 0; i < DAT_SYMTAB_MAX_PROBES; i++) {
        zvalue foundKey = array[index].key;
        if (foundKey == NULL) {
            // As keys are never deleted, `NULL` means that we can't possibly
            // find the key at a later index.
            return NULL;
        } else if (key == foundKey) {
            return array[index].value;
        }

        index = (index + 1) % arraySize;
    }

    return NULL;
}

/**
 * Mutates an instance, putting a mapping into it, possibly reallocating it
 * (hence the pointer arguments). As a minor convenience, this function
 * returns early (doing nothing) if given `NULL` for `key`.
 */
static void putInto(zvalue *result, SymbolTableInfo **info,
        zvalue key, zvalue value) {
    if (key == NULL) {
        return;
    }

    zint arraySize = (*info)->arraySize;
    zmapping *array = (*info)->array;
    zint index = symbolIndex(key) % arraySize;

    for (int i = 0; i < DAT_SYMTAB_MAX_PROBES; i++) {
        zvalue foundKey = array[index].key;
        if (foundKey == NULL) {
            array[index].key = key;
            array[index].value = value;
            (*info)->size++;
            return;
        } else if (foundKey == key) {
            // Update a pre-existing mapping for the key.
            array[index].value = value;
            return;
        }

        index = (index + 1) % arraySize;
    }

    // Too many collisions! Reallocate, and then add the originally-requested
    // pair.

    zvalue newResult = allocInstance(arraySize);  // This grows `array`.
    SymbolTableInfo *newInfo = getInfo(newResult);

    for (int i = 0; i < arraySize; i++) {
        putInto(&newResult, &newInfo, array[i].key, array[i].value);
    }

    putInto(&newResult, &newInfo, key, value);

    *result = newResult;
    *info = newInfo;
}


//
// Exported Definitions
//

// Documented in header.
void arrayFromSymbolTable(zmapping *result, zvalue symbolTable) {
    assertHasClass(symbolTable, CLS_SymbolTable);
    SymbolTableInfo *info = getInfo(symbolTable);
    zint arraySize = info->arraySize;
    zmapping *array = info->array;

    for (zint i = 0, at = 0; i < arraySize; i++) {
        zvalue key = array[i].key;
        if (key != NULL) {
            result[at].key = key;
            result[at].value = array[i].value;
            at++;
        }
    }
}

// Documented in header.
zvalue symbolTableFromArgs(zvalue first, ...) {
    if (first == NULL) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue result = allocInstance(0);
    SymbolTableInfo *info = getInfo(result);
    va_list rest;

    va_start(rest, first);
    for (;;) {
        zvalue symbol = (info->size == 0) ? first : va_arg(rest, zvalue);

        if (symbol == NULL) {
            break;
        }

        zvalue value = va_arg(rest, zvalue);
        if (value == NULL) {
            die("Odd argument count for symbol table construction.");
        }

        putInto(&result, &info, symbol, value);
    }
    va_end(rest);

    return result;
}

// Documented in header.
zvalue symbolTableFromArray(zint size, zmapping *mappings) {
    if (size == 0) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue result = allocInstance(size);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < size; i++) {
        putInto(&result, &info, mappings[i].key, mappings[i].value);
    }

    return result;
}

// Documented in header.
zint symbolTableSize(zvalue symbolTable) {
    assertHasClass(symbolTable, CLS_SymbolTable);
    return getInfo(symbolTable)->size;
}


//
// Class Definition
//

// Documented in header.
FUNC_IMPL_rest(SymbolTable_makeSymbolTable, args) {
    if ((argsSize & 1) != 0) {
        die("Odd argument count for symbol table construction.");
    }

    if (argsSize == 0) {
        return EMPTY_SYMBOL_TABLE;
    }

    zint size = argsSize >> 1;
    zvalue result = allocInstance(size);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0, at = 0; i < size; i++, at += 2) {
        putInto(&result, &info, args[at], args[at + 1]);
    }

    return result;
}

// Documented in header.
FUNC_IMPL_1_rest(SymbolTable_makeValueSymbolTable, first, args) {
    // Since the arguments are "stretchy" in the front instead of the
    // usual rear, we do a bit of non-obvious rearranging here.

    if (argsSize == 0) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue value = args[argsSize - 1];
    zvalue result = allocInstance(argsSize);
    SymbolTableInfo *info = getInfo(result);

    putInto(&result, &info, first, value);

    argsSize--;
    for (zint i = 0; i < argsSize; i++) {
        putInto(&result, &info, args[i], value);
    }

    return result;
}

// Documented in header.
METH_IMPL_rest(SymbolTable, cat, args) {
    if (argsSize == 0) {
        return ths;
    }

    zvalue result = allocClone(ths);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < argsSize; i++) {
        zvalue one = args[i];
        assertHasClass(one, CLS_SymbolTable);
        SymbolTableInfo *oneInfo = getInfo(one);
        zint arraySize = oneInfo->arraySize;
        zmapping *array = oneInfo->array;

        for (zint j = 0; j < arraySize; j++) {
            putInto(&result, &info, array[j].key, array[j].value);
        }
    }

    return result;
}

// Documented in header.
METH_IMPL_0(SymbolTable, gcMark) {
    SymbolTableInfo *info = getInfo(ths);
    zint arraySize = info->arraySize;

    for (zint i = 0; i < arraySize; i++) {
        datMark(info->array[i].key);
        datMark(info->array[i].value);
    }

    return NULL;
}

// Documented in header.
METH_IMPL_1(SymbolTable, get, key) {
    return infoGet(getInfo(ths), key);
}

// Documented in header.
METH_IMPL_0(SymbolTable, get_size) {
    return intFromZint(getInfo(ths)->size);
}

// Documented in header.
METH_IMPL_2(SymbolTable, put, key, value) {
    zvalue result = allocClone(ths);
    SymbolTableInfo *info = getInfo(result);

    putInto(&result, &info, key, value);
    return result;
}

// Documented in header.
METH_IMPL_1(SymbolTable, totalEq, other) {
    if (ths == other) {
        return ths;
    }

    // Note: `other` not guaranteed to be a `SymbolTable`.
    assertHasClass(other, CLS_SymbolTable);
    SymbolTableInfo *info = getInfo(ths);

    if (info->size != getInfo(other)->size) {
        return NULL;
    }

    // Go through each key in `ths`, looking it up in `other`. The two are
    // only equal if every key is found and bound to an equal value.

    zint arraySize = info->arraySize;
    for (zint i = 0; i < arraySize; i++) {
        zvalue key1 = info->array[i].key;

        if (key1 == NULL) {
            continue;
        }

        zvalue value1 = info->array[i].value;
        zvalue value2 = get(other, key1);

        if (!valEqNullOk(value1, value2)) {
            return NULL;
        }
    }

    return ths;
}

/** Initializes the module. */
MOD_INIT(SymbolTable) {
    MOD_USE(Symbol);
    MOD_USE(OneOff);

    // Note: The `objectModel` module initializes `CLS_SymbolTable`.
    classBindMethods(CLS_SymbolTable,
        NULL,
        symbolTableFromArgs(
            METH_BIND(SymbolTable, cat),
            METH_BIND(SymbolTable, gcMark),
            METH_BIND(SymbolTable, get),
            METH_BIND(SymbolTable, get_size),
            METH_BIND(SymbolTable, put),
            METH_BIND(SymbolTable, totalEq),
            NULL));

    FUN_SymbolTable_makeSymbolTable =
        datImmortalize(FUNC_VALUE(SymbolTable_makeSymbolTable));

    FUN_SymbolTable_makeValueSymbolTable =
        datImmortalize(FUNC_VALUE(SymbolTable_makeValueSymbolTable));

    EMPTY_SYMBOL_TABLE = datImmortalize(allocInstance(0));
}

// Documented in header.
zvalue CLS_SymbolTable = NULL;

// Documented in header.
zvalue EMPTY_SYMBOL_TABLE = NULL;

// Documented in header.
zvalue FUN_SymbolTable_makeSymbolTable;

// Documented in header.
zvalue FUN_SymbolTable_makeValueSymbolTable;
