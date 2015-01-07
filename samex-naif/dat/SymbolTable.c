// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Builtin.h"
#include "type/Cmp.h"
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
static SymbolTableInfo *getInfo(zvalue symtab) {
    return datPayload(symtab);
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
 * Returns the index where the `key` is stored in `info`, or returns `-1` if
 * not found.
 */
static zint infoFind(SymbolTableInfo *info, zvalue key) {
    zint arraySize = info->arraySize;
    zmapping *array = info->array;
    zint index = symbolIndex(key) % arraySize;

    for (int i = 0; i < DAT_SYMTAB_MAX_PROBES; i++) {
        zvalue foundKey = array[index].key;
        if (foundKey == NULL) {
            // As keys are never deleted, `NULL` means that we can't possibly
            // find the key at a later index.
            return -1;
        } else if (key == foundKey) {
            return index;
        }

        index = (index + 1) % arraySize;
    }

    return -1;
}

/**
 * Mutates an instance, putting a mapping into it, possibly reallocating it
 * (hence the pointer arguments). As a minor convenience, this function
 * returns early (doing nothing) if given `NULL` for `elem.key`.
 */
static void putInto(zvalue *result, SymbolTableInfo **info, zmapping elem) {
    if (elem.key == NULL) {
        return;
    }

    zint arraySize = (*info)->arraySize;
    zmapping *array = (*info)->array;
    zint index = symbolIndex(elem.key) % arraySize;

    for (int i = 0; i < DAT_SYMTAB_MAX_PROBES; i++) {
        zvalue foundKey = array[index].key;
        if (foundKey == NULL) {
            array[index] = elem;
            (*info)->size++;
            return;
        } else if (foundKey == elem.key) {
            // Update a pre-existing mapping for the key.
            array[index].value = elem.value;
            return;
        }

        index = (index + 1) % arraySize;
    }

    // Too many collisions! Reallocate, and then add the originally-requested
    // pair.

    zvalue newResult = allocInstance(arraySize);  // This grows `array`.
    SymbolTableInfo *newInfo = getInfo(newResult);

    for (int i = 0; i < arraySize; i++) {
        putInto(&newResult, &newInfo, array[i]);
    }

    putInto(&newResult, &newInfo, elem);

    *result = newResult;
    *info = newInfo;
}

/**
 * Compare two mappings. This is used as the function passed to `qsort`.
 * Note that `NULL` is made to sort *after* non-`NULL`, so that all keys
 * end up at the start of a sorted result.
 */
static int compareMappings(const void *m1, const void *m2) {
    zvalue key1 = ((zmapping *) m1)->key;
    zvalue key2 = ((zmapping *) m2)->key;

    if (key1 == key2) {
        return 0;
    } else if (key1 == NULL) {
        return 1;
    } else if (key2 == NULL) {
        return -1;
    } else {
        return cm_order(key1, key2);
    }
}


//
// Module Definitions
//

zvalue symtabGetUnchecked(zvalue symtab, zvalue key) {
    SymbolTableInfo *info = getInfo(symtab);
    zint index = infoFind(info, key);

    return (index < 0) ? NULL : info->array[index].value;
}


//
// Exported Definitions
//

// Documented in header.
void arrayFromSymtab(zmapping *result, zvalue symtab) {
    assertHasClass(symtab, CLS_SymbolTable);
    SymbolTableInfo *info = getInfo(symtab);
    zint arraySize = info->arraySize;
    zmapping *array = info->array;

    for (zint i = 0, at = 0; i < arraySize; i++) {
        zvalue key = array[i].key;
        if (key != NULL) {
            result[at] = (zmapping) {key, array[i].value};
            at++;
        }
    }
}

// Documented in header.
zvalue symtabCatMapping(zvalue symtab, zmapping mapping) {
    return symtabCatZassoc(symtab, (zassoc) {1, &mapping});
}

// Documented in header.
zvalue symtabCatZassoc(zvalue symtab, zassoc ass) {
    assertHasClass(symtab, CLS_SymbolTable);

    if (ass.size == 0) {
        return symtab;
    }

    zvalue result = allocClone(symtab);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < ass.size; i++) {
        putInto(&result, &info, ass.elems[i]);
    }
    return result;
}

// Documented in header.
zvalue symtabFromMapping(zmapping mapping) {
    return symtabFromZassoc((zassoc) {1, &mapping});
}

// Documented in header.
zvalue symtabFromZarray(zarray arr) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        for (zint i = 0; i < arr.size; i++) {
            assertValid(arr.elems[i]);
        }
    }

    if (arr.size == 0) {
        return EMPTY_SYMBOL_TABLE;
    } else if ((arr.size & 1) != 0) {
        die("Odd argument count for symbol table construction.");
    }

    zvalue result = allocInstance(arr.size >> 1);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < arr.size; i += 2) {
        putInto(&result, &info, (zmapping) {arr.elems[i], arr.elems[i + 1]});
    }

    return result;
}

// Documented in header.
zvalue symtabFromZassoc(zassoc ass) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        for (zint i = 0; i < ass.size; i++) {
            assertValid(ass.elems[i].key);
            assertValid(ass.elems[i].value);
        }
    }

    if (ass.size == 0) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue result = allocInstance(ass.size);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < ass.size; i++) {
        putInto(&result, &info, ass.elems[i]);
    }

    return result;
}

// Documented in header.
zvalue symtabGet(zvalue symtab, zvalue key) {
    assertHasClass(symtab, CLS_SymbolTable);
    return symtabGetUnchecked(symtab, key);
}

// Documented in header.
zint symtabSize(zvalue symtab) {
    assertHasClass(symtab, CLS_SymbolTable);
    return getInfo(symtab)->size;
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_rest(SymbolTable, new, args) {
    return symtabFromZarray(args);
}

// Documented in spec.
CMETH_IMPL_rest_1(SymbolTable, singleValue, keys, value) {
    if (keys.size == 0) {
        return EMPTY_SYMBOL_TABLE;
    }

    zvalue result = allocInstance(keys.size);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < keys.size; i++) {
        putInto(&result, &info, (zmapping) {keys.elems[i], value});
    }

    return result;
}

// Documented in spec.
METH_IMPL_rest(SymbolTable, cat, args) {
    if (args.size == 0) {
        return ths;
    }

    zvalue result = allocClone(ths);
    SymbolTableInfo *info = getInfo(result);

    for (zint i = 0; i < args.size; i++) {
        // Note: `typeCast` guarantees that a non-null result is of the
        // indicated class.
        zvalue one = typeCast(CLS_SymbolTable, args.elems[i]);

        if (one == NULL) {
            die("Invalid argument to `cat()`: %s", cm_debugString(one));
        }

        SymbolTableInfo *oneInfo = getInfo(one);
        zint arraySize = oneInfo->arraySize;
        zmapping *array = oneInfo->array;

        for (zint j = 0; j < arraySize; j++) {
            putInto(&result, &info, array[j]);
        }
    }

    return result;
}

// Documented in spec.
METH_IMPL_1(SymbolTable, crossEq, other) {
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
        zvalue value2 = cm_get(other, key1);

        if (!cmpEqNullOk(value1, value2)) {
            return NULL;
        }
    }

    return ths;
}

// Documented in spec.
METH_IMPL_1(SymbolTable, crossOrder, other) {
    if (ths == other) {
        return SYM(same);
    }

    // Note: `other` not guaranteed to be a `SymbolTable`.
    assertHasClass(other, CLS_SymbolTable);
    SymbolTableInfo *info1 = getInfo(ths);
    SymbolTableInfo *info2 = getInfo(other);
    zint size = info1->size;

    // Major order: Size.

    if (size < info2->size) {
        return SYM(less);
    } else if (size > info2->size) {
        return SYM(more);
    }

    // Next order: sorted key lists. In this case, we take both arrays of
    // mappings and sort them. If the key lists are equal, we'll then
    // reuse the array for comparing values.

    zmapping array1[info1->arraySize];
    zmapping array2[info2->arraySize];

    utilCpy(zmapping, array1, info1->array, info1->arraySize);
    utilCpy(zmapping, array2, info2->array, info2->arraySize);
    qsort(array1, info1->arraySize, sizeof(zmapping), compareMappings);
    qsort(array2, info2->arraySize, sizeof(zmapping), compareMappings);

    for (zint i = 0; i < size; i++) {
        zvalue key1 = array1[i].key;
        zvalue key2 = array2[i].key;
        if (key1 != key2) {
            return METH_CALL(key1, crossOrder, key2);
        }
    }

    // Last order: corresponding values.

    for (zint i = 0; i < size; i++) {
        zvalue value1 = array1[i].value;
        zvalue value2 = array2[i].value;
        zorder order = cm_order(value1, value2);
        if (order != ZSAME) {
            return symbolFromZorder(order);
        }
    }

    // They're equal!

    return SYM(same);
}

// Documented in spec.
METH_IMPL_rest(SymbolTable, del, keys) {
    SymbolTableInfo *info = getInfo(ths);
    zint arraySize = info->arraySize;
    zmapping array[arraySize];
    bool any = false;

    if ((keys.size == 0) || (info->size == 0)) {
        // Easy outs: Not actually deleting anything, and/or starting out
        // with the empty symbol table.
        return ths;
    }

    // Make a local copy of the original mappings.
    utilCpy(zmapping, array, info->array, arraySize);

    // Null out the `key` for any of the given `keys`.
    for (zint i = 0; i < keys.size; i++) {
        zint index = infoFind(info, keys.elems[i]);
        if (index >= 0) {
            any = true;
            array[index].key = NULL;
        }
    }

    if (! any) {
        // None of `keys` were in `ths`.
        return ths;
    }

    // Compact away the holes.
    zint at = 0;
    for (zint i = 0; i < arraySize; i++) {
        if (array[i].key != NULL) {
            if (i != at) {
                array[at] = array[i];
            }
            at++;
        }
    }

    // Construct a new instance with the remaining elements. This call also
    // will also return `EMPTY_SYMBOL_TABLE` when appropriate.
    return symtabFromZassoc((zassoc) {at, array});
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

// Documented in spec.
METH_IMPL_1(SymbolTable, get, key) {
    return symtabGetUnchecked(ths, key);
}

// Documented in spec.
METH_IMPL_0(SymbolTable, get_size) {
    return intFromZint(getInfo(ths)->size);
}

// Documented in header.
void bindMethodsForSymbolTable(void) {
    classBindMethods(CLS_SymbolTable,
        METH_TABLE(
            CMETH_BIND(SymbolTable, new),
            CMETH_BIND(SymbolTable, singleValue)),
        METH_TABLE(
            METH_BIND(SymbolTable, cat),
            METH_BIND(SymbolTable, crossEq),
            METH_BIND(SymbolTable, crossOrder),
            METH_BIND(SymbolTable, del),
            METH_BIND(SymbolTable, gcMark),
            METH_BIND(SymbolTable, get),
            METH_BIND(SymbolTable, get_size)));

    EMPTY_SYMBOL_TABLE = datImmortalize(allocInstance(0));
}

/** Initializes the module. */
MOD_INIT(SymbolTable) {
    MOD_USE(Symbol);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForSymbolTable()`.
}

// Documented in header.
zvalue CLS_SymbolTable = NULL;

// Documented in header.
zvalue EMPTY_SYMBOL_TABLE = NULL;
