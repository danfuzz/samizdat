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

#include "pb.h"


/*
 * C Types
 */

/**
 * Prototype for state access/manipulation functions. These are called on
 * uniqlet state pointers, and on other non-value pointers that those may in
 * turn point to.
 */
typedef void (*zstateFunction)(void *state);


/*
 * Constants, type references and generic functions
 */

/** The standard value `[]`. */
extern zvalue EMPTY_LIST;

/** The standard value `[:]`. */
extern zvalue EMPTY_MAP;

/** The standard value `nullBox`. */
extern zvalue DAT_NULL_BOX;

/** Type value for in-model type `Box`. */
extern zvalue TYPE_Box;

/** Type value for in-model type `List`. */
extern zvalue TYPE_List;

/** Type value for in-model type `Map`. */
extern zvalue TYPE_Map;


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


/*
 * List Functions
 */

/**
 * Copies all the values of the given list into the given result
 * array, which must be sized large enough to hold all of them.
 */
void arrayFromList(zvalue *result, zvalue list);

/**
 * Combines the elements of two lists, in order, into a new
 * list.
 *
 * Contrasting this with `listAppend()`, that function operates
 * heterogeneously on a list and an element, whereas this one
 * operates on two peer lists.
 */
zvalue listAdd(zvalue list1, zvalue list2);

/**
 * Gets the list resulting from appending the given value to the
 * given list.
 */
zvalue listAppend(zvalue list, zvalue value);

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
 * Given a list, returns the `n`th element. `list` must be a
 * list, and `n` must be `< pbSize(value)`.
 */
zvalue listNth(zvalue list, zint n);

/**
 * Gets the list resulting from setting the value at the
 * given index to the given value. `n` must be non-negative
 * and no greater than the size of the given list.
 */
zvalue listPutNth(zvalue list, zint n, zvalue value);

/**
 * Gets the list consisting of the given "slice" of elements
 * (start inclusive, end exclusive) of the given list.
 */
zvalue listSlice(zvalue list, zint start, zint end);


/*
 * Map Functions
 */

/**
 * Copies all the mappings of the given map into the given result
 * array, which must be sized large enough to hold all of them.
 */
void arrayFromMap(zmapping *result, zvalue map);

/**
 * Gets the map resulting from putting the all the given mappings
 * into the given map, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 * The effect is identical to calling a chain of `mapPut()`s on each
 * of the mappings in order.
 */
zvalue mapAddArray(zvalue map, zint size, const zmapping *mappings);

/**
 * Combines the mappings of the two given maps into a new map.
 * For overlapping keys between the two, the second argument "wins".
 */
zvalue mapAdd(zvalue map1, zvalue map2);

/**
 * Gets a map resulting from the removal of the given key from the
 * given map.
 */
zvalue mapDel(zvalue map, zvalue key);

/**
 * Given a map, find the mapping of the given key and return the
 * corresponding value, or `NULL` if there is no such key. `map`
 * must be a map, and `key` must be a valid value.
 */
zvalue mapGet(zvalue map, zvalue key);

/**
 * Given a map, returns the `n`th mapping. `map` must be a
 * map, and `n` must be `< pbSize(value)`. Mappings are returned
 * as single-element maps. Map ordering is by key.
 */
zvalue mapNth(zvalue map, zint n);

/**
 * Gets the map resulting from putting the given mapping into the
 * given map. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue mapPut(zvalue map, zvalue key, zvalue value);

/**
 * Constructs and returns a single-mapping map.
 */
zvalue mapping(zvalue key, zvalue value);

/**
 * Given a single-mapping map, returns its sole key. `map` must be a
 * map.
 */
zvalue mappingKey(zvalue map);

/**
 * Given a single-mapping map, returns its sole value. `map` must be a
 * map.
 */
zvalue mappingValue(zvalue map);


/*
 * Boxes
 */

/**
 * Gets the value out of the given box. Returns `NULL` if the box is
 * void or as yet unset.
 */
zvalue boxGet(zvalue box);

/**
 * Returns an indication of whether or not the given box has been set.
 */
bool boxCanStore(zvalue box);

/**
 * Resets the given box to an un-set state. The box must be a mutable box.
 */
void boxReset(zvalue box);

/**
 * Sets the value of the given box as indicated. Passing `value` as
 * `NULL` indicates that the box is to be set to void.
 */
void boxSet(zvalue box, zvalue value);

/**
 * Constructs a mutable (re-settable) box.
 */
zvalue makeMutableBox(void);

/**
 * Constructs a yield (set-once) box.
 */
zvalue makeYieldBox(void);


/*
 * Function application
 */

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `args` must be a list.
 */
zvalue funApply(zvalue function, zvalue args);

#endif
