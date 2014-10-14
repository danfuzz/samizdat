// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Concrete `Box` classes
//

#ifndef _TYPE_BOX_H_
#define _TYPE_BOX_H_

#include "type/Generator.h"


/** Class value for in-model class `Box`. */
extern zvalue CLS_Box;

/** Class value for in-model class `Cell`. */
extern zvalue CLS_Cell;

/** Class value for in-model class `Promise`. */
extern zvalue CLS_Promise;

/** Class value for in-model class `Result`. */
extern zvalue CLS_Result;

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
