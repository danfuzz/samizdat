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
 * Compares two classes for equality. It is an error to pass a non-class.
 */
bool classEq(zvalue cls1, zvalue cls2);

/**
 * Returns `true` iff the given class has the given secret.
 */
bool classHasSecret(zvalue cls, zvalue secret);

/**
 * Returns the unique index for the given class.
 */
zint classIndex(zvalue cls);

/**
 * Returns true iff the given class is derived (whether pure data or not).
 */
bool classIsDerived(zvalue cls);

/**
 * Gets the name of the given class, as a string.
 */
zvalue classNameString(zvalue cls);

/**
 * Gets the name of the given class, as a selector.
 */
zvalue classNameSelector(zvalue cls);

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
 * Makes a new class. `name` is the class's name (a selector or a string).
 * `parent` is its superclass. `secret` is the construction and access secret
 * (an arbitrary value). The two method table arguments must be
 * `SelectorTable`s or `NULL`.
 */
zvalue makeClass(zvalue name, zvalue parent, zvalue secret,
        zvalue classMethods, zvalue instanceMethods);

/**
 * Makes a new core class. This is just like `makeClass`, except that the
 * name is a C string and the predefined core class secret is used.
 */
zvalue makeCoreClass(const char *name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods);

#endif
