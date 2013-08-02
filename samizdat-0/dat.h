/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Low-layer data structures
 */

#ifndef _DAT_H_
#define _DAT_H_

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

/**
 * Prototype for state access/manipulation functions. These are called on
 * uniqlet state pointers, and on other non-value pointers that those may in
 * turn point to.
 */
typedef void (*zstateFunction)(void *state);

/**
 * Dispatch table for uniqlets with state.
 */
typedef struct {
    /** Function to call in order to do a gc mark on the uniqlet state. */
    zstateFunction mark;

    /** Function to call in order to free the uniqlet state's resources. */
    zstateFunction free;
} DatUniqletDispatch;

/** Type for local value stack pointers. */
typedef const zvalue *zstackPointer;


/*
 * Constants, type references and generic functions
 */

/** The standard value `[]` */
extern zvalue EMPTY_LIST;

/** The standard value `[:]` */
extern zvalue EMPTY_MAP;

/** The standard value `""` */
extern zvalue EMPTY_STRING;

/** Type value for in-model type `Deriv`. */
extern ztype DAT_Deriv;

/** Type value for in-model type `Function`. */
extern ztype DAT_Function;

/** Type value for in-model type `Generic`. */
extern ztype DAT_Generic;

/** Type value for in-model type `Int`. */
extern ztype DAT_Int;

/** Type value for in-model type `List`. */
extern ztype DAT_List;

/** Type value for in-model type `Map`. */
extern ztype DAT_Map;

/** Type value for in-model type `String`. */
extern ztype DAT_String;

/** Type value for in-model type `Uniqlet`. */
extern ztype DAT_Uniqlet;

/**
 * Generic `dataOf(value)`: Gets the data payload of a value of the given
 * type, if any. Defaults to returning the value itself as its own payload.
 */
extern zvalue genDataOf;

/**
 * Generic `sizeOf(value)`: Gets the "size" of a value of the given type,
 * for the appropriate per-type meaning of size. Defaults to always returning
 * `0`.
 */
extern zvalue genSizeOf;

/**
 * Generic `typeOf(value)`: Gets the (overt) type of a value of the given
 * type. Defaults to returning the low-layer type name.
 */
extern zvalue genTypeOf;


/*
 * Initialization Function
 */

/**
 * Initializes the `dat` module.
 */
void datInit(void);


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
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a list. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertList(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertMap(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
void datAssertMapSize1(zvalue value);

/**
 * Asserts that the given two values are valid `zvalue`s, and furthermore
 * that they have the same core type. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertSameType(zvalue v1, zvalue v2);

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
 * Asserts that the given value is a valid `zvalue`, and furthermore
 * that it is a derived value. If not, this aborts the process with a
 * diagnostic message.
 */
void datAssertDeriv(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a uniqlet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertUniqlet(zvalue value);


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
 * string. See `datListAdd()` for discussion about the choice
 * of name.
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
 * List Functions
 */

/**
 * Copies all the values of the given list into the given result
 * array, which must be sized large enough to hold all of them.
 */
void datArrayFromList(zvalue *result, zvalue list);

/**
 * Combines the elements of two lists, in order, into a new
 * list.
 *
 * Contrasting this with `datList{Append,Prepend}()` and above,
 * those functions operate
 * heterogeneously on a list and an element, whereas this one
 * operates on two peer lists.
 */
zvalue datListAdd(zvalue list1, zvalue list2);

/**
 * Gets the list resulting from appending the given value to the
 * given list.
 */
zvalue datListAppend(zvalue list, zvalue value);

/**
 * Gets the list resulting from deleting the nth element of the
 * given list.
 */
zvalue datListDelNth(zvalue list, zint n);

/**
 * Constructs a list from an array of `zvalue`s of the given size.
 */
zvalue datListFromArray(zint size, const zvalue *values);

/**
 * Gets the list resulting from inserting the given value at the
 * given index in the given list. `n` must be non-negative and no
 * greater than the size of the given list.
 */
zvalue datListInsNth(zvalue list, zint n, zvalue value);

/**
 * Given a list, returns the `n`th element. `list` must be a
 * list, and `n` must be `< datSize(value)`.
 */
zvalue datListNth(zvalue list, zint n);

/**
 * Gets the list resulting from setting the value at the
 * given index to the given value. `n` must be non-negative
 * and no greater than the size of the given list.
 */
zvalue datListPutNth(zvalue list, zint n, zvalue value);

/**
 * Gets the list consisting of the given "slice" of elements
 * (start inclusive, end exclusive) of the given list.
 */
zvalue datListSlice(zvalue list, zint start, zint end);


/*
 * Map Functions
 */

/**
 * Copies all the mappings of the given map into the given result
 * array, which must be sized large enough to hold all of them.
 */
void datArrayFromMap(zmapping *result, zvalue map);

/**
 * Gets the map resulting from putting the all the given mappings
 * into the given map, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 * The effect is identical to calling a chain of `datMapPut()`s on each
 * of the mappings in order.
 */
zvalue datMapAddArray(zvalue map, zint size, const zmapping *mappings);

/**
 * Combines the mappings of the two given maps into a new map.
 * For overlapping keys between the two, the second argument "wins".
 */
zvalue datMapAdd(zvalue map1, zvalue map2);

/**
 * Gets a map resulting from the removal of the given key from the
 * given map.
 */
zvalue datMapDel(zvalue map, zvalue key);

/**
 * Given a map, find the mapping of the given key and return the
 * corresponding value, or `NULL` if there is no such key. `map`
 * must be a map, and `key` must be a valid value.
 */
zvalue datMapGet(zvalue map, zvalue key);

/**
 * Given a map, returns the `n`th mapping. `map` must be a
 * map, and `n` must be `< datSize(value)`. Mappings are returned
 * as single-element maps. Map ordering is by key.
 */
zvalue datMapNth(zvalue map, zint n);

/**
 * Gets the map resulting from putting the given mapping into the
 * given map. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue datMapPut(zvalue map, zvalue key, zvalue value);

/**
 * Constructs and returns a single-mapping map.
 */
zvalue datMapping(zvalue key, zvalue value);

/**
 * Given a single-mapping map, returns its sole key. `map` must be a
 * map.
 */
zvalue datMappingKey(zvalue map);

/**
 * Given a single-mapping map, returns its sole value. `map` must be a
 * map.
 */
zvalue datMappingValue(zvalue map);


/*
 * Uniqlet Functions
 */

/**
 * Gets a new uniqlet. Each call to this function is guaranteed to
 * produce a value unequal to any other uniqlet (in any given process).
 */
zvalue datUniqlet(void);

/**
 * Gets the state value associated with the given uniqlet, asserting that
 * the uniqlet's dispatch table is as given.
 */
void *datUniqletGetState(zvalue uniqlet, DatUniqletDispatch *dispatch);

/**
 * Gets whether or not the given uniqlet has the given dispatch table.
 */
bool datUniqletHasDispatch(zvalue uniqlet, DatUniqletDispatch *dispatch);

/**
 * Gets a new uniqlet, associated with the given dispatcher and contents.
 * The uniqueness guarantee is the same as with `datUniqlet()`.
 *
 * In addition, this provides a convenient way to effectively build an
 * identity mapping between uniqlets (as the keys) and arbitrary
 * non-dat-module data (as the state values). Rather than store uniqlets
 * as keys in an external structure, this inverts the relationship,
 * storing the key (that represents the map) and associated `state`
 * inside the uniqlet. In object-capability terms, the key is a
 * sealer/unsealer, and the uniqlet serves secondary duty as a sealed
 * box. In this case, the key is in fact the associated state dispatch table.
 */
zvalue datUniqletWith(DatUniqletDispatch *dispatch, void *state);


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
 * `function` must be a valid `zfunction`, and `state` is arbitrary state
 * to be passed to `function` when called. The type must not have already
 * been bound in the given generic, and the generic must not be sealed.
 */
void datGenBindCore(zvalue generic, ztype type,
        zfunction function, zvalue state);

/**
 * Adds a default binding to the given generic. `generic` must be a generic
 * function, `function` must be a valid `zfunction`, and `state` is arbitrary
 * state to be passed to `function` when called. A default must not have
 * already been bound in the given generic, and the generic must not be
 * sealed.
 */
void datGenBindCoreDefault(zvalue generic, zfunction function, zvalue state);

/**
 * Constructs and returns a generic function with the given argument
 * restrictions and optional name (used when producing stack traces). It is
 * initially unsealed and without any bindings. `minArgs` must be at least
 * `1`, and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue datGenFrom(zint minArgs, zint maxArgs, zvalue name);

/**
 * Seal the given generic. This prevents it from gaining any new bindings.
 * `generic` must be a generic function.
 */
void datGenSeal(zvalue generic);


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
zvalue datDerivFrom(zvalue type, zvalue data);


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

#endif
