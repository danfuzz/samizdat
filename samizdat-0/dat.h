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
 * Possible low-level data types.
 *
 * **Note:** The enum ordering is the same as the type-based ordering for
 * comparing values. See `datOrder()`.
 */
typedef enum {
    DAT_INTLET = 1,
    DAT_STRINGLET,
    DAT_LISTLET,
    DAT_MAPLET,
    DAT_UNIQLET,
    DAT_HIGHLET
} ztype;

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct DatValue *zvalue;

/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    zvalue key;
    zvalue value;
} zmapping;

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


/*
 * Basic Functions
 */

/**
 * Asserts that the given value is a valid `zvalue` (non-`NULL` and
 * seems to actually have the right form). This performs reasonable,
 * but not exhaustive, tests. If not valid, this aborts the process
 * with a diagnostic message.
 */
void datAssertValid(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and that its size
 * accommodates accessing the `n`th element. This includes asserting that
 * `n >= 0`. Note that all non-negative `n` are valid for accessing
 * integers (their size notwithstanding).
 */
void datAssertNth(zvalue value, zint n);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is an integer. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertInteger(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertString(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a listlet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertListlet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a maplet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertMaplet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a uniqlet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertUniqlet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and furthermore
 * that it is a highlet (a high-layer value). If not, this aborts the
 * process with a diagnostic message.
 */
void datAssertHighlet(zvalue value);

/**
 * Returns whether the given value has the given low-layer type.
 * `value` must be a valid value (in particular, non-`NULL`).
 */
bool datTypeIs(zvalue value, ztype type);

/**
 * Gets the low-level data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
ztype datType(zvalue value);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * See the *Samizdat Layer 0* specification for details on
 * what low-layer "size" means.
 */
zint datSize(zvalue value);


/*
 * Integer Functions
 */

/**
 * Gets the `zchar` of the given integer, asserting that the value
 * is in fact an integer and in range for same.
 */
zchar datCharFromInteger(zvalue integer);

/**
 * Given a 32-bit int value, returns the `n`th bit. This is just like
 * `datIntegerGetBit()` except using a `zint` value. This function is
 * exported for the convenience of other modules.
 */
bool datIntGetBit(zint value, zint n);

/**
 * Given an integer, returns the `n`th bit, counting from the least
 * significant bit. `integer` must be an integer. Returns `false` for a
 * `0` bit, and `true` for a `1` bit. If `n` references a bit beyond
 * the value's size, then the return value is the sign bit of the
 * value. It is an error if `n < 0`.
 */
bool datIntegerGetBit(zvalue integer, zint n);

/**
 * Gets the sign of the given integer. `integer` must be an
 * integer. Returns `false` for non-negative, and `true` for negative.
 */
bool datIntegerSign(zvalue integer);

/**
 * Gets an integer value equal to the given `zint`. In this
 * implementation, integers are restricted to only taking on the range
 * of 32-bit signed quantities, when represented as twos-complement.
 */
zvalue datIntegerFromInt(zint value);

/**
 * Gets a `zint` equal to the given integer value. `integer` must be an
 * integer. It is an error if the value is out of range.
 */
zint datIntFromInteger(zvalue integer);


/*
 * String functions.
 */

/**
 * Given a string, returns the `n`th element, which is in the
 * range of a 32-bit unsigned int. If `n` is out of range, this
 * returns `-1`.
 */
zint datStringNth(zvalue string, zint n);

/**
 * Gets the string built from the given array of `zchar`s, of
 * the given size.
 */
zvalue datStringFromChars(zint size, const zchar *chars);

/**
 * Combines the characters of two strings, in order, into a new
 * string. See `datListletAdd()` for discussion about the choice
 * of name.
 */
zvalue datStringAdd(zvalue str1, zvalue str2);

/**
 * Gets the string resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue datStringFromUtf8(zint stringBytes, const char *string);

/**
 * Gets the number of bytes required to encode the given string
 * as UTF-8. The result does *not* account for a terminating `'\0'` byte.
 */
zint datUtf8SizeFromString(zvalue string);

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


/*
 * Listlet Functions
 */

/**
 * Gets the listlet resulting from inserting the given value at the
 * given index in the given listlet. `n` must be non-negative and no
 * greater than the size of the given listlet.
 */
zvalue datListletInsNth(zvalue listlet, zint n, zvalue value);

/**
 * Given a listlet, returns the `n`th element. `listlet` must be a
 * listlet, and `n` must be `< datSize(value)`.
 */
zvalue datListletNth(zvalue listlet, zint n);

/**
 * Gets the listlet resulting from appending the given value to the
 * given listlet.
 */
zvalue datListletAppend(zvalue listlet, zvalue value);

/**
 * Gets the listlet resulting from setting the value at the
 * given index to the given value. `n` must be non-negative
 * and no greater than the size of the given listlet.
 */
zvalue datListletPutNth(zvalue listlet, zint n, zvalue value);

/**
 * Combines the elements of two listlets, in order, into a new
 * listlet.
 *
 * Contrasting this with `datListlet{Append,Prepend}()` and above,
 * those functions operate
 * heterogeneously on a listlet and an element, whereas this one
 * operates on two peer listlets.
 */
zvalue datListletAdd(zvalue listlet1, zvalue listlet2);

/**
 * Gets the listlet resulting from deleting the nth element of the
 * given listlet.
 */
zvalue datListletDelNth(zvalue listlet, zint n);

/**
 * Constructs a listlet from an array of `zvalue`s of the given size.
 */
zvalue datListletFromArray(zint size, const zvalue *values);

/**
 * Copies all the values of the given listlet into the given result
 * array, which must be sized large enough to hold all of them.
 */
void datArrayFromListlet(zvalue *result, zvalue listlet);


/*
 * Maplet Functions
 */

/**
 * Gets an empty maplet value, i.e. `@[=]`. Note that this can return
 * a different allocated value every time.
 */
zvalue datMapletEmpty(void);

/**
 * Given a maplet, find the mapping of the given key and return the
 * corresponding value, or `NULL` if there is no such key. `maplet`
 * must be a maplet, and `key` must be a valid value.
 */
zvalue datMapletGet(zvalue maplet, zvalue key);

/**
 * Given a maplet, returns the `n`th mapping. `maplet` must be a
 * maplet, and `n` must be `< datSize(value)`. Mappings are returned
 * as single-element maplets. Maplet ordering is by key.
 */
zvalue datMapletNth(zvalue maplet, zint n);

/**
 * Given a maplet, returns the `n`th key. `maplet` must be a
 * maplet, and `n` must be `< datSize(value)`. Maplet ordering is by key.
 */
zvalue datMapletNthKey(zvalue maplet, zint n);

/**
 * Given a maplet, returns the `n`th value. `maplet` must be a
 * maplet, and `n` must be `< datSize(value)`. Maplet ordering is by key.
 */
zvalue datMapletNthValue(zvalue maplet, zint n);

/**
 * Gets the maplet resulting from putting the given mapping into the
 * given maplet. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue datMapletPut(zvalue maplet, zvalue key, zvalue value);

/**
 * Gets the maplet resulting from putting the all the given mappings
 * into the given maplet, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 * The effect is identical to calling a chain of `datMapletPut()`s on each
 * of the mappings in order.
 */
zvalue datMapletAddArray(zvalue maplet, zint size, const zmapping *mappings);

/**
 * Combines the bindings of the two given maplets into a new maplet.
 * For overlapping keys between the two, the second argument "wins".
 */
zvalue datMapletAdd(zvalue maplet1, zvalue maplet2);

/**
 * Gets a maplet resulting from the removal of the given key from the
 * given maplet.
 */
zvalue datMapletDel(zvalue maplet, zvalue key);


/*
 * Uniqlet Functions
 */

/**
 * Gets a new uniqlet. Each call to this function is guaranteed to
 * produce a value unequal to any other uniqlet (in any given process).
 */
zvalue datUniqlet(void);

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

/**
 * Gets whether or not the given uniqlet has the given dispatch table.
 */
bool datUniqletHasDispatch(zvalue uniqlet, DatUniqletDispatch *dispatch);

/**
 * Gets the state value associated with the given uniqlet, asserting that
 * the uniqlet's dispatch table is as given.
 */
void *datUniqletGetState(zvalue uniqlet, DatUniqletDispatch *dispatch);


/*
 * Highlet Functions
 */

/**
 * Gets the type tag of a highlet.
 */
zvalue datHighletType(zvalue highlet);

/**
 * Gets the value associated with a highlet. This is `NULL` for
 * valueless highlets (unsurprisingly).
 */
zvalue datHighletValue(zvalue highlet);

/**
 * Returns a possibly-valued highlet. The given value must either
 * be a valid value or `NULL`.
 */
zvalue datHighletFrom(zvalue type, zvalue value);

/**
 * Returns whether or not the type of the given highlet equals the
 * given value.
 */
bool datHighletTypeIs(zvalue highlet, zvalue type);

/**
 * Asserts that the given value is a highlet whose type is as given.
 */
void datHighletAssertType(zvalue highlet, zvalue type);


/*
 * Higher Level Functions
 */

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
 * See `lowOrder` in the Samizdat Layer 0 spec for more details about
 * value sorting.
 */
zorder datOrder(zvalue v1, zvalue v2);


/*
 * Memory management functions
 */

/**
 * Marks a value during garbage collection. This in turn calls a type-specific
 * mark function when appropriate and may recurse arbitrarily. It is valid
 * to pass `NULL` to this, but no other non-values are acceptable.
 */
void datMark(zvalue value);

/**
 * Sets the base of the stack. This has to be called from a function
 * which (a) performs no other dat module calls, and (b) is the ancestor
 * call of all functions which *do* make dat module calls. Pass it the
 * address of a local variable.
 */
void datSetStackBase(void *base);

/**
 * Marks the given value as "immortal." It is considered a root and
 * will never get freed.
 */
void datImmortalize(zvalue value);

/**
 * Forces a gc.
 */
void datGc(void);

#endif
