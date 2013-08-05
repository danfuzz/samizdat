/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Low-layer data structures
 */

#ifndef _DAT_H_
#define _DAT_H_

#include "pb.h"


/*
 * C Types
 */

/**
 * Prototype for state access/manipulation functions. These are called on
 * uniqlet state pointers, and on other non-value pointers that those may in
 * turn point to.
 */
typedef void (*zstateFunction)(void *state);

/**
 * Dispatch table for uniqlets with state.
 */
typedef struct {
    /** Function to call in order to do a gc mark on the uniqlet state. */
    zstateFunction mark;

    /** Function to call in order to free the uniqlet state's resources. */
    zstateFunction free;
} DatUniqletDispatch;


/*
 * Constants, type references and generic functions
 */

/** The standard value `[]`. */
extern zvalue EMPTY_LIST;

/** The standard value `[:]`. */
extern zvalue EMPTY_MAP;

/** The standard value `nullBox`. */
extern zvalue DAT_NULL_BOX;

/** Type value for in-model type `Box`. */
extern ztype DAT_Box;

/** Type value for in-model type `Deriv`. */
extern ztype DAT_Deriv;

/** Type value for in-model type `List`. */
extern ztype DAT_List;

/** Type value for in-model type `Map`. */
extern ztype DAT_Map;

/** Type value for in-model type `Uniqlet`. */
extern ztype DAT_Uniqlet;


/*
 * Initialization Function
 */

/**
 * Initializes the `dat` module.
 */
void datInit(void);


/*
 * Assertion Functions
 */

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a box. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertBox(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a list. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertList(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertMap(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a map, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
void datAssertMapSize1(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and furthermore
 * that it is a derived value. If not, this aborts the process with a
 * diagnostic message.
 */
void datAssertDeriv(zvalue value);

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a uniqlet. If not, this aborts the process
 * with a diagnostic message.
 */
void datAssertUniqlet(zvalue value);


/*
 * List Functions
 */

/**
 * Copies all the values of the given list into the given result
 * array, which must be sized large enough to hold all of them.
 */
void datArrayFromList(zvalue *result, zvalue list);

/**
 * Combines the elements of two lists, in order, into a new
 * list.
 *
 * Contrasting this with `datListAppend()`, that function operates
 * heterogeneously on a list and an element, whereas this one
 * operates on two peer lists.
 */
zvalue datListAdd(zvalue list1, zvalue list2);

/**
 * Gets the list resulting from appending the given value to the
 * given list.
 */
zvalue datListAppend(zvalue list, zvalue value);

/**
 * Gets the list resulting from deleting the nth element of the
 * given list.
 */
zvalue datListDelNth(zvalue list, zint n);

/**
 * Constructs a list from an array of `zvalue`s of the given size.
 */
zvalue datListFromArray(zint size, const zvalue *values);

/**
 * Gets the list resulting from inserting the given value at the
 * given index in the given list. `n` must be non-negative and no
 * greater than the size of the given list.
 */
zvalue datListInsNth(zvalue list, zint n, zvalue value);

/**
 * Given a list, returns the `n`th element. `list` must be a
 * list, and `n` must be `< datSize(value)`.
 */
zvalue datListNth(zvalue list, zint n);

/**
 * Gets the list resulting from setting the value at the
 * given index to the given value. `n` must be non-negative
 * and no greater than the size of the given list.
 */
zvalue datListPutNth(zvalue list, zint n, zvalue value);

/**
 * Gets the list consisting of the given "slice" of elements
 * (start inclusive, end exclusive) of the given list.
 */
zvalue datListSlice(zvalue list, zint start, zint end);


/*
 * Map Functions
 */

/**
 * Copies all the mappings of the given map into the given result
 * array, which must be sized large enough to hold all of them.
 */
void datArrayFromMap(zmapping *result, zvalue map);

/**
 * Gets the map resulting from putting the all the given mappings
 * into the given map, in the order given (so, in particular, higher-index
 * mappings take precedence over the lower-index mappings, when keys match).
 * The effect is identical to calling a chain of `datMapPut()`s on each
 * of the mappings in order.
 */
zvalue datMapAddArray(zvalue map, zint size, const zmapping *mappings);

/**
 * Combines the mappings of the two given maps into a new map.
 * For overlapping keys between the two, the second argument "wins".
 */
zvalue datMapAdd(zvalue map1, zvalue map2);

/**
 * Gets a map resulting from the removal of the given key from the
 * given map.
 */
zvalue datMapDel(zvalue map, zvalue key);

/**
 * Given a map, find the mapping of the given key and return the
 * corresponding value, or `NULL` if there is no such key. `map`
 * must be a map, and `key` must be a valid value.
 */
zvalue datMapGet(zvalue map, zvalue key);

/**
 * Given a map, returns the `n`th mapping. `map` must be a
 * map, and `n` must be `< datSize(value)`. Mappings are returned
 * as single-element maps. Map ordering is by key.
 */
zvalue datMapNth(zvalue map, zint n);

/**
 * Gets the map resulting from putting the given mapping into the
 * given map. This can either add a new mapping or replace an
 * existing mapping.
 */
zvalue datMapPut(zvalue map, zvalue key, zvalue value);

/**
 * Constructs and returns a single-mapping map.
 */
zvalue datMapping(zvalue key, zvalue value);

/**
 * Given a single-mapping map, returns its sole key. `map` must be a
 * map.
 */
zvalue datMappingKey(zvalue map);

/**
 * Given a single-mapping map, returns its sole value. `map` must be a
 * map.
 */
zvalue datMappingValue(zvalue map);


/*
 * Uniqlet Functions
 */

/**
 * Gets a new uniqlet. Each call to this function is guaranteed to
 * produce a value unequal to any other uniqlet (in any given process).
 */
zvalue datUniqlet(void);

/**
 * Gets the state value associated with the given uniqlet, asserting that
 * the uniqlet's dispatch table is as given.
 */
void *datUniqletGetState(zvalue uniqlet, DatUniqletDispatch *dispatch);

/**
 * Gets whether or not the given uniqlet has the given dispatch table.
 */
bool datUniqletHasDispatch(zvalue uniqlet, DatUniqletDispatch *dispatch);

/**
 * Gets a new uniqlet, associated with the given dispatcher and contents.
 * The uniqueness guarantee is the same as with `datUniqlet()`.
 *
 * In addition, this provides a convenient way to effectively build an
 * identity mapping between uniqlets (as the keys) and arbitrary
 * non-dat-module data (as the state values). Rather than store uniqlets
 * as keys in an external structure, this inverts the relationship,
 * storing the key (that represents the map) and associated `state`
 * inside the uniqlet. In object-capability terms, the key is a
 * sealer/unsealer, and the uniqlet serves secondary duty as a sealed
 * box. In this case, the key is in fact the associated state dispatch table.
 */
zvalue datUniqletWith(DatUniqletDispatch *dispatch, void *state);


/*
 * Boxes
 */

/**
 * Gets the value out of the given box. Returns `NULL` if the box is
 * void or as yet unset.
 */
zvalue boxGet(zvalue box);

/**
 * Returns an indication of whether or not the given box has been set.
 */
bool boxIsSet(zvalue box);

/**
 * Constructs a mutable (re-settable) box.
 */
zvalue boxMutable(void);

/**
 * Resets the given box to an un-set state. The box must be a mutable box.
 */
void boxReset(zvalue box);

/**
 * Sets the value of the given box as indicated. Passing `value` as
 * `NULL` indicates that the box is to be set to void.
 */
void boxSet(zvalue box, zvalue value);

/**
 * Constructs a yield (set-once) box.
 */
zvalue boxYield(void);


/*
 * Function application
 */

/**
 * Calls a function with the given list of arguments. `function` must be
 * a function (regular or generic), and `args` must be a list.
 */
zvalue datApply(zvalue function, zvalue args);


/*
 * Derived Value Functions
 */

/**
 * Returns a derived value with optional data payload. The given `data`
 * value must either be a valid value or `NULL`.
 *
 * **Note:** If `type` and `data` are of the right form to be represented
 * as a core value, this function will *not* notice that. So only call it
 * if you know that the value to be produced is *necessarily* derived. If
 * it's possible that the arguments correspond to a core value, use
 * `constValueFrom` (in the `const` module) instead.
 */
zvalue datDerivFrom(zvalue type, zvalue data);

#endif
