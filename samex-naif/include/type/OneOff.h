// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// One-Off methods
//

#ifndef _TYPE_ONE_OFF_H_
#define _TYPE_ONE_OFF_H_

#include "type/Value.h"


/** Method `.cat(more*)`: Documented in spec. */
SYM_DECL(cat);

/** Method `.get(key)`: Documented in spec. */
SYM_DECL(get);

/** Method `.get_key()`: Documented in spec. */
SYM_DECL(get_key);

/** Method `.get_size()`: Documented in spec. */
SYM_DECL(get_size);

/** Method `.get_value()`: Documented in spec. */
SYM_DECL(get_value);

/** Method `.nth(int)`: Documented in spec. */
SYM_DECL(nth);

/**
 * Method `.readResource(source, type)`: Documented in spec.
 *
 * **Note:** Actually part of the `Loader` protocol, but as the nature of
 * protocols is in flux, it's more convenient to just drop it here rather
 * than cons up a whole new file which will probably get removed again fairly
 * quickly.
 */
SYM_DECL(readResource);

/**
 * Method `.resolve(source)`: Documented in spec.
 *
 * **Note:** Same situation as `readResource` above.
 */
SYM_DECL(resolve);

/** Method `.toInt()`: Documented in spec. */
SYM_DECL(toInt);

/** Method `.toNumber()`: Documented in spec. */
SYM_DECL(toNumber);

/** Method `.toString()`: Documented in spec. */
SYM_DECL(toString);

/**
 * Calls the `get` method.
 */
zvalue get(zvalue coll, zvalue key);

/**
 * Calls `nth`, converting the given `zint` index to an `Int` value.
 */
zvalue nth(zvalue value, zint index);

/**
 * Calls `nth`, converting the given `zint` index to an `Int` value, and
 * converting a non-void return value &mdash; which must be a single-character
 * `String` &mdash; to a `zint` in the range of a `zchar`. A void return
 * value gets converted to `-1`.
 */
zint nthChar(zvalue value, zint index);

/**
 * Calls `get_size` on the given value, converting the result to a `zint`.
 */
zint get_size(zvalue value);

#endif
