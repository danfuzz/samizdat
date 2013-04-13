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
 * as the type-based ordering for comparing values. See `datCompare()`.
 */
typedef enum {
    DAT_INTLET = 1,
    DAT_LISTLET,
    DAT_MAPLET,
    DAT_UNIQLET
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
 * Asserts that the given value is a valid listlet, and furthermore
 * that all elements in it are intlets of valid Unicode code points.
 */
void datAssertStringlet(zvalue value);

/**
 * Gets the low-level data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
ztype datType(zvalue value);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * "Size" means:
 *
 * * an intlet's highest significant bit number plus one (may be
 *   rounded up to a word size).
 *
 * * a listlet's element count.
 *
 * * a maplet's mapping count.
 *
 * * `0` for all uniqlets.
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
 * Given an intlet, returns the `n`th byte, counting from the least
 * significant byte. `intlet` must be an intlet. If `n` references a
 * byte beyond the value's size, then the return value is the sign byte
 * of the value.
 */
zint datIntletGetByte(zvalue intlet, zint n);

/**
 * Given an intlet, returns the `n`th `zint`-sized word, counting from
 * the least significant word. `intlet` must be an intlet. If `n`
 * references a word beyond the value's size, then the return value is
 * the sign word of the value.
 */
zint datIntletGetInt(zvalue intlet, zint n);

/**
 * Gets the sign of the given intlet. `intlet` must be an
 * intlet. Returns `false` for non-negative, and `true` for negative.
 */
bool datIntletSign(zvalue intlet);

/**
 * Gets an intlet value equal to the given `zint`.
 */
zvalue datIntletFromInt(zint value);

/**
 * Gets a `zint` equal to the given intlet value. `intlet` must be an
 * intlet. It is an error if the value is out of range.
 */
zint datIntletToInt(zvalue intlet);


/*
 * Listlet Functions
 */

/**
 * Given a listlet, returns the `n`th element. `listlet` must be a
 * listlet, and `n` must be `< datSize(value)`.
 */
zvalue datListletGet(zvalue listlet, zint n);

/**
 * Gets the `zint` of the nth element of the given listlet. `listlet`
 * must be a listlet, `n` must be `< datSize(value)`, and the element
 * gotten must be an intlet in range to be represented as a `zint`.
 */
zint datListletGetInt(zvalue stringlet, zint n);

/**
 * Gets the value `@[]` (that is, the empty listlet).
 */
zvalue datListletEmpty(void);

/**
 * Gets the listlet resulting from appending the given value to the
 * given listlet.
 */
zvalue datListletAppend(zvalue listlet, zvalue value);

/**
 * Constructs a listlet from an array of `zvalue`s of the given size.
 */
zvalue datListletFromValues(zvalue *values, zint size);

/**
 * Gets the listlet resulting from deleting the nth element of the
 * given listlet.
 */
zvalue datListletDelete(zvalue listlet, zint n);


/*
 * Maplet Functions
 */

/**
 * Given a maplet, returns the `n`th mapping. `maplet` must be a
 * maplet, and `n` must be `< datSize(value)`.
 *
 * Note: When retrieved in ordinal order, keys are always returned in
 * sorted order.
 */
zmapping datMapletGetMapping(zvalue maplet, zint n);

/**
 * Given a maplet, find the mapping of the given key and return the
 * corresponding value, or `NULL` if there is no such key. `maplet`
 * must be a maplet, and `key` must be a valid value.
 */
zvalue datMapletGet(zvalue maplet, zvalue key);

/**
 * Given a maplet, find the index of the given key. `maplet` must be a
 * maplet. Returns the index of the key or `~insertionIndex` (a
 * negative number) if not found.
 */
zint datMapletFind(zvalue maplet, zvalue key);

/**
 * Gets the value `@{}` (that is, the empty maplet).
 */
zvalue datMapletEmpty(void);

/**
 * Gets the maplet resulting from putting the given mapping into the
 * given maplet. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue datMapletPut(zvalue maplet, zvalue key, zvalue value);


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
 * Stringlet functions. Stringlets are just listlets whose elements
 * are all intlets that represent Unicode code points.
 */

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
 * Higher Level Functions
 */

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ ZLESS, ZEQUAL, ZMORE }`, less symbolically equal to `{
 * -1, 0, 1 }` respectively, with the usual comparison result meaning.
 *
 * Major order is by type &mdash `intlet < listlet < maplet <
 * uniqlet` &mdash; and minor order is type-dependant.
 *
 * * Intlets order by integer value.
 *
 * * Listlets order by pairwise corresponding-element
 *   comparison, with a strict prefix always winning.
 *
 * * Maplets order by first comparing corresponding ordered lists
 *   of keys with the same rules as listlet comparison. If the key
 *   lists are identical, then the result is the comparison of
 *   corresponding lists of values, in key order.
 *
 * * Any given uniqlet never compares as equal to anything but itself.
 *   Any two uniqlets have a consistent and transitive &mdash; but
 *   otherwise arbitrary &mdash; comparison.
 */
zcomparison datCompare(zvalue v1, zvalue v2);

#endif
