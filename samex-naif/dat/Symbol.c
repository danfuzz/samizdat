// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Int.h"
#include "type/Symbol.h"
#include "type/String.h"
#include "type/define.h"

#include "impl.h"


//
// Private Definitions
//

/** Array of all symbols, in index order. */
static zvalue theSymbols[DAT_MAX_SYMBOLS];

/** Next symbol index to assign. */
static zint theNextIndex = 0;

/** Array of all interned symbols, in sort order (possibly stale). */
static zvalue theInternedSymbols[DAT_MAX_SYMBOLS];

/** The number of interned symbols. */
static zint theInternedSymbolCount = 0;

/** Whether `theInternedSymbols` needs a sort. */
static bool theNeedSort = false;

/**
 * Symbol structure.
 */
typedef struct {
    /** Index of the symbol. No two symbols have the same index. */
    zint index;

    /** Whether this instance is interned. */
    bool interned;

    /**
     * Name of the symbol. `chars` points at the actual array built into
     * this object.
     */
    zstring s;

    /** Characters of the symbol's name. */
    zchar chars[DAT_MAX_SYMBOL_SIZE];
} SymbolInfo;

/**
 * Gets a pointer to the value's info.
 */
static SymbolInfo *getInfo(zvalue symbol) {
    return datPayload(symbol);
}

/**
 * Shared implementation of `eq`, given valid symbol values.
 */
static bool uncheckedEq(zvalue symbol1, zvalue symbol2) {
    // It's safe to use `==`, since by definition two different symbol
    // references must be different symbols.
    return symbol1 == symbol2;
}

/**
 * Creates and returns a new symbol with the given name. Checks that the
 * size of the name is acceptable and that there aren't already too many
 * symbols. Does no other checking.
 */
static zvalue makeSymbol0(zstring name, bool interned) {
    if (theNextIndex >= DAT_MAX_SYMBOLS) {
        die("Too many symbols!");
    } else if (name.size > DAT_MAX_SYMBOL_SIZE) {
        die("Symbol name too long: \"%s\"", utf8DupFromZstring(name));
    }

    zvalue result = datAllocValue(CLS_Symbol, sizeof(SymbolInfo));
    SymbolInfo *info = getInfo(result);

    info->index = theNextIndex;
    info->interned = interned;
    info->s.size = name.size;
    info->s.chars = info->chars;
    utilCpy(zchar, info->chars, name.chars, name.size);

    theSymbols[theNextIndex] = result;
    theNextIndex++;

    if (interned) {
        theInternedSymbols[theInternedSymbolCount] = result;
        theInternedSymbolCount++;
        theNeedSort = true;
    }

    datImmortalize(result);
    return result;
}

/**
 * Compares two symbols for index order. Used for sorting.
 */
static int indexOrder(const void *ptr1, const void *ptr2) {
    zvalue sym1 = *(zvalue *) ptr1;
    zvalue sym2 = *(zvalue *) ptr2;

    if (uncheckedEq(sym1, sym2)) {
        return 0;
    }

    zint idx1 = getInfo(sym1)->index;
    zint idx2 = getInfo(sym2)->index;

    return (idx1 < idx2) ? -1 : 1;
}

/**
 * Compares two symbols for name order. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue v1 = *(zvalue *) vptr1;
    zvalue v2 = *(zvalue *) vptr2;
    return zstringOrder(getInfo(v1)->s, getInfo(v2)->s);
}

/**
 * Compares a name with a symbol. Used for searching.
 */
static int searchOrder(const void *key, const void *vptr) {
    const zstring *name = (const zstring *) key;
    zvalue symbol = *(zvalue *) vptr;

    return zstringOrder(*name, getInfo(symbol)->s);
}

/**
 * Finds an existing interned symbol with the given name, if any.
 */
static zvalue findInternedSymbol(zstring name) {
    if (theNeedSort) {
        qsort(
            theInternedSymbols, theInternedSymbolCount,
            sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue *found = (zvalue *) bsearch(
        &name, theInternedSymbols, theInternedSymbolCount,
        sizeof(zvalue), searchOrder);

    return (found == NULL) ? NULL : *found;
}

/**
 * Helper for `symbolFromUtf8` and `unlistedSymbolFromUtf8`, which
 * does all the real work.
 */
static zvalue anySymbolFromUtf8(zint utfBytes, const char *utf,
        bool interned) {
    zchar chars[DAT_MAX_SYMBOLS];
    zstring name = {utf8DecodeStringSize(utfBytes, utf), chars};

    utf8DecodeCharsFromString(chars, utfBytes, utf);

    if (interned) {
        return symbolFromZstring(name);
    } else {
        return makeSymbol0(name, false);
    }
}


//
// Exported Definitions
//

// Documented in header.
zvalue unlistedSymbolFromUtf8(zint utfBytes, const char *utf) {
    return anySymbolFromUtf8(utfBytes, utf, false);
}

// Documented in header.
zvalue symbolCat(zvalue symbol1, zvalue symbol2) {
    assertHasClass(symbol1, CLS_Symbol);
    assertHasClass(symbol2, CLS_Symbol);

    SymbolInfo *info1 = getInfo(symbol1);
    SymbolInfo *info2 = getInfo(symbol2);

    zint size1 = info1->s.size;
    zint size = size1 + info2->s.size;
    zchar chars[size];

    arrayFromZstring(chars, info1->s);
    arrayFromZstring(&chars[size1], info2->s);

    return symbolFromZstring((zstring) {size, chars});
}

// Documented in header.
bool symbolEq(zvalue symbol1, zvalue symbol2) {
    assertHasClass(symbol1, CLS_Symbol);
    assertHasClass(symbol2, CLS_Symbol);
    return uncheckedEq(symbol1, symbol2);
}

// Documented in header.
zvalue symbolFromIndex(zint index) {
    if ((index < 0) || (index >= theNextIndex)) {
        die("Bad index for symbol: %d", index);
    }

    return theSymbols[index];
}

// Documented in header.
zvalue symbolFromString(zvalue name) {
    return symbolFromZstring(zstringFromString(name));
}

// Documented in header.
zvalue symbolFromUtf8(zint utfBytes, const char *utf) {
    return anySymbolFromUtf8(utfBytes, utf, true);
}

// Documented in header.
zvalue symbolFromZorder(zorder order) {
    switch (order) {
        case ZLESS: { return SYM(less); }
        case ZMORE: { return SYM(more); }
        case ZSAME: { return SYM(same); }
    }
}

// Documented in header.
zvalue symbolFromZstring(zstring name) {
    zvalue result = findInternedSymbol(name);
    return (result != NULL) ? result : makeSymbol0(name, true);
}

// Documented in header.
zint symbolIndex(zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    return getInfo(symbol)->index;
}

// Documented in header.
void symbolSort(zint count, zvalue *symbols) {
    for (zint i = 0; i < count; i++) {
        assertHasClass(symbols[i], CLS_Symbol);
    }

    qsort(symbols, count, sizeof(zvalue), indexOrder);
}

// Documented in header.
zorder zorderFromSymbol(zvalue symbol) {
    zint index = symbolIndex(symbol);

    // This can't be a `switch`, since the indices aren't constants.
    if      (index == SYMIDX(less)) { return ZLESS; }
    else if (index == SYMIDX(more)) { return ZMORE; }
    else if (index == SYMIDX(same)) { return ZSAME; }

    die("Invalid order symbol: %s", cm_debugString(symbol));
}

// Documented in header.
zstring zstringFromSymbol(zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    return getInfo(symbol)->s;
}


//
// Class Definition
//

// Documented in spec.
METH_IMPL_rest(Symbol, cat, args) {
    if (args.size == 0) {
        return ths;
    }

    zint thsSize = getInfo(ths)->s.size;
    zint size = thsSize;
    zstring strings[args.size];

    for (zint i = 0; i < args.size; i++) {
        zvalue one = args.elems[i];
        assertHasClass(one, CLS_Symbol);
        strings[i] = getInfo(one)->s;
        size += strings[i].size;
    }

    if (size > DAT_MAX_SYMBOL_SIZE) {
        die("Too many characters in arguments to `Symbol.cat()`.");
    }

    zchar chars[size];
    zint at = thsSize;
    arrayFromZstring(chars, getInfo(ths)->s);
    for (zint i = 0; i < args.size; i++) {
        arrayFromZstring(&chars[at], strings[i]);
        at += strings[i].size;
    }

    return symbolFromZstring((zstring) {size, chars});
}

// Documented in spec.
METH_IMPL_1(Symbol, crossEq, other) {
    assertHasClass(other, CLS_Symbol);  // Not guaranteed to be a `Symbol`.
    return uncheckedEq(ths, other) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Symbol, crossOrder, other) {
    assertHasClass(other, CLS_Symbol);  // Not guaranteed to be a `Symbol`.

    if (ths == other) {
        // Note: This check is necessary to keep the `ZSAME` case below from
        // incorrectly claiming an unlisted symbol is unordered with
        // respect to itself.
        return SYM(same);
    }

    SymbolInfo *info1 = getInfo(ths);
    SymbolInfo *info2 = getInfo(other);
    bool interned = info1->interned;

    if (interned != info2->interned) {
        return interned ? SYM(less) : SYM(more);
    }

    switch (zstringOrder(info1->s, info2->s)) {
        case ZLESS: { return SYM(less); }
        case ZMORE: { return SYM(more); }
        case ZSAME: {
            // Per spec, two different unlisted symbols with the same name
            // are unordered with respect to each other.
            return interned ? SYM(same) : NULL;
        }
    }
}

// Documented in spec.
METH_IMPL_0(Symbol, debugString) {
    SymbolInfo *info = getInfo(ths);
    const char *prefix = info->interned ? "@" : "@+";

    return cm_cat(stringFromUtf8(-1, prefix), cm_castFrom(CLS_String, ths));
}

// Documented in spec.
METH_IMPL_0(Symbol, debugSymbol) {
    return ths;
}

// Documented in spec.
METH_IMPL_0(Symbol, isInterned) {
    return (getInfo(ths)->interned) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_0(Symbol, toUnlisted) {
    SymbolInfo *info = getInfo(ths);
    return makeSymbol0(info->s, false);
}

// Documented in header.
void bindMethodsForSymbol(void) {
    classBindMethods(CLS_Symbol,
        NULL,
        METH_TABLE(
            METH_BIND(Symbol, cat),
            METH_BIND(Symbol, crossEq),
            METH_BIND(Symbol, crossOrder),
            METH_BIND(Symbol, debugString),
            METH_BIND(Symbol, debugSymbol),
            METH_BIND(Symbol, isInterned),
            METH_BIND(Symbol, toUnlisted)));
}

/** Initializes the module. */
MOD_INIT(Symbol) {
    MOD_USE(Core);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForSymbol()`.
}

// Documented in header.
zvalue CLS_Symbol = NULL;
