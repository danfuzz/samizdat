// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Selector` class
//

#ifndef _TYPE_SELECTOR_H_
#define _TYPE_SELECTOR_H_

#include "type/Value.h"

/** Class value for in-model class `Selector`. */
extern zvalue CLS_Selector;

/** Global function `makeAnonymousSelector`: Documented in spec. */
extern zvalue FUN_Selector_makeAnonymousSelector;

/** Global function `selectorFromName`: Documented in spec. */
extern zvalue FUN_Selector_selectorFromName;


/**
 * Makes an anonymous selector. The `methodName` is used as the "debugging"
 * name but does not have any effect on lookup.
 */
zvalue makeAnonymousSelector(zvalue methodName);

/**
 * Gets the selector that corresponds to the given method name, but only if
 * it already exists. Returns `NULL` if there is no existing selector that
 * corresponds to `methodName`.
 */
zvalue selectorFromExistingName(zvalue methodName);

/**
 * Gets the selector that corresponds to the given method name, creating it
 * if it doesn't already exist. `methodName` must be a `String`.
 */
zvalue selectorFromName(zvalue methodName);

/**
 * Gets the integer index of the given selector.
 */
zint selectorIndex(zvalue selector);

#endif
