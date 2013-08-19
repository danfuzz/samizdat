/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Mid-layer data types
 */

#ifndef _DAT_H_
#define _DAT_H_

#include "pb.h"


/*
 * `List` Type Declarations
 */

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
 * Combines the elements of two lists, in order, into a new
 * list.
 */
zvalue listCat(zvalue list1, zvalue list2);

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
 * `Map` Type Declarations
 */

/** Type value for in-model type `Map`. */
extern zvalue TYPE_Map;

/** The standard value `[:]`. */
extern zvalue EMPTY_MAP;

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
zvalue mapCatArray(zvalue map, zint size, const zmapping *mappings);

/**
 * Combines the mappings of the two given maps into a new map.
 * For overlapping keys between the two, the second argument "wins".
 */
zvalue mapCat(zvalue map1, zvalue map2);

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
 * `Box` Type Declarations
 */

/** Type value for in-model type `Box`. */
extern zvalue TYPE_Box;

/**
 * Generic `canStore(box)`: Returns `box` if it can be stored to. Documented
 * in spec.
 */
extern zvalue GFN_canStore;

/**
 * Generic `fetch(box)`: Fetches the value from a box. Documented in spec.
 */
extern zvalue GFN_fetch;

/**
 * Generic `store(box, value?)`: Stores a value (or void) into a box.
 * Documented in spec.
 */
extern zvalue GFN_store;

/** The standard value `nullBox`. */
extern zvalue DAT_NULL_BOX;

/**
 * Constructs a mutable (re-settable) box, with the given initial value.
 * Pass `NULL` to leave it initially unset.
 */
zvalue makeMutableBox(zvalue value);

/**
 * Constructs a yield (set-once) box.
 */
zvalue makeYieldBox(void);


/*
 * Miscellaneous Declarations
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

/**
 * Initializes the `dat` module.
 */
void datInit(void);

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `args` must be a list.
 */
zvalue funApply(zvalue function, zvalue args);

#endif
