/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Datatype API for low-level (C) code
 */

#ifndef SAM_DATA_H
#define SAM_DATA_H

#include <stdbool.h>
#include <stdint.h>


/*
 * C Types
 */

/**
 * 64-bit integer. This is the type used for all lowest-level integer
 * values.
 */
typedef int64_t zint;

/**
 * Unsigned byte. This is the type used for all lowest-level byte values
 * (including UTF-8 octets).
 */
typedef uint8_t zbyte;

/**
 * The result of a comparison.
 */
typedef enum {
    SAM_IS_LESS = -1,
    SAM_IS_EQUAL = 0,
    SAM_IS_MORE = 1
} zcomparison;

/**
 * Possible low-level data types. Note: The enum ordering is the same
 * as the type-based ordering for comparing values. See `samCompare()`.
 */
typedef enum {
    SAM_INTLET = 1,
    SAM_LISTLET,
    SAM_MAPLET,
    SAM_UNIQLET
} ztype;

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct SamValue *zvalue;

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
void samAssertValid(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and that its size
 * accommodates accessing the `n`th element. This includes asserting that
 * `n >= 0`. Note that all non-negative `n` are valid for accessing
 * intlets (their size notwithstanding).
 */
void samAssertNth(zvalue value, zint n);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is an intlet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertIntlet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a listlet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertListlet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a maplet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertMaplet(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a uniqlet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertUniqlet(zvalue value);

/**
 * Asserts that the given value is a valid listlet, and furthermore
 * that all elements in it are intlets of valid Unicode code points.
 */
void samAssertStringlet(zvalue value);

/**
 * Gets the low-level data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
ztype samType(zvalue value);

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
zint samSize(zvalue value);


/*
 * Intlet Functions
 */

/**
 * Given an intlet, returns the `n`th bit, counting from the least
 * significant bit. `value` must be an intlet. Returns `false` for a
 * `0` bit, and `true` for a `1` bit. If `n` references a bit beyond
 * the value's size, then the return value is the sign bit of the
 * value.
 */
bool samIntletGetBit(zvalue intlet, zint n);

/**
 * Given an intlet, returns the `n`th byte, counting from the least
 * significant byte. `value` must be an intlet. If `n` references a
 * byte beyond the value's size, then the return value is the sign byte
 * of the value.
 */
zint samIntletGetByte(zvalue intlet, zint n);

/**
 * Given an intlet, returns the `n`th `zint`-sized word, counting from
 * the least significant word. `value` must be an intlet. If `n`
 * references a word beyond the value's size, then the return value is
 * the sign word of the value.
 */
zint samIntletGetInt(zvalue intlet, zint n);

/**
 * Gets the sign of the given intlet. Returns `false` for
 * non-negative, and `true` for negative.
 */
bool samIntletSign(zvalue intlet);

/**
 * Gets an intlet value equal to the given `zint`.
 */
zvalue samIntletFromInt(zint value);

/**
 * Gets a `zint` equal to the given intlet value. It is an error if
 * the value is out of range.
 */
zint samIntletToInt(zvalue intlet);


/*
 * Listlet Functions
 */

/**
 * Given a listlet, returns the `n`th element. `value` must be a listlet, and
 * `n` must be `< samSize(value)`.
 */
zvalue samListletGet(zvalue listlet, zint n);

/**
 * Gets the value `@[]` (that is, the empty listlet).
 */
zvalue samListletEmpty(void);

/**
 * Gets the listlet resulting from appending the given value to the
 * given listlet.
 */
zvalue samListletAppend(zvalue listlet, zvalue value);


/*
 * Stringlet functions
 */

/**
 * Gets the stringlet (listlet Unicode-representing intlets) resulting
 * from interpreting the given UTF-8 encoded string, whose size in
 * bytes is as given.
 */
zvalue samStringletFromUtf8String(const zbyte *string, zint stringBytes);

/**
 * Gets the stringlet resulting from interpreting the ASCII C-style
 * (`'\0'`-terminated) string. It is an error if any byte value is
 * `> 0x7f`.
 */
zvalue samStringletFromAsciiString(const zbyte *string);


/*
 * Maplet Functions
 */

/**
 * Given a maplet, returns the `n`th mapping. `value` must be a maplet, and
 * `n` must be `< samSize(value)`.
 *
 * Note: When retrieved in ordinal order, keys are always returned in
 * sorted order.
 */
zmapping samMapletGet(zvalue maplet, zint n);

/**
 * Given a maplet, find the index of the given key. `value` must be a
 * maplet. Returns the index of the key or `~insertionIndex` (a
 * negative number) if not found.
 */
zint samMapletFind(zvalue maplet, zvalue key);

/**
 * Gets the value `@{}` (that is, the empty maplet).
 */
zvalue samMapletEmpty(void);

/**
 * Gets the maplet resulting from putting the given mapping into the
 * given maplet. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue samMapletPut(zvalue maplet, zvalue key, zvalue value);


/*
 * Uniqlet Functions
 */

/**
 * Gets a new uniqlet. Each call to this function is guaranteed to
 * produce a value unequal to any other call to this function (in any
 * given process).
 */
zvalue samUniqlet(void);


/*
 * Higher Level Functions
 */

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `{ SAM_IS_LESS, SAM_IS_EQUAL, SAM_IS_MORE }`, less
 * symbolically equal to `{ -1, 0, 1 }` respectively, with the usual
 * comparison result meaning.
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
zcomparison samCompare(zvalue v1, zvalue v2);


#endif
