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
#include "type/Collection.h"


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
 * Gets the map resulting from adding all the given mappings
 * to an empty map, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 * The effect is identical to calling a chain of `put()`s on each
 * of the mappings in order, with the empty map as a base.
 *
 * **Warning:** This function *may* modify the memory pointed at by
 * `mappings`. However, once this function returns, it is safe to reuse
 * or discard the memory in question.
 */
zvalue mapFromArray(zint size, zmapping *mappings);

#endif
