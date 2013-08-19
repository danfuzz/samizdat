/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Map` data type
 */

#ifndef _TYPE_MAP_H_
#define _TYPE_MAP_H_

#include "pb.h"


/**
 * Arbitrary (key, value) mapping.
 */
typedef struct {
    /** The key. */
    zvalue key;

    /** The value. */
    zvalue value;
} zmapping;

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
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map. If not, this aborts the process
 * with a diagnostic message.
 */
void assertMap(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
void assertMapSize1(zvalue value);

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
 * map, and `n` must be `< valSize(value)`. Mappings are returned
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

#endif
