// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Collection` protocol
//
// **Note:** There is no in-model value `Collection`. There are a few
// different subsets of collection-like functionality which may eventually
// be represented as protocols or similar.
//
// **Note:** Because `collection` per se is an awkward prefix, instead the
// suggestive prefix `coll` is used.
//

#ifndef _TYPE_COLLECTION_H_
#define _TYPE_COLLECTION_H_

#include "type/Generator.h"  // Because all collections are generators.
#include "type/OneOff.h"


/** Method `.del(key)`: Documented in spec. */
SEL_DECL(2, 2, del);

/** Method `.keyList()`: Documented in spec. */
SEL_DECL(1, 1, keyList);

/** Method `.nthMapping(n)`: Documented in spec. */
SEL_DECL(2, 2, nthMapping);

/** Method `.put(key, value)`: Documented in spec. */
SEL_DECL(3, 3, put);

/** Method `.valueList()`: Documented in spec. */
SEL_DECL(1, 1, valueList);

/**
 * Calls the `del` method.
 */
zvalue collDel(zvalue coll, zvalue key);

/**
 * Calls the `put` method.
 */
zvalue collPut(zvalue coll, zvalue key, zvalue value);

#endif
