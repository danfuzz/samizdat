/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `List` data type
 */

#ifndef _TYPE_LIST_H_
#define _TYPE_LIST_H_

#include "pb.h"
#include "type/Collection.h"


/** Type value for in-model type `List`. */
extern zvalue TYPE_List;

/** The standard value `[]`. */
extern zvalue EMPTY_LIST;

/**
 * Copies all the values of the given list into the given result
 * array, which must be sized large enough to hold all of them.
 */
void arrayFromList(zvalue *result, zvalue list);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a list. If not, this aborts the process
 * with a diagnostic message.
 */
void assertList(zvalue value);

/**
 * Gets the list resulting from deleting the nth element of the
 * given list.
 */
zvalue listDelNth(zvalue list, zint n);

/**
 * Constructs a list from an array of `zvalue`s of the given size.
 */
zvalue listFromArray(zint size, const zvalue *values);

/**
 * Gets the list resulting from inserting the given value at the
 * given index in the given list. `n` must be non-negative and no
 * greater than the size of the given list.
 */
zvalue listInsNth(zvalue list, zint n, zvalue value);

/**
 * Gets the list resulting from setting the value at the
 * given index to the given value. `n` must be non-negative
 * and no greater than the size of the given list.
 */
zvalue listPutNth(zvalue list, zint n, zvalue value);

#endif
