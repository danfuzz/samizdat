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
 * Constructs a list from an array of `zvalue`s of the given size.
 */
zvalue listFromArray(zint size, const zvalue *values);

#endif
