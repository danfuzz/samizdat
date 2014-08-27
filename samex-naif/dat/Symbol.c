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
static zvalue makeSymbol0(zvalue name, bool interned) {
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
zvalue anonymousSymbolFromUtf8(zint stringBytes, const char *string) {
    return makeSymbol0(stringFromUtf8(stringBytes, string), false);
}

// Documented in header.
zvalue makeSymbol(zvalue name) {
    zvalue result = findInternedSymbol(name);

    if (result == NULL) {
        if (!hasClass(name, CLS_String)) {
            die("Improper symbol name: %s", valDebugString(name));
        }
        result = makeSymbol0(name, true);
    }

    return result;
}

// Documented in header.
zvalue symbolCall(zvalue symbol, zint argCount, const zvalue *args) {
    if (argCount < 1) {
        die("Too few arguments for symbol call.");
    }

    SymbolInfo *info = getInfo(symbol);
    zint index = info->index;
    zvalue cls = get_class(args[0]);
    zvalue function = classFindMethodBySymbolIndex(cls, index);

    if (function == NULL) {
        die("Unbound method: %s.%s",
            valDebugString(cls), valDebugString(info->name));
    }

    UTIL_TRACE_START(callReporter, cls);
    zvalue result = funCall(function, argCount, args);
    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue symbolFromUtf8(zint stringBytes, const char *string) {
    return makeSymbol(stringFromUtf8(stringBytes, string));
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
FUNC_IMPL_1(Symbol_makeSymbol, name) {
    return makeSymbol(name);
}

// Documented in header.
METH_IMPL_rest(Symbol, call, args) {
    return symbolCall(ths, argsSize, args);
}

// Documented in header.
METH_IMPL_0(Symbol, debugString) {
    SymbolInfo *info = getInfo(ths);
    const char *prefix = info->interned ? "@." : "@?";

    return METH_CALL(cat, stringFromUtf8(-1, prefix), info->name);
}

// Documented in header.
METH_IMPL_0(Symbol, debugSymbol) {
    return ths;
}

// Documented in header.
METH_IMPL_0(Symbol, gcMark) {
    SymbolInfo *info = getInfo(ths);

    datMark(info->name);
    return NULL;
}

// Documented in header.
METH_IMPL_0(Symbol, makeAnonymous) {
    SymbolInfo *info = getInfo(ths);
    return makeSymbol0(info->name, false);
}

/** Function (not method) `symbolIsInterned`. Documented in spec. */
METH_IMPL_0(Symbol, symbolIsInterned) {
    // TODO: Should be an instance method.
    assertHasClass(ths, CLS_Symbol);

    SymbolInfo *info = getInfo(ths);
    return (info->interned) ? ths : NULL;
}

/** Function (not method) `symbolName`. Documented in spec. */
METH_IMPL_0(Symbol, symbolName) {
    // TODO: Should be an instance method.
    assertHasClass(ths, CLS_Symbol);

    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_1(Symbol, totalOrder, other) {
    assertHasClass(other, CLS_Symbol);  // Not guaranteed to be a `Symbol`.

    if (ths == other) {
        // Note: This check is necessary to keep the `ZSAME` case below from
        // incorrectly claiming an anonymous symbol is unordered with
        // respect to itself.
        return INT_0;
    }

    SymbolInfo *info1 = getInfo(ths);
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
    MOD_USE(Value);

    SYM_INIT(makeAnonymous);

    // Note: The `objectModel` module initializes `CLS_Symbol`.
    classBindMethods(CLS_Symbol,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Symbol, call),
            METH_BIND(Symbol, debugString),
            METH_BIND(Symbol, debugSymbol),
            METH_BIND(Symbol, gcMark),
            METH_BIND(Symbol, makeAnonymous),
            METH_BIND(Symbol, totalOrder),
            NULL));

    FUN_Symbol_makeSymbol = datImmortalize(FUNC_VALUE(Symbol_makeSymbol));
    FUN_Symbol_symbolIsInterned =
        datImmortalize(FUNC_VALUE(Symbol_symbolIsInterned));
    FUN_Symbol_symbolName =
        datImmortalize(FUNC_VALUE(Symbol_symbolName));
}

// Documented in header.
zvalue CLS_Symbol = NULL;

// Documented in header.
SYM_DEF(makeAnonymous);

// Documented in header.
zvalue FUN_Symbol_makeSymbol = NULL;

// Documented in header.
zvalue FUN_Symbol_symbolIsInterned = NULL;

// Documented in header.
zvalue FUN_Symbol_symbolName = NULL;
