// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `SymbolTable` class
//

#ifndef _TYPE_SYMBOL_TABLE_H_
#define _TYPE_SYMBOL_TABLE_H_

#include "type/Value.h"

/** Class value for in-model class `SymbolTable`. */
extern zvalue CLS_SymbolTable;

/** The standard empty symbol table value. */
extern zvalue EMPTY_SYMBOL_TABLE;


/**
 * Copies all the values of the given symbol table into the given result
 * array, which must be sized large enough to hold all of them. The result
 * has no particular ordering.
 */
void arrayFromSymtab(zmapping *result, zvalue symtab);

/**
 * Adds or replaces a mapping in a symbol table. This is equivalent to
 * calling the `.cat()` method with a single-mapping argument, but (a) avoids
 * method dispatch and (b) is available for use before the system is fully
 * booted.
 */
zvalue symtabCatMapping(zvalue symtab, zmapping mapping);

/**
 * Adds or replaces a series of mappings in a symbol table. This is equivalent
 * to calling the `.cat()` method with the result of a call to
 * `symtabFromZassoc`, but (a) avoids method dispatch and (b) is available for
 * use before the system is fully booted.
 */
zvalue symtabCatZassoc(zvalue symtab, zassoc ass);

/**
 * Gets a single-mapping symbol table of the given mapping.
 */
zvalue symtabFromMapping(zmapping mapping);

/**
 * Makes a symbol table from a `zarray` where the contents are a list of
 * "splayed" key-then-value pairs. The keys must all be symbols (of course).
 */
zvalue symtabFromZarray(zarray arr);

/**
 * Makes a symbol table from a `zassoc`. The keys must all be symbols (of
 * course).
 */
zvalue symtabFromZassoc(zassoc ass);

/**
 * Gets a single bound value from of a symbol table. This is equivalent to
 * calling the `.get()` method but (a) avoids method dispatch and (b) is
 * available for use before the system is fully booted.
 */
zvalue symtabGet(zvalue symtab, zvalue key);

/**
 * Gets the size of a symbol table.
 */
zint symtabSize(zvalue symtab);

#endif
