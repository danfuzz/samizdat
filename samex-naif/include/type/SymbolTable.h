// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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

/** Global function `makeSymbolTable`: Documented in spec. */
extern zvalue FUN_SymbolTable_makeSymbolTable;


/**
 * Copies all the values of the given symbol table into the given result
 * array, which must be sized large enough to hold all of them. The result
 * is indexed by symbol index.
 */
void arrayFromSymbolTable(zvalue *result, zvalue symbolTable);

/**
 * Makes a symbol table from a series of individual arguments (symbol
 * then value), `NULL` terminated.
 */
zvalue symbolTableFromArgs(zvalue first, ...);

/**
 * Makes a symbol table from an array of mappings. The keys must all be
 * symbols (of course).
 */
zvalue symbolTableFromArray(zint size, zmapping *mappings);

#endif
