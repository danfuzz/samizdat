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

/** Global function `makeAnonymousSymbol`: Documented in spec. */
extern zvalue FUN_Selector_makeAnonymousSymbol;

/** Global function `makeInternedSymbol`: Documented in spec. */
extern zvalue FUN_Selector_makeInternedSymbol;

/** Global function `symbolIsInterned`: Documented in spec. */
extern zvalue FUN_Selector_symbolIsInterned;

/** Global function `symbolName`: Documented in spec. */
extern zvalue FUN_Selector_symbolName;


/**
 * Makes an anonymous selector. The `name` is used as the "debugging"
 * name but does not have any effect on lookup.
 */
zvalue makeAnonymousSymbol(zvalue name);

/**
 * Gets the selector that corresponds to the given method name, creating it
 * if it doesn't already exist. `name` must be a `String`.
 */
zvalue makeInternedSymbol(zvalue name);

/**
 * Makes an interned selector from a UTF-8 string. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue selectorFromUtf8(zint stringBytes, const char *string);

/**
 * Gets the integer index of the given selector.
 */
zint selectorIndex(zvalue selector);

/**
 * Gets the name of the given selector. This returns a string.
 */
zvalue symbolName(zvalue selector);

/**
 * Like `utf8FromSelector`, except this returns an allocated buffer containing
 * the result.
 */
char *utf8DupFromSelector(zvalue selector);

/**
 * Encodes the name of the given selector as UTF-8 into the given buffer of the
 * given size in bytes, returning the number of bytes written to. The buffer
 * must be large enough to hold the entire encoded result plus a terminating
 * `'\0'` byte; if not, this function will complain and exit the runtime.
 * To be clear, the result *is* `'\0'`-terminated, and the `'\0'` is included
 * in the result count.
 *
 * **Note:** If the given string possibly contains any `U+0` code points,
 * then the only "safe" way to use the result is as an explicitly-sized
 * buffer. (For example, `strlen()` might "lie.")
 */
zint utf8FromSelector(zint resultSize, char *result, zvalue selector);

/**
 * Gets the number of bytes required to encode the name of the given selector
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint utf8SizeFromSelector(zvalue selector);

#endif
