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
    /** Name of the method. Always a string. */
    zvalue methodName;

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
static zvalue makeSelector(zvalue methodName, bool interned) {
    if (theNextIndex >= DAT_MAX_SELECTORS) {
        die("Too many method selectors!");
    }

    zvalue result = datAllocValue(CLS_Selector, sizeof(SelectorInfo));
    SelectorInfo *info = getInfo(result);

    info->methodName = methodName;
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
 * Compares a method name with a selector. Common function used for searching,
 * sorting, and ordering.
 */
static int compareNameAndSelector(zvalue methodName, zvalue selector) {
    SelectorInfo *info = getInfo(selector);
    zvalue methodName2 = info->methodName;

    return stringZorder(methodName, methodName2);
}

/**
 * Compares two selectors. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue v1 = *(zvalue *) vptr1;
    zvalue v2 = *(zvalue *) vptr2;
    SelectorInfo *info1 = getInfo(v1);

    return compareNameAndSelector(info1->methodName, v2);
}

/**
 * Compares a method name with a selector. Used for searching.
 */
static int searchOrder(const void *key, const void *vptr) {
    zvalue methodName = (zvalue) key;
    zvalue selector = *(zvalue *) vptr;

    return compareNameAndSelector(methodName, selector);
}

/**
 * Finds an existing interned selector with the given name, if any.
 */
static zvalue findInternedSelector(zvalue methodName) {
    if (theNeedSort) {
        mergesort(
            theInternedSelectors, theInternedSelectorCount,
            sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue *found = (zvalue *) bsearch(
        methodName, theInternedSelectors, theInternedSelectorCount,
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
zvalue makeAnonymousSelector(zvalue methodName) {
    return makeSelector(methodName, false);
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
        die("Unbound method: %s%s",
            valDebugString(cls), valDebugString(selector));
    }

    UTIL_TRACE_START(callReporter, cls);
    zvalue result = funCall(function, argCount, args);
    UTIL_TRACE_END();
    return result;
}

// Documented in header.
zvalue selectorFromName(zvalue methodName) {
    zvalue result = findInternedSelector(methodName);

    if (result == NULL) {
        if (!hasClass(methodName, CLS_String)) {
            die("Improper method name: %s", valDebugString(methodName));
        }
        result = makeSelector(methodName, true);
    }

    return result;
}

// Documented in header.
zint selectorIndex(zvalue selector) {
    assertHasClass(selector, CLS_Selector);
    return getInfo(selector)->index;
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

    return info->methodName;
}

// Documented in header.
METH_IMPL(Selector, debugString) {
    zvalue selector = args[0];
    SelectorInfo *info = getInfo(selector);
    const char *prefix = info->interned ? "@." : "@?";

    return METH_CALL(cat, stringFromUtf8(-1, prefix), info->methodName);
}

// Documented in header.
METH_IMPL(Selector, gcMark) {
    zvalue selector = args[0];
    SelectorInfo *info = getInfo(selector);

    datMark(info->methodName);
    return NULL;
}

/** Function (not method) `makeAnonymousSelector`. Documented in spec. */
METH_IMPL(Selector, makeAnonymousSelector) {
    return makeAnonymousSelector(args[0]);
}

/** Function (not method) `selectorIsInterned`. Documented in spec. */
METH_IMPL(Selector, selectorIsInterned) {
    // TODO: Should be an instance method.
    zvalue selector = args[0];
    assertHasClass(selector, CLS_Selector);

    SelectorInfo *info = getInfo(selector);
    return (info->interned) ? selector : NULL;
}

/** Function (not method) `selectorName`. Documented in spec. */
METH_IMPL(Selector, selectorName) {
    // TODO: Should be an instance method.
    zvalue selector = args[0];
    assertHasClass(selector, CLS_Selector);

    SelectorInfo *info = getInfo(selector);
    return info->methodName;
}

/** Function (not method) `selectorFromName`. Documented in spec. */
METH_IMPL(Selector, selectorFromName) {
    return selectorFromName(args[0]);
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

    zorder order = stringZorder(info1->methodName, info2->methodName);
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

    METH_BIND(Selector, call);
    METH_BIND(Selector, debugName);
    METH_BIND(Selector, debugString);
    METH_BIND(Selector, gcMark);
    METH_BIND(Selector, totalOrder);

    FUN_Selector_makeAnonymousSelector = makeBuiltin(1, 1,
        METH_NAME(Selector, makeAnonymousSelector), 0,
        stringFromUtf8(-1, "Selector.makeAnonymousSelector"));
    datImmortalize(FUN_Selector_makeAnonymousSelector);

    FUN_Selector_selectorFromName = makeBuiltin(1, 1,
        METH_NAME(Selector, selectorFromName), 0,
        stringFromUtf8(-1, "Selector.selectorFromName"));
    datImmortalize(FUN_Selector_selectorFromName);

    FUN_Selector_selectorIsInterned = makeBuiltin(1, 1,
        METH_NAME(Selector, selectorIsInterned), 0,
        stringFromUtf8(-1, "Selector.selectorIsInterned"));
    datImmortalize(FUN_Selector_selectorIsInterned);

    FUN_Selector_selectorName = makeBuiltin(1, 1,
        METH_NAME(Selector, selectorName), 0,
        stringFromUtf8(-1, "Selector.selectorName"));
    datImmortalize(FUN_Selector_selectorName);
}

// Documented in header.
zvalue CLS_Selector = NULL;

// Documented in header.
zvalue FUN_Selector_makeAnonymousSelector = NULL;

// Documented in header.
zvalue FUN_Selector_selectorFromName = NULL;

// Documented in header.
zvalue FUN_Selector_selectorIsInterned = NULL;

// Documented in header.
zvalue FUN_Selector_selectorName = NULL;