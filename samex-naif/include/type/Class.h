// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Class` class
//

#ifndef _TYPE_CLASS_H_
#define _TYPE_CLASS_H_

#include <stdbool.h>

#include "type/Value.h"


/** Class value for in-model class `Class`. */
extern zvalue CLS_Class;

/** Class value for in-model class `Metaclass`. */
extern zvalue CLS_Metaclass;

/**
 * Asserts that the given value has the given class. If not, this aborts
 * the process with a diagnostic message. **Note:** This does not do a
 * validity check on the given arguments.
 */
void assertHasClass(zvalue value, zvalue cls);

/**
 * Like `cls.accepts(value)`, except this works before the system is fully
 * started (in particular, before any methods are bound).
 */
bool classAccepts(zvalue cls, zvalue value);

/**
 * Returns `true` iff the given class has the given parent. This is for
 * a direct parent check only, not for general ancestry.
 */
bool classHasParent(zvalue cls, zvalue parent);

/**
 * Returns `true` iff the given class has the given secret.
 */
bool classHasSecret(zvalue cls, zvalue secret);

/**
 * Returns true iff the classes of the given values (that is, `get_class()` on
 * each) are the same.
 */
bool haveSameClass(zvalue value, zvalue other);

/**
 * Makes a new class. `name` is the class's name (a symbol or a string).
 * `parent` is its superclass. `secret` is the construction and access secret
 * (an arbitrary value). The two method table arguments must be
 * `SymbolTable`s or `NULL`. `NULL` is treated as `@{}` (the empty symbol
 * table).
 */
zvalue makeClass(zvalue name, zvalue parent, zvalue secret,
        zvalue classMethods, zvalue instanceMethods);

/**
 * Makes a new core class. This is just like `makeClass`, except that the
 * predefined core class secret is used.
 */
zvalue makeCoreClass(zvalue name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods);

#endif
