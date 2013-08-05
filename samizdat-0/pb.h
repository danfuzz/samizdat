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
typedef struct DatHeader *zvalue;

/**
 * Low-layer data type.
 */
typedef const struct DatType *ztype;

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

/**
 * Core type info.
 */
typedef struct DatType {
    /** Simple string name for the type. */
    const char *name;

    /**
     * In-model string value corresponding to `name` (above). Lazily
     * initialized.
     */
    zvalue nameValue;

    /**
     * Type sequence number, complemented to allow `0` to mean
     * "uninitialized".
     */
    zint seqNumCompl;
} DatType;


/*
 * Constants, type references and generic functions
 */

/** The standard value `""`. */
extern zvalue EMPTY_STRING;

/** The standard value `0`. */
extern zvalue DAT_0;

/** The standard value `1`. */
extern zvalue DAT_1;

/** The standard value `-1`. */
extern zvalue DAT_NEG1;

/** Type value for in-model type `Deriv`. */
extern ztype DAT_Deriv;

/** Type value for in-model type `Function`. */
extern ztype DAT_Function;

/** Type value for in-model type `Generic`. */
extern ztype DAT_Generic;

/** Type value for in-model type `Int`. */
extern ztype DAT_Int;

/** Type value for in-model type `String`. */
extern ztype DAT_String;

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
 * type, if any. Defaults to returning the value itself as its own payload.
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

/**
 * Generic `typeOf(value)`: Gets the (overt) type of a value of the given
 * type. Defaults to returning the low-layer type name.
 */
extern zvalue GFN_typeOf;


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
void datAssertFunction(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a generic function. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertGeneric(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is an int. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertInt(zvalue value);

/**
 * Asserts that the given size accommodates accessing the `n`th element.
 * This includes asserting that `n >= 0`. Note that all non-negative `n`
 * are valid for accessing ints (their size notwithstanding).
 */
void datAssertNth(zint size, zint n);

/**
 * Like `datAssertNth` but also accepts the case where `n` is the size
 * of the value.
 */
void datAssertNthOrSize(zint size, zint n);

/**
 * Asserts that the given two values are valid `zvalue`s, and furthermore
 * that they have the same core type. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertSameType(zvalue v1, zvalue v2);

/**
 * Asserts that the given range is valid for a `slice`-like operation
 * for a value of the given size.
 */
void datAssertSliceRange(zint size, zint start, zint end);

/**
 * Asserts that the given value is a valid `zvalue`, furthermore has the
 * given core type. If not, this aborts the process with a diagnostic message.
 */
void datAssertType(zvalue v1, ztype type);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertString(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
void datAssertStringSize1(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue` (non-`NULL` and
 * seems to actually have the right form). This performs reasonable,
 * but not exhaustive, tests. If not valid, this aborts the process
 * with a diagnostic message.
 */
void datAssertValid(zvalue value);


/*
 * Int Functions
 */

/**
 * Gets the `zchar` of the given int, asserting that the value
 * is in fact an int and in range for same.
 */
zchar datZcharFromInt(zvalue intval);

/**
 * Gets an int value equal to the given `zint`. In this
 * implementation, ints are restricted to only taking on the range
 * of 32-bit signed quantities, when represented as twos-complement.
 */
zvalue datIntFromZint(zint value);

/**
 * Given an int, returns the `n`th bit, counting from the least
 * significant bit. `intval` must be an int. Returns `false` for a
 * `0` bit, and `true` for a `1` bit. If `n` references a bit beyond
 * the value's size, then the return value is the sign bit of the
 * value. It is an error if `n < 0`.
 */
bool datIntGetBit(zvalue intval, zint n);

/**
 * Gets a `zint` equal to the given int value. `intval` must be an
 * int. It is an error if the value is out of range.
 */
zint datZintFromInt(zvalue intval);

/**
 * Given a 32-bit int value, returns the `n`th bit. This is just like
 * `datIntGetBit()` except using a `zint` value. This function is
 * exported for the convenience of other modules.
 */
bool datZintGetBit(zint value, zint n);


/*
 * String functions.
 */

/**
 * Combines the characters of two strings, in order, into a new
 * string.
 */
zvalue datStringAdd(zvalue str1, zvalue str2);

/**
 * Gets the string built from the given array of `zchar`s, of
 * the given size.
 */
zvalue datStringFromZchars(zint size, const zchar *chars);

/**
 * Gets the string resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue datStringFromUtf8(zint stringBytes, const char *string);

/**
 * Given a string, returns the `n`th element, which is in the
 * range of a 32-bit unsigned int. If `n` is out of range, this
 * returns `-1`.
 */
zint datStringNth(zvalue string, zint n);

/**
 * Gets the string consisting of the given "slice" of elements
 * (start inclusive, end exclusive) of the given string.
 */
zvalue datStringSlice(zvalue string, zint start, zint end);

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
void datUtf8FromString(zint resultSize, char *result, zvalue string);

/**
 * Gets the number of bytes required to encode the given string
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint datUtf8SizeFromString(zvalue string);

/**
 * Copies all the characters of the given string into the given result
 * array, which must be sized large enough to hold all of them.
 */
void datZcharsFromString(zchar *result, zvalue string);


/*
 * Function definition and application
 */

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `args` must be a list.
 */
zvalue datApply(zvalue function, zvalue args);

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `argCount` must be non-negative.
 * If `argCount` is positive, then `args` must not be `NULL`.
 */
zvalue datCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Constructs and returns a function with the given argument
 * restrictions, optional associated closure state, and optional name
 * (used when producing stack traces). `minArgs` must be non-negative,
 * and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue datFnFrom(zint minArgs, zint maxArgs, zfunction function, zvalue state,
        zvalue name);


/*
 * Generic function definition
 */

/**
 * Adds a type-to-function binding to the given generic, for a core type.
 * `generic` must be a generic function, `type` must be a valid core `ztype`,
 * and `function` must be a valid `zfunction`. The type must not have already
 * been bound in the given generic, and the generic must not be sealed.
 */
void datGfnBindCore(zvalue generic, ztype type, zfunction function);

/**
 * Adds a default binding to the given generic. `generic` must be a generic
 * function, and `function` must be a valid `zfunction`. A default must not
 * have already been bound in the given generic, and the generic must not be
 * sealed.
 */
void datGfnBindCoreDefault(zvalue generic, zfunction function);

/**
 * Constructs and returns a generic function with the given argument
 * restrictions and optional name (used when producing stack traces). It is
 * initially unsealed and without any bindings. `minArgs` must be at least
 * `1`, and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue datGfnFrom(zint minArgs, zint maxArgs, zvalue name);

/**
 * Seal the given generic. This prevents it from gaining any new bindings.
 * `generic` must be a generic function.
 */
void datGfnSeal(zvalue generic);


/*
 * Dispatched (type-based) Functions
 */

/**
 * Returns whether the given value has the given core (low-layer) type.
 * `value` must be a valid value (in particular, non-`NULL`).
 */
bool datCoreTypeIs(zvalue value, ztype type);

/**
 * Gets the data payload of the given value. `value` must be a
 * valid value (in particular, non-`NULL`). For everything but derived
 * values, the data payload is the same as the value itself. For derived
 * values, the data payload is (unsurprisingly) `NULL` for type-only
 * values.
 */
zvalue datDataOf(zvalue value);

/**
 * Gets the "debug string" of the given value, as a `char *`. The caller
 * is responsible for `free()`ing the result. As a convenience, this
 * converts `NULL` into `"(null)"`.
 */
char *datDebugString(zvalue value);

/**
 * Compares two values for equality. This exists in addition to
 * `datOrder`, because it is possible for this function run much
 * quicker in the not-equal case.
 */
bool datEq(zvalue v1, zvalue v2);

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ ZLESS, ZSAME, ZMORE }`, less symbolically equal to `{
 * -1, 0, 1 }` respectively, with the usual comparison result meaning.
 * See `totalOrder` in the Samizdat Layer 0 spec for more details about
 * value sorting.
 */
zorder datOrder(zvalue v1, zvalue v2);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * See the *Samizdat Layer 0* specification for details on
 * what low-layer "size" means.
 */
zint datSize(zvalue value);

/**
 * Returns true iff the type of the given value (that is, `datTypeOf(value)`)
 * is as given.
 */
bool datTypeIs(zvalue value, zvalue type);

/**
 * Gets the overt data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
zvalue datTypeOf(zvalue value);


/*
 * Memory management functions
 */

/**
 * Allocates memory, sized to include a `DatHeader` header plus the
 * indicated number of extra bytes. The `DatHeader` header is
 * initialized with the indicated type and size. The resulting value
 * is added to the live reference stack.
 */
zvalue datAllocValue(ztype type, zint extraBytes);

/**
 * Adds an item to the current frame. This is only necessary to call when
 * a reference gets "detached" from a live structure (via mutation), which
 * is to say, rarely.
 */
void datFrameAdd(zvalue value);

/**
 * Indicates the start of a new frame of references on the stack.
 * The return value can subsequently be passed to `datFrameEnd` to
 * indicate that this frame is no longer active.
 */
zstackPointer datFrameStart(void);

/**
 * Indicates that the frame whose start returned the given stack pointer
 * should be reset to a state that *only* includes the given value
 * (or is totally reset if `stackedValue` is `NULL`).
 */
void datFrameReset(zstackPointer savedStack, zvalue stackedValue);

/**
 * Indicates that the frame whose start returned the given stack pointer
 * is no longer active. If the given additional value is non-`NULL` it is
 * added to the frame being "returned" to. It is valid to return to any
 * frame above the current one, not just the immediately-previous frame;
 * non-immediate return can happen during a nonlocal exit.
 */
void datFrameReturn(zstackPointer savedStack, zvalue returnValue);

/**
 * Forces a gc.
 */
void datGc(void);

/**
 * Marks the given value as "immortal." It is considered a root and
 * will never get freed.
 */
void datImmortalize(zvalue value);

/**
 * Marks a value during garbage collection. This in turn calls a type-specific
 * mark function when appropriate and may recurse arbitrarily. It is valid
 * to pass `NULL` to this, but no other non-values are acceptable.
 */
void datMark(zvalue value);

/**
 * Gets a pointer to the data payload of a `zvalue`.
 */
void *datPayload(zvalue value);

#endif
