// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>
#include <stdio.h>

#include "type/Builtin.h"
#include "type/Int.h"
#include "type/Symbol.h"
#include "type/String.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

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
    /** Name of the symbol. Always a string. */
    zvalue name;

    /** Whether this instance is interned. */
    bool interned;

    /** Index of the symbol. No two symbols have the same index. */
    zint index;
} SymbolInfo;

/**
 * Gets a pointer to the value's info.
 */
static SymbolInfo *getInfo(zvalue symbol) {
    return datPayload(symbol);
}

/**
 * Creates and returns a new symbol with the given name. Does no checking
 * other than that there aren't already too many symbols.
 */
static zvalue makeSymbol(zvalue name, bool interned) {
    if (theNextIndex >= DAT_MAX_SYMBOLS) {
        die("Too many symbols!");
    }

    zvalue result = datAllocValue(CLS_Symbol, sizeof(SymbolInfo));
    SymbolInfo *info = getInfo(result);

    info->name = name;
    info->interned = interned;
    info->index = theNextIndex;
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
 * Compares a name with a symbol. Common function used for searching,
 * sorting, and ordering.
 */
static int compareNameAndSymbol(zvalue name, zvalue symbol) {
    SymbolInfo *info = getInfo(symbol);
    zvalue name2 = info->name;

    return stringZorder(name, name2);
}

/**
 * Compares two symbols. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue v1 = *(zvalue *) vptr1;
    zvalue v2 = *(zvalue *) vptr2;
    SymbolInfo *info1 = getInfo(v1);

    return compareNameAndSymbol(info1->name, v2);
}

/**
 * Compares a name with a symbol. Used for searching.
 */
static int searchOrder(const void *key, const void *vptr) {
    zvalue name = (zvalue) key;
    zvalue symbol = *(zvalue *) vptr;

    return compareNameAndSymbol(name, symbol);
}

/**
 * Finds an existing interned symbol with the given name, if any.
 */
static zvalue findInternedSymbol(zvalue name) {
    if (theNeedSort) {
        mergesort(
            theInternedSymbols, theInternedSymbolCount,
            sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue *found = (zvalue *) bsearch(
        name, theInternedSymbols, theInternedSymbolCount,
        sizeof(zvalue), searchOrder);

    return (found == NULL) ? NULL : *found;
}

/**
 * This is the function that handles emitting a context string for a call,
 * when dumping the stack.
 */
static char *callReporter(void *state) {
    zvalue cls = state;
    char *clsString = valDebugString(cls);
    char *result;

    asprintf(&result, "class %s", clsString);
    free(clsString);

    return result;
}


//
// Exported Definitions
//

// Documented in header.
zvalue makeAnonymousSymbol(zvalue name) {
    return makeSymbol(name, false);
}

// Documented in header.
zvalue symbolCall(zvalue symbol, zint argCount, const zvalue *args) {
    if (argCount < 1) {
        die("Too few arguments for symbol call.");
    }

    zint index = getInfo(symbol)->index;
    zvalue cls = get_class(args[0]);
    zvalue function = classFindMethodBySymbolIndex(cls, index);

    if (function == NULL) {
        die("Unbound method: %s.%s",
            valDebugString(cls), valDebugName(symbol));
    }

    UTIL_TRACE_START(callReporter, cls);
    zvalue result = funCall(function, argCount, args);
    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue makeInternedSymbol(zvalue name) {
    zvalue result = findInternedSymbol(name);

    if (result == NULL) {
        if (!hasClass(name, CLS_String)) {
            die("Improper symbol name: %s", valDebugString(name));
        }
        result = makeSymbol(name, true);
    }

    return result;
}

// Documented in header.
zvalue symbolFromUtf8(zint stringBytes, const char *string) {
    return makeInternedSymbol(stringFromUtf8(stringBytes, string));
}

// Documented in header.
zvalue symbolName(zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    return getInfo(symbol)->name;
}

// Documented in header.
zint symbolIndex(zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    return getInfo(symbol)->index;
}

// Documented in header.
char *utf8DupFromSymbol(zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    SymbolInfo *info = getInfo(symbol);

    return utf8DupFromString(info->name);
}

// Documented in header.
zint utf8FromSymbol(zint resultSize, char *result, zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    SymbolInfo *info = getInfo(symbol);

    return utf8FromSymbol(resultSize, result, info->name);
}

// Documented in header.
zint utf8SizeFromSymbol(zvalue symbol) {
    assertHasClass(symbol, CLS_Symbol);
    SymbolInfo *info = getInfo(symbol);

    return utf8SizeFromString(info->name);
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Symbol, call) {
    // The first argument is the symbol, and the rest are the
    // arguments to call it with.
    return symbolCall(args[0], argCount - 1, &args[1]);
}

// Documented in header.
METH_IMPL(Symbol, debugName) {
    zvalue symbol = args[0];
    SymbolInfo *info = getInfo(symbol);

    return info->name;
}

// Documented in header.
METH_IMPL(Symbol, debugString) {
    zvalue symbol = args[0];
    SymbolInfo *info = getInfo(symbol);
    const char *prefix = info->interned ? "@." : "@?";

    return METH_CALL(cat, stringFromUtf8(-1, prefix), info->name);
}

// Documented in header.
METH_IMPL(Symbol, gcMark) {
    zvalue symbol = args[0];
    SymbolInfo *info = getInfo(symbol);

    datMark(info->name);
    return NULL;
}

/** Function (not method) `makeAnonymousSymbol`. Documented in spec. */
METH_IMPL(Symbol, makeAnonymousSymbol) {
    return makeAnonymousSymbol(args[0]);
}

/** Function (not method) `symbolIsInterned`. Documented in spec. */
METH_IMPL(Symbol, symbolIsInterned) {
    // TODO: Should be an instance method.
    zvalue symbol = args[0];
    assertHasClass(symbol, CLS_Symbol);

    SymbolInfo *info = getInfo(symbol);
    return (info->interned) ? symbol : NULL;
}

/** Function (not method) `symbolName`. Documented in spec. */
METH_IMPL(Symbol, symbolName) {
    // TODO: Should be an instance method.
    zvalue symbol = args[0];
    assertHasClass(symbol, CLS_Symbol);

    SymbolInfo *info = getInfo(symbol);
    return info->name;
}

/** Function (not method) `makeInternedSymbol`. Documented in spec. */
METH_IMPL(Symbol, makeInternedSymbol) {
    return makeInternedSymbol(args[0]);
}

// Documented in header.
METH_IMPL(Symbol, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `Symbol`.

    assertHasClass(other, CLS_Symbol);

    if (value == other) {
        // Note: This check is necessary to keep the `ZSAME` case below from
        // incorrectly claiming an anonymous symbol is unordered with
        // respect to itself.
        return INT_0;
    }

    SymbolInfo *info1 = getInfo(value);
    SymbolInfo *info2 = getInfo(other);
    bool interned = info1->interned;

    if (interned != info2->interned) {
        return interned ? INT_NEG1 : INT_1;
    }

    zorder order = stringZorder(info1->name, info2->name);
    switch (order) {
        case ZLESS: return INT_NEG1;
        case ZMORE: return INT_1;
        case ZSAME: {
            // Per spec, two different anonymous symbols with the same name
            // are unordered with respect to each other.
            return interned ? INT_0 : NULL;
        }
    }
}

/** Initializes the module. */
MOD_INIT(Symbol) {
    MOD_USE(Function);

    // Note: The `objectModel` module initializes `CLS_Symbol`.
    classBindMethods(CLS_Symbol,
        NULL,
        symbolTableFromArgs(
            SYM_METH(Symbol, call),
            SYM_METH(Symbol, debugName),
            SYM_METH(Symbol, debugString),
            SYM_METH(Symbol, gcMark),
            SYM_METH(Symbol, totalOrder),
            NULL));

    FUN_Symbol_makeAnonymousSymbol = makeBuiltin(1, 1,
        METH_NAME(Symbol, makeAnonymousSymbol), 0,
        stringFromUtf8(-1, "Symbol.makeAnonymousSymbol"));
    datImmortalize(FUN_Symbol_makeAnonymousSymbol);

    FUN_Symbol_makeInternedSymbol = makeBuiltin(1, 1,
        METH_NAME(Symbol, makeInternedSymbol), 0,
        stringFromUtf8(-1, "Symbol.makeInternedSymbol"));
    datImmortalize(FUN_Symbol_makeInternedSymbol);

    FUN_Symbol_symbolIsInterned = makeBuiltin(1, 1,
        METH_NAME(Symbol, symbolIsInterned), 0,
        stringFromUtf8(-1, "Symbol.symbolIsInterned"));
    datImmortalize(FUN_Symbol_symbolIsInterned);

    FUN_Symbol_symbolName = makeBuiltin(1, 1,
        METH_NAME(Symbol, symbolName), 0,
        stringFromUtf8(-1, "Symbol.symbolName"));
    datImmortalize(FUN_Symbol_symbolName);
}

// Documented in header.
zvalue CLS_Symbol = NULL;

// Documented in header.
zvalue FUN_Symbol_makeAnonymousSymbol = NULL;

// Documented in header.
zvalue FUN_Symbol_makeInternedSymbol = NULL;

// Documented in header.
zvalue FUN_Symbol_symbolIsInterned = NULL;

// Documented in header.
zvalue FUN_Symbol_symbolName = NULL;
