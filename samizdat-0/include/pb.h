/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Lowest-layer plumbing and data structures
 */

#ifndef _PB_H_
#define _PB_H_

#include "ztype.h"

#include <stddef.h>
#include <stdbool.h>


/*
 * C Types and macros
 */

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct PbHeader *zvalue;

/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    zvalue key;
    zvalue value;
} zmapping;

/**
 * Prototype for an underlying C function corresponding to an in-model
 * function (value of type `Function`).
 */
typedef zvalue (*zfunction)(zint argCount, const zvalue *args);

/** Type for local value stack pointers. */
typedef const zvalue *zstackPointer;

enum {
    /**
     * (Private) Size of the `PbHeader` struct; used so that `pbOffset`
     * can be an inline function.
     */
    PB_HEADER_SIZE =
        (sizeof(zvalue) * 3) + (sizeof(int32_t) * 2) + sizeof(zint)
};

/** Declaration for a method on the given type with the given name. */
#define METH_IMPL(type, name) \
    static zvalue type##_##name(zint argCount, const zvalue *args)

/** Performs binding of the indicated method. */
#define METH_BIND(type, name) \
    do { genericBindCore(GFN_##name, TYPE_##type, type##_##name); } while(0)


/*
 * Initialization Declarations
 */

/**
 * Initializes the `pb` module.
 */
void pbInit(void);


/*
 * Assertion Declarations
 */

/**
 * Asserts that the given size accommodates accessing the `n`th element.
 * This includes asserting that `n >= 0`. Note that all non-negative `n`
 * are valid for accessing ints (their size notwithstanding).
 */
void pbAssertNth(zint size, zint n);

/**
 * Like `pbAssertNth` but also accepts the case where `n` is the size
 * of the value.
 */
void pbAssertNthOrSize(zint size, zint n);

/**
 * Asserts that the given range is valid for a `slice`-like operation
 * for a value of the given size.
 */
void pbAssertSliceRange(zint size, zint start, zint end);

/**
 * Asserts that the given value is a valid `zvalue` (non-`NULL` and
 * seems to actually have the right form). This performs reasonable,
 * but not exhaustive, tests. If not valid, this aborts the process
 * with a diagnostic message.
 */
void pbAssertValid(zvalue value);

/**
 * Like `pbAssertValid` except that `NULL` is accepted too.
 */
void pbAssertValidOrNull(zvalue value);


/*
 * `Value` Type Declarations
 */

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
 * Gets the data payload of the given value, if possible. `value` must be a
 * valid value (in particular, non-`NULL`). This is a convenient shorthand
 * for calling `derivDataOf(value, NULL)`. For everything but derived
 * values, the data payload is the same as the value itself. For derived
 * values, the data payload is (unsurprisingly) `NULL` for type-only
 * values.
 */
zvalue dataOf(zvalue value);

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
zvalue derivDataOf(zvalue value, zvalue secret);

/**
 * Gets a unique identity number associated with this value. Only works
 * on values of an opaque type, and only if the type is marked as
 * `identified`.
 */
zint identityOf(zvalue value);

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
zvalue makeDeriv(zvalue type, zvalue data, zvalue secret);

/**
 * Returns a transparent derived value with the given type tag, and with the
 * given optional data payload. This is a convenient shorthand for calling
 * `makeDeriv(type, data, NULL)`.
 */
zvalue makeValue(zvalue type, zvalue data);

/**
 * Gets the "debug string" of the given value, as a `char *`. The caller
 * is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *pbDebugString(zvalue value);

/**
 * Compares two values for equality. This exists in addition to
 * `pbOrder`, because it is possible for this function run much
 * quicker in the not-equal case. As with `pbOrder`, this accepts
 * `NULL` as a value, treating it as not the same as any other value.
 */
bool pbEq(zvalue v1, zvalue v2);

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
zorder pbOrder(zvalue v1, zvalue v2);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * See the *Samizdat Layer 0* specification for details on
 * what low-layer "size" means.
 */
zint pbSize(zvalue value);


/*
 * Memory Management Declarations
 */

/**
 * Allocates a value, assigning it the given type, and sizing the memory
 * to include the given amount of extra bytes as raw payload data.
 * The resulting value is added to the live reference stack.
 */
zvalue pbAllocValue(zvalue type, zint extraBytes);

/**
 * Adds an item to the current frame. This is only necessary to call when
 * a reference gets "detached" from a live structure (via mutation), which
 * is to say, rarely.
 */
void pbFrameAdd(zvalue value);

/**
 * Indicates the start of a new frame of references on the stack.
 * The return value can subsequently be passed to `pbFrameEnd` to
 * indicate that this frame is no longer active.
 */
zstackPointer pbFrameStart(void);

/**
 * Indicates that the frame whose start returned the given stack pointer
 * should be reset to a state that *only* includes the given value
 * (or is totally reset if `stackedValue` is `NULL`).
 */
void pbFrameReset(zstackPointer savedStack, zvalue stackedValue);

/**
 * Indicates that the frame whose start returned the given stack pointer
 * is no longer active. If the given additional value is non-`NULL` it is
 * added to the frame being "returned" to. It is valid to return to any
 * frame above the current one, not just the immediately-previous frame;
 * non-immediate return can happen during a nonlocal exit.
 */
void pbFrameReturn(zstackPointer savedStack, zvalue returnValue);

/**
 * Forces a gc.
 */
void pbGc(void);

/**
 * Marks the given value as "immortal." It is considered a root and
 * will never get freed.
 */
void pbImmortalize(zvalue value);

/**
 * Marks a value during garbage collection. This in turn calls a type-specific
 * mark function when appropriate and may recurse arbitrarily. It is valid
 * to pass `NULL` to this, but no other non-values are acceptable.
 */
void pbMark(zvalue value);

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
inline void *pbPayload(zvalue value) {
    return (void *) (((char *) value) + PB_HEADER_SIZE);
}

#endif
