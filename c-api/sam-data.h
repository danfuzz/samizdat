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
 * Possible low-level data types.
 */
typedef enum {
    SAM_INTLET = 1,
    SAM_LISTLET,
    SAM_MAPLET,
    SAM_UNIQUELET
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
 * furthermore that it is a uniquelet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertUniquelet(zvalue value);

/**
 * Gets the low-level data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
ztype samType(zvalue value);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * "Size" means:
 *
 * * an intlet's byte count
 * * a listlet's element count
 * * a maplet's mapping count
 * * `0` for all uniquelets
 */
zint samSize(zvalue value);


/*
 * Intlet Functions
 */

/**
 * Given an intlet, returns the `nth` byte. `value` must be an intlet, and
 * `n` must be `< samSize(value)`.
 */
zint samIntletGet(zvalue intlet, zint n);

/**
 * Gets an intlet value equal to the given `zint`.
 */
zvalue samIntletFromInt(zint value);

/**
 * Gets an intlet value equal to the first UTF-8 code point in the given
 * byte string. Updates the byte string pointer.
 */
zvalue samIntletFromUtf8(const zbyte **string);


/*
 * Listlet Functions
 */

/**
 * Given a listlet, returns the `nth` element. `value` must be a listlet, and
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

/**
 * Gets the listlet resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given.
 */
zvalue samListletFromUtf8(const zbyte *string, zint stringSize);


/*
 * Maplet Functions
 */

/**
 * Given a maplet, returns the `nth` mapping. `value` must be a maplet, and
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
zmapping samMapletFind(zvalue maplet, zvalue key);

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
 * Uniquelet Functions
 */

/**
 * Gets a new uniquelet. Each call to this function is guaranteed to
 * produce a value unequal to any other call to this function (in any
 * given process).
 */
zvalue samUniquelet(void);


/*
 * Higher Level Functions
 */

/**
 * Compares two values, providing a full ordering. Returns one of the
 * values `-1 0 1` with the usual comparison result meaning.
 *
 * Major order is by type &mdash `intlet < listlet < maplet <
 * uniquelet` &mdash; and minor order is type-dependant. Intlets order
 * by value. Listlets order by pairwise corresponding-element
 * comparison (with a strict prefix always winning). Maplets order by
 * pairwise corresponding-key comparison (prefixes as with listlets)
 * and then by pairwise corresponding-value comparison. Uniquelets
 * never compare as equal to anything but themselves and have a unique
 * and consistent, but arbitrary, comparison with other uniquelets.
 */
zcomparison samCompare(zvalue v1, zvalue v2);


#endif
