/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Value` data type
 *
 * **Note:** As a slight concession to conciseness, the name prefix used for
 * this type is `val` and not `value`.
 */

#ifndef _TYPE_VALUE_H_
#define _TYPE_VALUE_H_

#include "dat.h"

#include <stdbool.h>
#include <stddef.h>


/** Type value for in-model type `Value`. */
extern zvalue TYPE_Value;

/**
 * Generic `debugString(value)`: Returns a minimal string form of the
 * given value. Notably, functions and generics include their names.
 * The default implementation returns strings of the form
 * `@(TypeName @ address)`.
 */
extern zvalue GFN_debugString;

/**
 * Generic `gcMark(value)`: Does GC marking for the given value.
 */
extern zvalue GFN_gcMark;

/** Generic `perEq(value, other)`: Documented in spec. */
extern zvalue GFN_perEq;

/** Generic `perOrder(value, other)`: Documented in spec. */
extern zvalue GFN_perOrder;

/**
 * Generic `totEq(value, value)`: Compares two values for equality / sameness.
 * Documented in spec.
 */
extern zvalue GFN_totEq;

/**
 * Generic `totOrder(value, value)`: Compares two values with respect to the
 * total order of values. Documented in spec.
 */
extern zvalue GFN_totOrder;

/**
 * Gets the data payload of the given value, if it is a value-bearing
 * derived data value. `value` must be a valid value (in particular,
 * non-`NULL`). This is a convenient shorthand for calling
 * `valDataOf(value, NULL)`.
 */
zvalue dataOf(zvalue value);

/**
 * Returns a derived value with the given type tag, and with the given
 * optional data payload (`NULL` indicating a type-only value). `type` must
 * be a value of type `Type`, and the result is a value of the indicated type.
 * If `type` represents an opaque type, then `secret` must match the secret
 * known by `type`. If `type` is a derived data type, then `secret` must be
 * `NULL`.
 */
zvalue makeValue(zvalue type, zvalue data, zvalue secret);

/**
 * Gets the data payload of the given value, if possible. This behaves
 * as follows:
 *
 * * If `value` is a core value, this returns `NULL`.
 *
 * * If `value`'s type secret does not match the given secret, this returns
 *   `NULL`. Notably, if `value` is of a derived data type and `secret`
 *   is *not* passed as `NULL`, this returns `NULL`.
 *
 * * If `value` does not have any payload data, this returns `NULL`.
 *
 * * Otherwise, this returns the payload data of `value`.
 *
 * **Note:** The function name reflects the fact that it will only possibly
 * return non-`NULL` for a derived value.
 *
 */
zvalue valDataOf(zvalue value, zvalue secret);

/**
 * Gets a unique identity number associated with this value. Only works
 * on values of an opaque type, and only if the type is marked as
 * `selfish`.
 */
zint valSelfIdOf(zvalue value);

/**
 * Gets the "debug string" of the given value, as a `char *`. The caller
 * is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *valDebugString(zvalue value);

/**
 * Compares two values for equality, returning the first argument to
 * represent logical-true or `NULL` for logical-false. This calls through
 * to `totEq` given values of the same type. **Note:** It is invalid to
 * pass `NULL` to this function.
 */
zvalue valEq(zvalue v1, zvalue v2);

/**
 * Like `valEq`, except that `NULL`s are accepted as arguments, and the
 * function returns a `bool`.
 */
bool valEqNullOk(zvalue v1, zvalue v2);

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ ZLESS, ZSAME, ZMORE }`, less symbolically equal to `{
 * -1, 0, 1 }` respectively, with the usual comparison result meaning.
 * See `totOrder` in the Samizdat Layer 0 spec for more details about
 * value sorting.
 *
 * If `NULL` is passed as an argument, it is accepted and treated as
 * being ordered earlier than any other value.
 */
zorder valZorder(zvalue v1, zvalue v2);

#endif
