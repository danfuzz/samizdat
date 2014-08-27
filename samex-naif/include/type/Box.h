// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Box` class
//

#ifndef _TYPE_BOX_H_
#define _TYPE_BOX_H_

#include "type/Generator.h"


/** Class value for in-model class `Box`. */
extern zvalue CLS_Box;

/** Method `.store(value?)`: Documented in spec. */
SYM_DECL(store);

/**
 * Calls the method `store`, with either one or two arguments, one if
 * `value` is `NULL`, two if not. `box` is *not* allowed to be `NULL`.
 */
zvalue boxStoreNullOk(zvalue box, zvalue value);

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
