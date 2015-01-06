// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Bool` class
//

#ifndef _TYPE_BOOL_H_
#define _TYPE_BOOL_H_

#include "type/Core.h"


/** Class value for in-model class `Bool`. */
extern zvalue CLS_Bool;

/** The standard value `false`. */
extern zvalue BOOL_FALSE;

/** The standard value `true`. */
extern zvalue BOOL_TRUE;

/**
 * Gets an in-model `bool` value equal to the given `zbool`.
 */
zvalue boolFromZbool(zbool value);

/**
 * Gets a `zbool` equal to the given in-model `bool` value. `boolval` must be
 * a `bool`.
 */
zbool zboolFromBool(zvalue boolval);

#endif
