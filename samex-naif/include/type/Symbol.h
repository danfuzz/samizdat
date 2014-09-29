// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Symbol` class
//

#ifndef _TYPE_SYMBOL_H_
#define _TYPE_SYMBOL_H_

#include "type/Value.h"
#include "util.h"

/** Class value for in-model class `Symbol`. */
extern zvalue CLS_Symbol;

/** Method `.toUnlisted()`: Documented in spec. */
SYM_DECL(toUnlisted);

/** Method `.isInterned()`: Documented in spec. */
SYM_DECL(isInterned);


/**
 * Like `symbolFromUtf8`, except this makes an unlisted (uninterned) symbol.
 */
zvalue unlistedSymbolFromUtf8(zint utfBytes, const char *utf);

/**
 * Gets the pre-existing symbol with the given index.
 */
zvalue symbolFromIndex(zint index);

/**
 * Compares two symbols for equality. This will die with an error if one
 * or the other argument is not actually a symbol.
 */
bool symbolEq(zvalue symbol1, zvalue symbol2);

/**
 * Gets the interned symbol that corresponds to the given `name`, creating it
 * if it doesn't already exist. `name` must be a `String`.
 */
zvalue symbolFromString(zvalue name);

/**
 * Makes an interned symbol from a UTF-8 string. If `utfBytes`
 * is passed as `-1`, this relies on `utf` being `\0`-terminated.
 */
zvalue symbolFromUtf8(zint utfBytes, const char *utf);

/**
 * Makes an interned symbol from a `zstring`.
 */
zvalue symbolFromZstring(zstring name);

/**
 * Gets the integer index of the given symbol.
 */
zint symbolIndex(zvalue symbol);

/**
 * Like `utf8FromSymbol`, except this returns an allocated buffer containing
 * the result.
 */
char *utf8DupFromSymbol(zvalue symbol);

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
zint utf8FromSymbol(zint resultSize, char *result, zvalue symbol);

/**
 * Gets the number of bytes required to encode the name of the given symbol
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint utf8SizeFromSymbol(zvalue symbol);

#endif
