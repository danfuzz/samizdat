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
 * Helper for `assertHasClass()` which does a more heavyweight check.
 * This should *not* be called directly.
 */
void assertHasClass0(zvalue value, zvalue cls);

/**
 * Asserts that the given value has the given class. If not, this aborts
 * the process with a diagnostic message. **Note:** This does not necessarily
 * do a validity check on the given arguments.
 */
inline void assertHasClass(zvalue value, zvalue cls) {
    if (classOf(value) != cls) {
        assertHasClass0(value, cls);
    }
}

/**
 * Performs the equivalent of the static method call `Class.typeCast(cls, value)`.
 */
zvalue cast(zvalue cls, zvalue value);

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
 * Returns true iff the classes of the given values (that is, `classOf()` on
 * each) are the same.
 */
bool haveSameClass(zvalue value, zvalue other);

/**
 * Makes a new class. `name` is the class's name (a symbol or a string).
 * `parent` is its superclass. The two method table arguments must be
 * `SymbolTable`s or `NULL`. `NULL` is treated as `@{}` (the empty symbol
 * table).
 */
zvalue makeClass(zvalue name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods);

/**
 * Makes a new core class. This is just like `makeClass`, except that the
 * result (a) is designated as a "core" class, and (b) is marked "immortal."
 */
zvalue makeCoreClass(zvalue name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods);

/**
 * Performs the equivalent of the static method call
 * `Class.typeCast(cls, value)`.
 */
zvalue typeCast(zvalue cls, zvalue value);

#endif
