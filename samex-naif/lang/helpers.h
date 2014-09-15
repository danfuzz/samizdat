// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Collection constructor utilities.
//

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include "type/Value.h"


/**
 * Makes a 0-1 mapping derived data value.
 */
zvalue derivFrom1(zvalue cls, zvalue k1, zvalue v1);

/**
 * Makes a 0-2 mapping derived data value.
 */
zvalue derivFrom2(zvalue cls, zvalue k1, zvalue v1, zvalue k2, zvalue v2);

/**
 * Makes a 0-3 mapping derived data value.
 */
zvalue derivFrom3(zvalue cls, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3);

/**
 * Makes a 0-4 mapping derived data value. Values are allowed to be `NULL`, in
 * which case the corresponding key isn't included in the result.
 */
zvalue derivFrom4(zvalue cls, zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4);

/**
 * Makes a 0-1 mapping map.
 */
zvalue mapFrom1(zvalue k1, zvalue v1);

/**
 * Makes a 0-2 mapping map.
 */
zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2);

/**
 * Makes a 0-3 mapping map.
 */
zvalue mapFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3);

/**
 * Makes a 0-4 mapping map. Values are allowed to be `NULL`, in
 * which case the corresponding key isn't included in the result.
 */
zvalue mapFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4);

/**
 * Makes a 1 element list.
 */
zvalue listFrom1(zvalue e1);

/**
 * Makes a 2 element list.
 */
zvalue listFrom2(zvalue e1, zvalue e2);

/**
 * Makes a 3 element list.
 */
zvalue listFrom3(zvalue e1, zvalue e2, zvalue e3);

/**
 * Makes a 4 element list.
 */
zvalue listFrom4(zvalue e1, zvalue e2, zvalue e3, zvalue e4);

/**
 * Appends an element to a list.
 */
zvalue listAppend(zvalue list, zvalue elem);

/**
 * Prepends an element to a list.
 */
zvalue listPrepend(zvalue elem, zvalue list);

/**
 * Makes a 0-1 mapping symbol table.
 */
zvalue tableFrom1(zvalue k1, zvalue v1);

/**
 * Makes a 0-2 mapping symbol table.
 */
zvalue tableFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2);

/**
 * Makes a 0-3 mapping symbol table.
 */
zvalue tableFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3);

/**
 * Makes a 0-4 mapping symbol table. Values are allowed to be `NULL`, in
 * which case the corresponding key isn't included in the result.
 */
zvalue tableFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4);

#endif
