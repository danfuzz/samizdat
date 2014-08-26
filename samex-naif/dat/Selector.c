// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>
#include <stdio.h>

#include "type/Builtin.h"
#include "type/Int.h"
#include "type/Selector.h"
#include "type/String.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/** Next selector index to assign. */
static zint theNextIndex = 0;

/** Array of all interned selectors, in sort order (possibly stale). */
static zvalue theInternedSelectors[DAT_MAX_SELECTORS];

/** The number of interned selectors. */
static zint theInternedSelectorCount = 0;

/** Whether `theInternedSelectors` needs a sort. */
static bool theNeedSort = false;

/**
 * Selector structure.
 */
typedef struct {
    /** Name of the selector. Always a string. */
    zvalue name;

    /** Whether this instance is interned. */
    bool interned;

    /** Index of the selector. No two selectors have the same index. */
    zint index;
} SelectorInfo;

/**
 * Gets a pointer to the value's info.
 */
static SelectorInfo *getInfo(zvalue selector) {
    return datPayload(selector);
}

/**
 * Creates and returns a new selector with the given name. Does no checking
 * other than that there aren't already too many selectors.
 */
static zvalue makeSelector(zvalue name, bool interned) {
    if (theNextIndex >= DAT_MAX_SELECTORS) {
        die("Too many selectors!");
    }

    zvalue result = datAllocValue(CLS_Selector, sizeof(SelectorInfo));
    SelectorInfo *info = getInfo(result);

    info->name = name;
    info->interned = interned;
    info->index = theNextIndex;
    theNextIndex++;

    if (interned) {
        theInternedSelectors[theInternedSelectorCount] = result;
        theInternedSelectorCount++;
        theNeedSort = true;
    }

    datImmortalize(result);
    return result;
}

/**
 * Compares a name with a selector. Common function used for searching,
 * sorting, and ordering.
 */
static int compareNameAndSelector(zvalue name, zvalue selector) {
    SelectorInfo *info = getInfo(selector);
    zvalue name2 = info->name;

    return stringZorder(name, name2);
}

/**
 * Compares two selectors. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue v1 = *(zvalue *) vptr1;
    zvalue v2 = *(zvalue *) vptr2;
    SelectorInfo *info1 = getInfo(v1);

    return compareNameAndSelector(info1->name, v2);
}

/**
 * Compares a name with a selector. Used for searching.
 */
static int searchOrder(const void *key, const void *vptr) {
    zvalue name = (zvalue) key;
    zvalue selector = *(zvalue *) vptr;

    return compareNameAndSelector(name, selector);
}

/**
 * Finds an existing interned selector with the given name, if any.
 */
static zvalue findInternedSelector(zvalue name) {
    if (theNeedSort) {
        mergesort(
            theInternedSelectors, theInternedSelectorCount,
            sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue *found = (zvalue *) bsearch(
        name, theInternedSelectors, theInternedSelectorCount,
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
    return makeSelector(name, false);
}

// Documented in header.
zvalue selectorCall(zvalue selector, zint argCount, const zvalue *args) {
    if (argCount < 1) {
        die("Too few arguments for selector call.");
    }

    zint index = getInfo(selector)->index;
    zvalue cls = get_class(args[0]);
    zvalue function = classFindMethodBySelectorIndex(cls, index);

    if (function == NULL) {
        die("Unbound method: %s.%s",
            valDebugString(cls), valDebugName(selector));
    }

    UTIL_TRACE_START(callReporter, cls);
    zvalue result = funCall(function, argCount, args);
    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue makeInternedSymbol(zvalue name) {
    zvalue result = findInternedSelector(name);

    if (result == NULL) {
        if (!hasClass(name, CLS_String)) {
            die("Improper selector name: %s", valDebugString(name));
        }
        result = makeSelector(name, true);
    }

    return result;
}

// Documented in header.
zvalue selectorFromUtf8(zint stringBytes, const char *string) {
    return makeInternedSymbol(stringFromUtf8(stringBytes, string));
}

// Documented in header.
zvalue symbolName(zvalue selector) {
    assertHasClass(selector, CLS_Selector);
    return getInfo(selector)->name;
}

// Documented in header.
zint selectorIndex(zvalue selector) {
    assertHasClass(selector, CLS_Selector);
    return getInfo(selector)->index;
}

// Documented in header.
char *utf8DupFromSelector(zvalue selector) {
    assertHasClass(selector, CLS_Selector);
    SelectorInfo *info = getInfo(selector);

    return utf8DupFromString(info->name);
}

// Documented in header.
zint utf8FromSelector(zint resultSize, char *result, zvalue selector) {
    assertHasClass(selector, CLS_Selector);
    SelectorInfo *info = getInfo(selector);

    return utf8FromSelector(resultSize, result, info->name);
}

// Documented in header.
zint utf8SizeFromSelector(zvalue selector) {
    assertHasClass(selector, CLS_Selector);
    SelectorInfo *info = getInfo(selector);

    return utf8SizeFromString(info->name);
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Selector, call) {
    // The first argument is the selector, and the rest are the
    // arguments to call it with.
    return selectorCall(args[0], argCount - 1, &args[1]);
}

// Documented in header.
METH_IMPL(Selector, debugName) {
    zvalue selector = args[0];
    SelectorInfo *info = getInfo(selector);

    return info->name;
}

// Documented in header.
METH_IMPL(Selector, debugString) {
    zvalue selector = args[0];
    SelectorInfo *info = getInfo(selector);
    const char *prefix = info->interned ? "@." : "@?";

    return METH_CALL(cat, stringFromUtf8(-1, prefix), info->name);
}

// Documented in header.
METH_IMPL(Selector, gcMark) {
    zvalue selector = args[0];
    SelectorInfo *info = getInfo(selector);

    datMark(info->name);
    return NULL;
}

/** Function (not method) `makeAnonymousSymbol`. Documented in spec. */
METH_IMPL(Selector, makeAnonymousSymbol) {
    return makeAnonymousSymbol(args[0]);
}

/** Function (not method) `symbolIsInterned`. Documented in spec. */
METH_IMPL(Selector, symbolIsInterned) {
    // TODO: Should be an instance method.
    zvalue selector = args[0];
    assertHasClass(selector, CLS_Selector);

    SelectorInfo *info = getInfo(selector);
    return (info->interned) ? selector : NULL;
}

/** Function (not method) `symbolName`. Documented in spec. */
METH_IMPL(Selector, symbolName) {
    // TODO: Should be an instance method.
    zvalue selector = args[0];
    assertHasClass(selector, CLS_Selector);

    SelectorInfo *info = getInfo(selector);
    return info->name;
}

/** Function (not method) `makeInternedSymbol`. Documented in spec. */
METH_IMPL(Selector, makeInternedSymbol) {
    return makeInternedSymbol(args[0]);
}

// Documented in header.
METH_IMPL(Selector, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `Selector`.

    assertHasClass(other, CLS_Selector);

    if (value == other) {
        // Note: This check is necessary to keep the `ZSAME` case below from
        // incorrectly claiming an anonymous selector is unordered with
        // respect to itself.
        return INT_0;
    }

    SelectorInfo *info1 = getInfo(value);
    SelectorInfo *info2 = getInfo(other);
    bool interned = info1->interned;

    if (interned != info2->interned) {
        return interned ? INT_NEG1 : INT_1;
    }

    zorder order = stringZorder(info1->name, info2->name);
    switch (order) {
        case ZLESS: return INT_NEG1;
        case ZMORE: return INT_1;
        case ZSAME: {
            // Per spec, two different anonymous selectors with the same name
            // are unordered with respect to each other.
            return interned ? INT_0 : NULL;
        }
    }
}

/** Initializes the module. */
MOD_INIT(Selector) {
    MOD_USE(Function);

    // Note: The `objectModel` module initializes `CLS_Selector`.
    classBindMethods(CLS_Selector,
        NULL,
        symbolTableFromArgs(
            SEL_METH(Selector, call),
            SEL_METH(Selector, debugName),
            SEL_METH(Selector, debugString),
            SEL_METH(Selector, gcMark),
            SEL_METH(Selector, totalOrder),
            NULL));

    FUN_Selector_makeAnonymousSymbol = makeBuiltin(1, 1,
        METH_NAME(Selector, makeAnonymousSymbol), 0,
        stringFromUtf8(-1, "Selector.makeAnonymousSymbol"));
    datImmortalize(FUN_Selector_makeAnonymousSymbol);

    FUN_Selector_makeInternedSymbol = makeBuiltin(1, 1,
        METH_NAME(Selector, makeInternedSymbol), 0,
        stringFromUtf8(-1, "Selector.makeInternedSymbol"));
    datImmortalize(FUN_Selector_makeInternedSymbol);

    FUN_Selector_symbolIsInterned = makeBuiltin(1, 1,
        METH_NAME(Selector, symbolIsInterned), 0,
        stringFromUtf8(-1, "Selector.symbolIsInterned"));
    datImmortalize(FUN_Selector_symbolIsInterned);

    FUN_Selector_symbolName = makeBuiltin(1, 1,
        METH_NAME(Selector, symbolName), 0,
        stringFromUtf8(-1, "Selector.symbolName"));
    datImmortalize(FUN_Selector_symbolName);
}

// Documented in header.
zvalue CLS_Selector = NULL;

// Documented in header.
zvalue FUN_Selector_makeAnonymousSymbol = NULL;

// Documented in header.
zvalue FUN_Selector_makeInternedSymbol = NULL;

// Documented in header.
zvalue FUN_Selector_symbolIsInterned = NULL;

// Documented in header.
zvalue FUN_Selector_symbolName = NULL;
