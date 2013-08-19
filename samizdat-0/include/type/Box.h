/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Box` data type.
 */

#ifndef _TYPE_BOX_H_
#define _TYPE_BOX_H_

#include "pb.h"


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

#endif
