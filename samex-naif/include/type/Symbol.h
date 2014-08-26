// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Symbol` class
//

#ifndef _TYPE_SYMBOL_H_
#define _TYPE_SYMBOL_H_

#include "type/Value.h"

/** Class value for in-model class `Symbol`. */
extern zvalue CLS_Symbol;

/** Global function `makeAnonymousSymbol`: Documented in spec. */
extern zvalue FUN_Symbol_makeAnonymousSymbol;

/** Global function `makeInternedSymbol`: Documented in spec. */
extern zvalue FUN_Symbol_makeInternedSymbol;

/** Global function `symbolIsInterned`: Documented in spec. */
extern zvalue FUN_Symbol_symbolIsInterned;

/** Global function `symbolName`: Documented in spec. */
extern zvalue FUN_Symbol_symbolName;


/**
 * Makes an anonymous symbol. The `name` is used as the "debugging"
 * name but does not have any effect on lookup.
 */
zvalue makeAnonymousSymbol(zvalue name);

/**
 * Gets the symbol that corresponds to the given method name, creating it
 * if it doesn't already exist. `name` must be a `String`.
 */
zvalue makeInternedSymbol(zvalue name);

/**
 * Makes an interned symbol from a UTF-8 string. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue selectorFromUtf8(zint stringBytes, const char *string);

/**
 * Gets the integer index of the given symbol.
 */
zint selectorIndex(zvalue selector);

/**
 * Gets the name of the given symbol. This returns a string.
 */
zvalue symbolName(zvalue selector);

/**
 * Like `utf8FromSelector`, except this returns an allocated buffer containing
 * the result.
 */
char *utf8DupFromSelector(zvalue selector);

/**
 * Encodes the name of the given symbol as UTF-8 into the given buffer of the
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
 * Gets the number of bytes required to encode the name of the given symbol
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint utf8SizeFromSelector(zvalue selector);

#endif
