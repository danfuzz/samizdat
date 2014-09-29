// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Value` base class
//
// **Note:** As a slight concession to conciseness, the name prefix used for
// this class is `val` and not `value`.
//

#ifndef _TYPE_VALUE_H_
#define _TYPE_VALUE_H_

#include <stdbool.h>
#include <stddef.h>

#include "type/declare.h"


/** Class value for in-model class `Value`. */
extern zvalue CLS_Value;

/** Method `.call(args*)`: Documented in spec. */
SYM_DECL(call);

/** Method `.debugString()`: Documented in spec. */
SYM_DECL(debugString);

/** Method `.debugSymbol()`: Documented in spec. */
SYM_DECL(debugSymbol);

/**
 * Symbol `exports`. Used when accessing modules. Not needed in `dat` per se,
 * but defined here, because there isn't really a better place to put it.
 */
SYM_DECL(exports);

/** Symbol `imports`. Same comment applies as with `exports`, above. */
SYM_DECL(imports);

/**
 * Method `.gcMark()`: Does GC marking for the given value.
 *
 * TODO: This should be defined as an unlisted symbol and *not* exported
 * in any way to the higher layer environment.
 */
SYM_DECL(gcMark);

/** Method `.get_name()`: Documented in spec. */
SYM_DECL(get_name);

/** Method `.hasName()`: Documented in spec. */
SYM_DECL(hasName);

/** Symbol `main`. Same comment applies as with `exports`, above. */
SYM_DECL(main);

/** Method `.perEq(other)`: Documented in spec. */
SYM_DECL(perEq);

/** Method `.perOrder(other)`: Documented in spec. */
SYM_DECL(perOrder);

/** Symbol `resources`. Same comment applies as with `exports`, above. */
SYM_DECL(resources);

/** Method `.toString()`: Documented in spec. */
SYM_DECL(toString);

/** Method `.totalEq(other)`: Documented in spec. */
SYM_DECL(totalEq);

/** Method `.totalOrder(other)`: Documented in spec. */
SYM_DECL(totalOrder);

/**
 * Calls the method `value.get_name()`.
 */
zvalue get_name(zvalue value);

/**
 * Gets the "debug string" of the given value, as a `char *`. The caller
 * is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *valDebugString(zvalue value);

/**
 * Compares two values for equality, returning the first argument to
 * represent logical-true or `NULL` for logical-false. This calls through
 * to `totalEq` given values of the same class. **Note:** It is invalid to
 * pass `NULL` to this function.
 */
zvalue valEq(zvalue value, zvalue other);

/**
 * Like `valEq`, except that `NULL`s are accepted as arguments, and the
 * function returns a `bool` (of necessity, since a `zvalue` result would be
 * ambiguous).
 */
bool valEqNullOk(zvalue value, zvalue other);

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ -1, 0, 1 }`, with the usual comparison result meaning.
 * This calls through to `totalOrder` given values of the same class. **Note:**
 * It is invalid to pass `NULL` to this function.
 */
zvalue valOrder(zvalue value, zvalue other);

/**
 * Calls `.toString()` on the given value.
 */
zvalue valToString(zvalue value);

/**
 * Like `valOrder`, except that the return value is of type `zorder` and this
 * reports a fatal error if given incomparable values.
 *
 * **Note:** The constants `{ ZLESS, ZSAME, ZMORE }` can be used when looking
 * at results.
 */
zorder valZorder(zvalue value, zvalue other);

#endif
