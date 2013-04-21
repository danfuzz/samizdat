/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
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
 * Possible low-level data types. Note: The enum ordering is the same
 * as the type-based ordering for comparing values. See `datOrder()`.
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
 * intlets (their size notwithstanding).
 */
void datAssertNth(zvalue value, zint n);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is an intlet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertIntlet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a stringlet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertStringlet(zvalue value);

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
 * Intlet Functions
 */

/**
 * Given an intlet, returns the `n`th bit, counting from the least
 * significant bit. `intlet` must be an intlet. Returns `false` for a
 * `0` bit, and `true` for a `1` bit. If `n` references a bit beyond
 * the value's size, then the return value is the sign bit of the
 * value.
 */
bool datIntletGetBit(zvalue intlet, zint n);

/**
 * Gets the sign of the given intlet. `intlet` must be an
 * intlet. Returns `false` for non-negative, and `true` for negative.
 */
bool datIntletSign(zvalue intlet);

/**
 * Gets an intlet value equal to the given `zint`. In this
 * implementation, intlets are restricted to only taking on the range
 * of 32-bit signed quantities, when represented as twos-complement.
 */
zvalue datIntletFromInt(zint value);

/**
 * Gets a `zint` equal to the given intlet value. `intlet` must be an
 * intlet. It is an error if the value is out of range.
 */
zint datIntFromIntlet(zvalue intlet);


/*
 * Stringlet functions.
 */

/**
 * Given a stringlet, returns the `n`th element. `stringlet` must be a
 * stringlet, and `n` must be `< datSize(value)`.
 */
zchar datStringletGet(zvalue stringlet, zint n);

/**
 * Gets the stringlet built from the given array of `zchar`s, of
 * the given size.
 */
zvalue datStringletFromChars(const zchar *chars, zint size);

/**
 * Gets the stringlet built from the given character-representing
 * intlet.
 */
zvalue datStringletFromIntlet(zvalue intlet);

/**
 * Gets the stringlet built from the given listlet of intlets.
 */
zvalue datStringletFromListlet(zvalue listlet);

/**
 * Gets the list of characters in the given stringlet, in order,
 * as a listlet of intlets.
 */
zvalue datStringletToListlet(zvalue stringlet);

/**
 * Combines the characters of two stringlets, in order, into a new
 * stringlet. See `datListletAdd()` for discussion about the choice
 * of name.
 */
zvalue datStringletAdd(zvalue str1, zvalue str2);

/**
 * Gets the stringlet resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given. If `stringBytes`
 * is passed as `-1`, this uses `strlen()` to determine size.
 */
zvalue datStringletFromUtf8String(const char *string, zint stringBytes);

/**
 * Encodes the given stringlet as UTF-8, returning permanently
 * allocated storage for the result, and storing the size in bytes via
 * the given `resultSize` pointer if non-`NULL`. The result *is*
 * `'\0'`-terminated, but `*resultSize` will need to be used if the
 * original stringlet might have contained any `U+0` code points.
 */
const char *datStringletEncodeUtf8(zvalue stringlet, zint *resultSize);


/*
 * Listlet Functions
 */

/**
 * Gets the value `@[]` (that is, the empty listlet).
 */
zvalue datListletEmpty(void);

/**
 * Given a listlet, returns the `n`th element. `listlet` must be a
 * listlet, and `n` must be `< datSize(value)`.
 */
zvalue datListletGet(zvalue listlet, zint n);

/**
 * Gets the listlet resulting from appending the given value to the
 * given listlet.
 */
zvalue datListletAppend(zvalue listlet, zvalue value);

/**
 * Gets the listlet resulting from prepending the given value to the
 * front of the given listlet. Note that the order of arguments is in
 * parallel to what the result will be.
 */
zvalue datListletPrepend(zvalue value, zvalue listlet);

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
zvalue datListletDelete(zvalue listlet, zint n);

/**
 * Constructs a listlet from an array of `zvalue`s of the given size.
 */
zvalue datListletFromValues(zint size, const zvalue *values);

/**
 * Copies all the values of the given listlet into the given output
 * array, which must be sized large enough to hold all of them.
 */
void datListletToValues(zvalue listlet, zvalue *values);


/*
 * Maplet Functions
 */

/**
 * Gets the value `@{}` (that is, the empty maplet).
 */
zvalue datMapletEmpty(void);

/**
 * Gets a listlet of all the keys of the given maplet.
 */
zvalue datMapletKeys(zvalue maplet);

/**
 * Given a maplet, find the mapping of the given key and return the
 * corresponding value, or `NULL` if there is no such key. `maplet`
 * must be a maplet, and `key` must be a valid value.
 */
zvalue datMapletGet(zvalue maplet, zvalue key);

/**
 * Gets the maplet resulting from putting the given mapping into the
 * given maplet. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue datMapletPut(zvalue maplet, zvalue key, zvalue value);

/**
 * Combines the bindings of the two given maplets into a new maplet.
 * For overlapping keys between the two, the second argument "wins".
 */
zvalue datMapletAdd(zvalue maplet1, zvalue maplet2);

/**
 * Gets a maplet resulting from the removal of the given key from the
 * given maplet. It is an error if the key isn't in the maplet.
 */
zvalue datMapletDelete(zvalue maplet, zvalue key);


/*
 * Uniqlet Functions
 */

/**
 * Gets a new uniqlet. Each call to this function is guaranteed to
 * produce a value unequal to any other uniqlet (in any given process).
 */
zvalue datUniqlet(void);

/**
 * Gets a new uniqlet, associated with the given key and contents.
 * The uniqueness guarantee is the same as with `datUniqlet()`.
 *
 * In addition, this provides a convenient way to effectively build an
 * identity mapping between uniqlets (as the keys) and arbitrary
 * non-dat-module data (as the values). Rather than store uniqlets
 * as keys in an external structure, this inverts the relationship,
 * storing the `key` (that represents the map) and associated `value`
 * inside the uniqlet. In object-capability terms, the `key` is a
 * sealer/unsealer, and the uniqlet serves secondary duty as a sealed
 * box.
 */
zvalue datUniqletWith(void *key, void *value);

/**
 * Gets whether or not the given uniqlet has the given key. The `key`
 * must not be `NULL`.
 */
bool datUniqletHasKey(zvalue uniqlet, void *key);

/**
 * Gets the value associated with the given uniqlet, asserting that
 * the uniqlet's key is as given.
 */
void *datUniqletGetValue(zvalue uniqlet, void *key);

/**
 * Sets the value associated with the given uniqlet as given,
 * asserting that the uniqlet's key is as given.
 */
void datUniqletSetValue(zvalue uniqlet, void *key, void *value);


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
bool datHighletHasType(zvalue highlet, zvalue type);

/**
 * Asserts that the given value is a highlet whose type is as given.
 */
void datHighletAssertType(zvalue highlet, zvalue type);


/*
 * Higher Level Functions
 */

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ ZLESS, ZSAME, ZMORE }`, less symbolically equal to `{
 * -1, 0, 1 }` respectively, with the usual comparison result meaning.
 * See `lowOrder` in the Samizdat Layer 0 spec for more details about
 * value sorting.
 */
zorder datOrder(zvalue v1, zvalue v2);

#endif
