/*
 * Samizdat. Copyright Dan Bornstein.
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
typedef int64_t sam_int;

/**
 * Unsigned byte. This is the type used for all lowest-level byte values
 * (including UTF-8 octets).
 */
typedef uint8_t sam_byte;

/**
 * The result of a comparison.
 */
typedef enum {
    SAM_IS_LESS = -1,
    SAM_IS_EQUAL = 0,
    SAM_IS_MORE = 1
} sam_comparison;

/**
 * Possible low-level data types.
 */
typedef enum {
    SAM_INTLET = 1,
    SAM_LISTLET,
    SAM_MAPLET,
    SAM_UNIQUELET
} sam_type;

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct sam_value sam_value;

/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    sam_value key;
    sam_value value;
} sam_mapping;


/*
 * Basic Accessors
 */

/**
 * Asserts that the given value is a valid `sam_value` (non-`NULL` and
 * seems to actually have the right form). This performs reasonable,
 * but not exhaustive, tests. If not valid, this aborts the process
 * with a diagnostic message.
 */
void samAssertValid(sam_value value);

/**
 * Asserts that the given value is a valid `sam_value`, and
 * furthermore that it is an intlet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertIntlet(sam_value value);

/**
 * Asserts that the given value is a valid `sam_value`, and
 * furthermore that it is a listlet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertListlet(sam_value value);

/**
 * Asserts that the given value is a valid `sam_value`, and
 * furthermore that it is a maplet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertMaplet(sam_value value);

/**
 * Asserts that the given value is a valid `sam_value`, and
 * furthermore that it is a uniquelet. If not, this aborts the process
 * with a diagnostic message.
 */
void samAssertUniquelet(sam_value value);

/**
 * Gets the low-level data type of the given value. `value` must be a
 * valid value (in particular, non-`NULL`).
 */
sam_type samType(sam_value value);

/**
 * Gets the size of the given value. `value` must be a valid value.
 * "Size" means:
 *
 * * an intlet's byte count
 * * a listlet's element count
 * * a maplet's mapping count
 * * `0` for all uniquelets
 */
sam_int samSize(sam_value value);

/**
 * Given an intlet, returns the `nth` byte. `value` must be an intlet, and
 * `n` must be `< samSize(value)`.
 */
sam_int samIntletGet(sam_value intlet, sam_int n);

/**
 * Given a listlet, returns the `nth` element. `value` must be a listlet, and
 * `n` must be `< samSize(value)`.
 */
sam_value samListletGet(sam_value listlet, sam_int n);

/**
 * Given a maplet, returns the `nth` mapping. `value` must be a maplet, and
 * `n` must be `< samSize(value)`.
 * 
 * Note: When retrieved in ordinal order, keys are always returned in
 * sorted order.
 */
sam_mapping samMapletGet(sam_value maplet, sam_int n);

/**
 * Given a maplet, find the index of the given key. `value` must be a
 * maplet. Returns the index of the key or `~insertionIndex` (a
 * negative number) if not found.
 */
sam_mapping samMapletFind(sam_value maplet, sam_value key);

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
sam_comparison samCompare(sam_value v1, sam_value v2);


/*
 * Intlet Constructors
 */

/**
 * Gets an intlet value equal to the given `sam_int`.
 */
sam_value samIntletFromInt(sam_int value);

/**
 * Gets an intlet value equal to the first UTF-8 code point in the given
 * byte string. Updates the byte string pointer.
 */
sam_value samIntletFromUtf8(const sam_byte **string);


/*
 * Listlet Constructors
 */

/**
 * Gets the value `@[]` (that is, the empty listlet).
 */
sam_value samListletEmpty(void);

/**
 * Gets the listlet resulting from appending the given value to the
 * given listlet.
 */
sam_value samListletAppend(sam_value listlet, sam_value value);

/**
 * Gets the listlet resulting from interpreting the given UTF-8
 * encoded string, whose size in bytes is as given.
 */
sam_value samListletFromUtf8(const sam_byte *string, sam_int stringSize);


/*
 * Maplet Constructors
 */

/**
 * Gets the value `@{}` (that is, the empty maplet).
 */
sam_value samMapletEmpty(void);

/**
 * Gets the maplet resulting from putting the given mapping into the
 * given maplet. This can either add a new mapping or replace an
 * existing mapping.
 */
sam_value samMapletPut(sam_value maplet, sam_value key, sam_value value);


/*
 * Uniquelet Constructors
 */

/**
 * Gets a new uniquelet. Each call to this function is guaranteed to
 * produce a value unequal to any other call to this function (in any
 * given process).
 */
sam_value samUniquelet(void);


#endif
