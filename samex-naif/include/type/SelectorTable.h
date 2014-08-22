// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `SelectorTable` class
//

#ifndef _TYPE_SELECTOR_TABLE_H_
#define _TYPE_SELECTOR_TABLE_H_

#include "type/Value.h"

/** Class value for in-model class `SelectorTable`. */
extern zvalue CLS_SelectorTable;

/** The standard empty selector table value. */
extern zvalue EMPTY_SELECTOR_TABLE;

/** Global function `makeSelectorTable`: Documented in spec. */
extern zvalue FUN_SelectorTable_makeSelectorTable;


/**
 * Makes a selector table from a series of individual arguments (selector
 * then value), `NULL` terminated.
 */
zvalue selectorTableFromArgs(zvalue first, ...);

/**
 * Makes a selector table from an array of mappings. The keys must all be
 * selectors (of course).
 */
zvalue selectorTableFromArray(zint size, zmapping *mappings);

#endif
