// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Box` data type.
//

#ifndef _TYPE_BOX_H_
#define _TYPE_BOX_H_

#include "dat.h"
#include "type/Generator.h"


/** Type value for in-model type `Box`. */
extern zvalue TYPE_Box;

/**
 * Generic `store(box, value?)`: Stores a value (or void) into a box.
 * Documented in spec.
 */
extern zvalue GFN_store;

/**
 * Constructs a mutable (re-settable) box, with the given initial value.
 * Pass `NULL` to leave it initially unset.
 */
zvalue makeCell(zvalue value);

/**
 * Constructs a yield (set-once) box.
 */
zvalue makePromise(void);

/**
 * Constructs a permanently-set box, with the given value. Pass `NULL` to
 * make it un-valued.
 */
zvalue makeResult(zvalue value);

#endif
