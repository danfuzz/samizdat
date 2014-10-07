// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Map` class
//

#ifndef _TYPE_MAP_H_
#define _TYPE_MAP_H_

#include "type/Core.h"


/** Class value for in-model class `Map`. */
extern zvalue CLS_Map;

/** The standard value `{}`. */
extern zvalue EMPTY_MAP;

/**
 * Copies all the mappings of the given map into the given result
 * array, which must be sized large enough to hold all of them.
 */
void arrayFromMap(zmapping *result, zvalue map);

/**
 * Constructs a map from a `NULL`-terminated list of key-then-value argument
 * pairs.
 */
zvalue mapFromArgs(zvalue first, ...);

/**
 * Gets the map resulting from adding all the given mappings
 * to an empty map, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 * The effect is identical to calling a chain of `put()`s on each
 * of the mappings in order, with the empty map as a base.
 *
 * **Warning:** This function *may* modify the memory pointed at by
 * `mappings`. However, once this function returns, it is safe to reuse
 * or discard the memory in question.
 */
zvalue mapFromArray(zint size, zmapping *mappings);

/**
 * Gets the map whose bindings are identical to the given symbol table's.
 * This only works if the symbol table's bindings are fully ordered. Ordering
 * is *not* possible if there are two different unlisted symbols in the
 * table with the same name.
 */
zvalue mapFromSymbolTable(zvalue symbolTable);

/**
 * Gets the symbol table whose bindings are identical to the given map's.
 * This only works if the map's keys are all symbols.
 *
 * **Note:** This is defined as part of `Map` and not `SymbolTable`, as the
 * latter is meant to be lower layer and with minimal dependencies.
 */
zvalue symbolTableFromMap(zvalue map);

#endif
