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
#include <stdbool.h>


/*
 * C Types
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
 * Prototype for all in-model functions. The `state` is arbitrary closure
 * state (passed in when the function was bound).
 */
typedef zvalue (*zfunction)(zvalue state, zint argCount, const zvalue *args);

/** Type for local value stack pointers. */
typedef const zvalue *zstackPointer;


/*
 * Constants, type references and generic functions
 */

/** The standard value `""`. */
extern zvalue EMPTY_STRING;

/** The standard value `0`. */
extern zvalue PB_0;

/** The standard value `1`. */
extern zvalue PB_1;

/** The standard value `-1`. */
extern zvalue PB_NEG1;

/** The `secret` value to use when defining core types. */
extern zvalue PB_SECRET;

/** Type value for in-model type `Function`. */
extern zvalue TYPE_Function;

/** Type value for in-model type `Generic`. */
extern zvalue TYPE_Generic;

/** Type value for in-model type `Int`. */
extern zvalue TYPE_Int;

/** Type value for in-model type `String`. */
extern zvalue TYPE_String;

/** Type value for in-model type `Type`. */
extern zvalue TYPE_Type;

/**
 * Generic `call(value)`: Somewhat-degenerate generic for dispatching to
 * a function call mechanism (how meta). Only defined for types `Function`
 * and `Generic`. When called, argument count and pointer will have been
 * checked, but the argument count may not match what's expected by the
 * target function. The `state` argument is always passed as the function
 * or generic value itself.
 */
extern zvalue GFN_call;

/**
 * Generic `dataOf(value)`: Gets the data payload of a value of the given
 * type, if any. Defaults to returning `NULL` (void).
 */
extern zvalue GFN_dataOf;

/**
 * Generic `debugString(value)`: Returns a minimal string form of the
 * given value. Notably, functions and generics include their names.
 * The default implementation returns strings of the form
 * `#(TypeName @ address)`.
 */
extern zvalue GFN_debugString;

/**
 * Generic `gcMark(value)`: Does GC marking for the given value.
 */
extern zvalue GFN_gcMark;

/**
 * Generic `gcFree(value)`: Does GC freeing for the given value. This is
 * to do immediate pre-mortem freeing of value contents.
 */
extern zvalue GFN_gcFree;

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
 * Generic `sizeOf(value)`: Gets the "size" of a value of the given type,
 * for the appropriate per-type meaning of size. Defaults to always returning
 * `0`.
 */
extern zvalue GFN_sizeOf;


/*
 * Initialization Function
 */

/**
 * Initializes the `pb` module.
 */
void pbInit(void);


/*
 * Assertion Functions
 */

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a function. If not, this aborts the process
 * with a diagnostic message.
 */
void pbAssertFunction(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a generic function. If not, this aborts the process
 * with a diagnostic message.
 */
void pbAssertGeneric(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is an int. If not, this aborts the process
 * with a diagnostic message.
 */
void pbAssertInt(zvalue value);

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
 * Asserts that the given two values are valid `zvalue`s, and furthermore
 * that they have the same core type. If not, this aborts the process
 * with a diagnostic message.
 */
void pbAssertSameType(zvalue v1, zvalue v2);

/**
 * Asserts that the given range is valid for a `slice`-like operation
 * for a value of the given size.
 */
void pbAssertSliceRange(zint size, zint start, zint end);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string. If not, this aborts the process
 * with a diagnostic message.
 */
void pbAssertString(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
void pbAssertStringSize1(zvalue value);

/**
 * Asserts that the given value is of type `Type`. If not, this aborts the
 * process with a diagnostic message.
 */
void typeAssert(zvalue value);

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
 * Int Functions
 */

/**
 * Gets the `zchar` of the given int, asserting that the value
 * is in fact an int and in range for same.
 */
zchar zcharFromInt(zvalue intval);

/**
 * Gets an int value equal to the given `zint`. In this
 * implementation, ints are restricted to only taking on the range
 * of 32-bit signed quantities, when represented as twos-complement.
 */
zvalue intFromZint(zint value);

/**
 * Given an int, returns the `n`th bit, counting from the least
 * significant bit. `intval` must be an int. Returns `false` for a
 * `0` bit, and `true` for a `1` bit. If `n` references a bit beyond
 * the value's size, then the return value is the sign bit of the
 * value. It is an error if `n < 0`.
 */
bool intGetBit(zvalue intval, zint n);

/**
 * Gets a `zint` equal to the given int value. `intval` must be an
 * int. It is an error if the value is out of range.
 */
zint zintFromInt(zvalue intval);

/**
 * Given a 32-bit int value, returns the `n`th bit. This is just like
 * `intGetBit()` except using a `zint` value. This function is
 * exported for the convenience of other modules.
 */
bool zintGetBit(zint value, zint n);


/*
 * String functions.
 */

/**
 * Combines the characters of two strings, in order, into a new
 * string.
 */
zvalue stringAdd(zvalue str1, zvalue str2);

/**
 * Gets the string built from the given array of `zchar`s, of
 * the given size.
 */
zvalue stringFromZchars(zint size, const zchar *chars);

/**
 * Gets the string resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue stringFromUtf8(zint stringBytes, const char *string);

/**
 * Given a string, returns the `n`th element, which is in the
 * range of a 32-bit unsigned int. If `n` is out of range, this
 * returns `-1`.
 */
zint stringNth(zvalue string, zint n);

/**
 * Gets the string consisting of the given "slice" of elements
 * (start inclusive, end exclusive) of the given string.
 */
zvalue stringSlice(zvalue string, zint start, zint end);

/**
 * Encodes the given string as UTF-8 into the given buffer of the
 * given size in bytes. The buffer must be large enough to hold the entire
 * encoded result plus a terminating `'\0'` byte; if not, this function
 * will complain and exit the runtime. To be clear, the result *is*
 * `'\0'`-terminated.
 *
 * **Note:** If the given string possibly contains any `U+0` code points,
 * then the only "safe" way to use the result is as an explicitly-sized
 * buffer. (For example, `strlen()` might "lie".)
 */
void utf8FromString(zint resultSize, char *result, zvalue string);

/**
 * Gets the number of bytes required to encode the given string
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint utf8SizeFromString(zvalue string);

/**
 * Copies all the characters of the given string into the given result
 * array, which must be sized large enough to hold all of them.
 */
void zcharsFromString(zchar *result, zvalue string);


/*
 * Function definition and application
 */

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `argCount` must be non-negative.
 * If `argCount` is positive, then `args` must not be `NULL`.
 */
zvalue fnCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Constructs and returns a function with the given argument
 * restrictions, optional associated closure state, and optional name
 * (used when producing stack traces). `minArgs` must be non-negative,
 * and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue fnFrom(zint minArgs, zint maxArgs, zfunction function, zvalue state,
        zvalue name);


/*
 * Generic function definition
 */

/**
 * Adds a type-to-function binding to the given generic, for a core type.
 * `generic` must be a generic function, `type` must be a valid value of
 * type `Type`, and `function` must be a valid `zfunction`. The type must
 * not have already been bound in the given generic, and the generic must
 * not be sealed.
 */
void gfnBindCore(zvalue generic, zvalue type, zfunction function);

/**
 * Adds a default binding to the given generic. `generic` must be a generic
 * function, and `function` must be a valid `zfunction`. A default must not
 * have already been bound in the given generic, and the generic must not be
 * sealed.
 */
void gfnBindCoreDefault(zvalue generic, zfunction function);

/**
 * Constructs and returns a generic function with the given argument
 * restrictions and optional name (used when producing stack traces). It is
 * initially unsealed and without any bindings. `minArgs` must be at least
 * `1`, and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue gfnFrom(zint minArgs, zint maxArgs, zvalue name);

/**
 * Seal the given generic. This prevents it from gaining any new bindings.
 * `generic` must be a generic function.
 */
void gfnSeal(zvalue generic);


/*
 * Type Functions
 */

/**
 * Asserts that the given value is a valid `zvalue`, furthermore has the
 * given type. If not, this aborts the process with a diagnostic message.
 */
void pbAssertType(zvalue v1, zvalue type);

/**
 * Gets a type value whose name and optional secret are as given. The name
 * is an arbitrary value, used for ordering types and for displaying them
 * (e.g. for debugging purposes).
 *
 * If the secret is `NULL`, then the type is considered "transparent", meaning
 * that there are no restrictions on creation of values of the type, and the
 * data payload (if any) of a value of the type can be retrieved by arbitrary
 * code. Otherwise, the type is "opaque", and instances can only be created
 * by code possessing the secret, and payload data (or lack thereof) may only
 * be retrieved by code that possesses the secret.
 *
 * When called multiple times with identical arguments, this function returns
 * the same result. It is invalid (terminating the runtime) to attempt to
 * get a same-named type that differs only in the choice of secret.
 */
zvalue typeFrom(zvalue name, zvalue secret);

/**
 * Gets the name of the given type.
 */
zvalue typeName(zvalue type);

/**
 * Returns true iff the type of the given value (that is, `pbTypeOf(value)`)
 * is as given.
 */
bool pbTypeIs(zvalue value, zvalue type);

/**
 * Gets the overt data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
zvalue pbTypeOf(zvalue value);


/*
 * Derived Value Functions
 */

/**
 * Returns a derived value with optional data payload. The given `data`
 * value must either be a valid value or `NULL`.
 *
 * **Note:** If `type` and `data` are of the right form to be represented
 * as a core value, this function will *not* notice that. So only call it
 * if you know that the value to be produced is *necessarily* derived. If
 * it's possible that the arguments correspond to a core value, use
 * `constValueFrom` (in the `const` module) instead.
 */
zvalue derivFrom(zvalue type, zvalue data);


/*
 * Dispatched (type-based) Functions
 */

/**
 * Gets the data payload of the given value. `value` must be a
 * valid value (in particular, non-`NULL`). For everything but derived
 * values, the data payload is the same as the value itself. For derived
 * values, the data payload is (unsurprisingly) `NULL` for type-only
 * values.
 */
zvalue pbDataOf(zvalue value);

/**
 * Gets the "debug string" of the given value, as a `char *`. The caller
 * is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *pbDebugString(zvalue value);

/**
 * Compares two values for equality. This exists in addition to
 * `pbOrder`, because it is possible for this function run much
 * quicker in the not-equal case.
 */
bool pbEq(zvalue v1, zvalue v2);

/**
 * Like `pbEq`, but accepts `NULL` as a valid value.
 */
bool pbNullSafeEq(zvalue v1, zvalue v2);

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ ZLESS, ZSAME, ZMORE }`, less symbolically equal to `{
 * -1, 0, 1 }` respectively, with the usual comparison result meaning.
 * See `totalOrder` in the Samizdat Layer 0 spec for more details about
 * value sorting.
 */
zorder pbOrder(zvalue v1, zvalue v2);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * See the *Samizdat Layer 0* specification for details on
 * what low-layer "size" means.
 */
zint pbSize(zvalue value);


/*
 * Memory management functions
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
 * Gets a unique "order id" to use when comparing otherwise-incomparable
 * values of the same type, for use in defining the total order of values.
 */
zint pbOrderId(void);

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
void *pbPayload(zvalue value);

#endif
