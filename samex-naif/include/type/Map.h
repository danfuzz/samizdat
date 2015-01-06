// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Map` class
//

#ifndef _TYPE_MAP_H_
#define _TYPE_MAP_H_

#include "type/Core.h"


/** Class value for in-model class `Map`. */
extern zvalue CLS_Map;

/** The standard value `{}`. */
extern zvalue EMPTY_MAP;

/**
 * Gets the map resulting from adding all the given mappings
 * to an empty map, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 *
 * **Warning:** This function *may* modify the memory pointed at by
 * `mappings`. However, once this function returns, it is safe to reuse
 * or discard the memory in question.
 */
zvalue mapFromArray(zint size, zmapping *mappings);

/**
 * Gets a single-mapping map of the given mapping.
 */
zvalue mapFromMapping(zmapping mapping);

/**
 * Gets a `zassoc` of the given map. The result `elems` shares storage
 * with `map`. As such, it is important to *not* modify the contents.
 */
zassoc zassocFromMap(zvalue map);


#endif
