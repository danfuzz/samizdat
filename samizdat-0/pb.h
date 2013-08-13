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

/** Declaration for a method on the given type with the given name */
#define METH_IMPL(type, name) \
    static zvalue type##_##name(zint argCount, const zvalue *args)

/** Performs binding of the indicated method. */
#define METH_BIND(type, name) \
    do { genericBindCore(GFN_##name, TYPE_##type, type##_##name); } while(0)


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

/** Type value for in-model type `Uniqlet`. */
extern zvalue TYPE_Uniqlet;

/** Type value for in-model type `Value`. */
extern zvalue TYPE_Value;

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
 * Generic `canCall(function, value)`: See spec for details.
 */
extern zvalue GFN_canCall;

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
 * Gets the string resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue stringFromUtf8(zint stringBytes, const char *string);

/**
 * Converts a C `zchar` to an in-model single-character string.
 */
zvalue stringFromZchar(zchar value);

/**
 * Gets the string built from the given array of `zchar`s, of
 * the given size.
 */
zvalue stringFromZchars(zint size, const zchar *chars);

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
 * a callable value (regular or generic function, or other type which binds
 * the `call` generic function), and `argCount` must be non-negative.
 * If `argCount` is positive, then `args` must not be `NULL`.
 *
 * **Note:** The `fun` prefix is used to denote functions which operate
 * on all sorts of callable function-like things.
 */
zvalue funCall(zvalue function, zint argCount, const zvalue *args);

/**
 * Constructs and returns a function with the given argument
 * restrictions, optional associated closure state, and optional name
 * (used when producing stack traces). `minArgs` must be non-negative,
 * and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue functionFrom(zint minArgs, zint maxArgs, zfunction function,
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
void genericBindCore(zvalue generic, zvalue type, zfunction function);

/**
 * Constructs and returns a generic function with the given argument
 * restrictions and optional name (used when producing stack traces). It is
 * initially unsealed and without any bindings. `minArgs` must be at least
 * `1`, and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue genericFrom(zint minArgs, zint maxArgs, zvalue name);

/**
 * Seal the given generic. This prevents it from gaining any new bindings.
 * `generic` must be a generic function.
 */
void genericSeal(zvalue generic);


/*
 * Type Functions
 */

/**
 * Asserts that the given value is a valid `zvalue`, furthermore has the
 * given type. If not, this aborts the process with a diagnostic message.
 * If given a non-`Type` value for `type`, this takes it to name a
 * transparent derived type.
 */
void assertHasType(zvalue value, zvalue type);

/**
 * Asserts that the given two values are valid `zvalue`s, and furthermore
 * that they have the same type. If not, this aborts the process
 * with a diagnostic message.
 */
void assertHaveSameType(zvalue v1, zvalue v2);

/**
 * Gets a new core type, given its name. When given the same name twice, this
 * returns identical results. `identified` indicates whether the type should
 * be considered "identified". Values of an "identified" type have unique
 * identity values which can be retrieved using `identityOf`. These values
 * are automatically used when comparing values of the same type.
 */
zvalue coreTypeFromName(zvalue name, bool identified);

/**
 * Returns true iff the type of the given value (that is, `typeOf(value)`)
 * is as given.
 */
bool hasType(zvalue value, zvalue type);

/**
 * Returns true iff the types of the given values (that is, `typeOf()` on
 * each) are the same.
 */
bool haveSameType(zvalue v1, zvalue v2);

/**
 * Returns true iff the given type is "identified". That is, this returns
 * true if values of the type can be fruitfully used as the argument
 * to `identityOf`.
 */
bool typeIsIdentified(zvalue type);

/**
 * Gets the name of the given type. If given a non-`Type` value for `type`,
 * this takes it to name a transparent derived type; as such it will return
 * `type` itself in these cases.
 */
zvalue typeName(zvalue type);

/**
 * Gets the overt data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`). For transparent derived types,
 * this returns the name of the type, and not a `Type` value per se.
 */
zvalue typeOf(zvalue value);

/**
 * Gets the parent type of the given type. If given a non-`Type` value for
 * `type`, this takes it to name a transparent derived type; as such it
 * will return `TYPE_Value` in these cases.
 */
zvalue typeParent(zvalue type);


/*
 * Uniqlet Functions
 */

/**
 * Gets a new uniqlet. Each call to this function is guaranteed to
 * produce a value unequal to any other uniqlet (in any given process).
 */
zvalue uniqlet(void);


/*
 * General Value Functions
 */

/**
 * Gets the data payload of the given value, if possible. `value` must be a
 * valid value (in particular, non-`NULL`). This is a convenient shorthand
 * for calling `dataFromValue(value, NULL)`.
 For everything but derived
 * values, the data payload is the same as the value itself. For derived
 * values, the data payload is (unsurprisingly) `NULL` for type-only
 * values.
 */
zvalue dataOf(zvalue value);

/**
 * Gets a unique identity number associated with this value. Only works
 * on values of an opaque type, and only if the type is marked as
 * `identified`.
 */
zint identityOf(zvalue value);

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
 */
zvalue dataFromValue(zvalue value, zvalue secret);

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
zvalue derivFrom(zvalue type, zvalue data, zvalue secret);

/**
 * Returns a transparent derived value with the given type tag, and with the
 * given optional data payload. This is a convenient shorthand for calling
 * `derivFrom(type, data, NULL)`.
 */
zvalue valueFrom(zvalue type, zvalue data);


/*
 * Dispatched (type-based) Functions
 */

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
 * Gets a pointer to the data payload of a `zvalue`.
 */
inline void *pbPayload(zvalue value) {
    return (void *) (((char *) value) + PB_HEADER_SIZE);
}

#endif
