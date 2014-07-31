// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Class` class
//

#ifndef _TYPE_TYPE_H_
#define _TYPE_TYPE_H_

#include <stdbool.h>

#include "type/Value.h"


/** Class value for in-model class `Class`. */
extern zvalue CLS_Class;

/**
 * Asserts that the given value has the given class. If not, this aborts
 * the process with a diagnostic message. **Note:** This does not do a
 * validity check on the given arguments.
 */
void assertHasClass(zvalue value, zvalue cls);

/**
 * Adds a new method to a class. TODO: This function should get removed once
 * the system can cleanly construct classes with their methods in one go.
 */
void classAddMethod(zvalue cls, zvalue selector, zvalue function);

/**
 * Returns the unique index for the given class.
 */
zint classIndex(zvalue cls);

/**
 * Returns true iff the given class is derived (whether pure data or not).
 */
bool classIsDerived(zvalue cls);

/**
 * Gets the name of the given class.
 */
zvalue className(zvalue cls);

/**
 * Gets the parent class of the given class.
 */
zvalue classParent(zvalue cls);

/**
 * Returns the unique index for the class of the given value.
 */
zint get_classIndex(zvalue value);

/**
 * Returns true iff the class of the given value (that is, `get_class(value)`)
 * is either the given class or is a subclass of the given class.
 */
bool hasClass(zvalue value, zvalue cls);

/**
 * Returns true iff the classes of the given values (that is, `get_class()` on
 * each) are the same.
 */
bool haveSameClass(zvalue value, zvalue other);

/**
 * Makes a new core class. `name` is the class's name. `parent` is its
 * superclass. It is a fatal error to call this function more than once with
 * any given name.
 */
zvalue makeCoreClass(zvalue name, zvalue parent);

/**
 * Returns the class value for the derived data class with the given name.
 */
zvalue makeDerivedDataClass(zvalue name);

#endif
