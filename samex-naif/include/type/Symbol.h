// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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


/**
 * Like `symbolFromUtf8`, except this makes an unlisted (uninterned) symbol.
 */
zvalue unlistedSymbolFromUtf8(zint utfBytes, const char *utf);

/**
 * Concatenates two (just two) symbols together, resulting in an interned
 * symbol. This is like calling `symbol1.cat(symbol2)`, execpt that this
 * is safe to call before `Symbol`'s methods are bound.
 */
zvalue symbolCat(zvalue symbol1, zvalue symbol2);

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
 * Gets the interned symbol corresponding to a `zorder` value.
 */
zvalue symbolFromZorder(zorder order);

/**
 * Makes an interned symbol from a `zstring`.
 */
zvalue symbolFromZstring(zstring name);

/**
 * Gets the integer index of the given symbol.
 */
zint symbolIndex(zvalue symbol);

/**
 * Sorts an array of symbols by index, in place.
 */
void symbolSort(zint count, zvalue *symbols);

/**
 * Gets the `zorder` value corresponding to the given interned symbol.
 */
zorder zorderFromSymbol(zvalue symbol);

/**
 * Gets a `zstring` of the given symbol. The result `chars` shares storage
 * with the `symbol`. As such, it is important to *not* modify the contents.
 */
zstring zstringFromSymbol(zvalue symbol);

#endif
