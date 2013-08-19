/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
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

#include "pb.h"

#include <stdbool.h>


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

/**
 * Generic `eq(value, value)`: Compares two values for equality / sameness.
 * Returns the second value to indicate logical-true. Only ever called when
 * the two values are not `==`, and only ever called when the two values are
 * of the same type. Defaults to always returning logical-false.
 */
extern zvalue GFN_eq;

/**
 * Generic `order(value, value)`: Compares two values with respect to the
 * total order of values. Returns one of `-1` `0` `1` corresponding to
 * the usual meanings for comparison. Must be implemented by every type.
 */
extern zvalue GFN_order;

/**
 * Generic `size(value)`: Gets the "size" of a value of the given type,
 * for the appropriate per-type meaning of size. Defaults to always returning
 * `0`.
 */
extern zvalue GFN_size;

/**
 * Gets the data payload of the given value, if it is a value-bearing
 * transparent derived value. `value` must be a valid value (in particular,
 * non-`NULL`). This is a convenient shorthand for calling
 * `valDataOf(value, NULL)`.
 */
zvalue dataOf(zvalue value);

/**
 * Returns a derived value with the given type tag, and with the given
 * optional data payload (`NULL` indicating a type-only value). `type` and
 * `secret` must be as follows:
 *
 * * If `type` is a value of type `Type`, then the resulting value is of
 *   that type. If `type` is an opaque type, then `secret` must match the
 *   secret known by `type`. If `type` is a transparent type, then `secret`
 *   must be `NULL`.
 *
 * * If `type` is any other value (that is, other than a `Type`), then it
 *   is taken to indicate a transparent type whose name is `type`. As such
 *   `secret` must be `NULL`.
 */
zvalue makeValue(zvalue type, zvalue data, zvalue secret);

/**
 * Returns a transparent derived value with the given type tag, and with the
 * given optional data payload. This is a convenient shorthand for calling
 * `makeValue(type, data, NULL)`.
 */
zvalue makeTransValue(zvalue type, zvalue data);

/**
 * Gets the data payload of the given value, if possible. This behaves
 * as follows:
 *
 * * If `value` is a core value, this returns `NULL`.
 *
 * * If `value`'s type secret does not match the given secret, this returns
 *   `NULL`. Notably, if `value` is of a transparent derived type and `secret`
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
 * `identified`.
 */
zint valIdentityOf(zvalue value);

/**
 * Gets the "debug string" of the given value, as a `char *`. The caller
 * is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *valDebugString(zvalue value);

/**
 * Compares two values for equality. This exists in addition to
 * `valOrder`, because it is possible for this function run much
 * quicker in the not-equal case. As with `valOrder`, this accepts
 * `NULL` as a value, treating it as not the same as any other value.
 */
bool valEq(zvalue v1, zvalue v2);

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ ZLESS, ZSAME, ZMORE }`, less symbolically equal to `{
 * -1, 0, 1 }` respectively, with the usual comparison result meaning.
 * See `totalOrder` in the Samizdat Layer 0 spec for more details about
 * value sorting.
 *
 * If `NULL` is passed as an argument, it is accepted and treated as
 * being ordered earlier than any other value.
 */
zorder valOrder(zvalue v1, zvalue v2);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * See the *Samizdat Layer 0* specification for details on
 * what low-layer "size" means.
 */
zint valSize(zvalue value);

#endif
