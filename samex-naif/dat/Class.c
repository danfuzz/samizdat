// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Core.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "type/define.h"
#include "util.h"

#include "impl.h"


//
// Private Definitions
//

/**
 * Payload struct for class `Class`.
 */
typedef struct {
    /** Parent class. Only allowed to be `NULL` for `Value`. */
    zvalue parent;

    /** Name of the class, as a symbol. */
    zvalue name;

    /**
     * Whether the class is considered "core." See `Class.perOrder()` in the
     * spec for details.
     */
    bool isCore;

    /**
     * Bindings from method symbols to functions, keyed off of symbol
     * index number.
     */
    zvalue methods[DAT_MAX_SYMBOLS];
} ClassInfo;


/**
 * Gets a pointer to the value's info.
 */
static ClassInfo *getInfo(zvalue cls) {
    return datPayload(cls);
}

/**
 * Compare two classes for equality. Does *not* check to see if the two
 * arguments are actually classes.
 *
 * **Note:** This is just a `==` check, as the system doesn't allow for
 * two different underlying pointers to be references to the same class.
 */
static bool classEqUnchecked(zvalue cls1, zvalue cls2) {
    return (cls1 == cls2);
}

/**
 * Asserts that `value` is an instance of `Class` or a subclass thereof.
 */
static void assertIsClass(zvalue value) {
    zvalue cls = classOf(value);

    while (cls != NULL) {
        if (classEqUnchecked(cls, CLS_Class)) {
            return;
        }
        cls = getInfo(cls)->parent;
    }

    die("Expected a class; got %s.", cm_debugString(value));
}

/**
 * Allocates and partially initializes a class. Initialization includes
 * allocating a metaclass, setting up the correct is-a and heritage
 * relationships, and setting names. This does *not* set up any method tables.
 * Neither of the first two arguments can be `NULL`.
 *
 * If `isCore` is passed as `true`, then the result is marked a core class
 * and is also made immortal.
 *
 * As special cases, it is valid to pass `NULL` for the `parent` as long
 * as `Value` is not yet initialized, and it is valid to pass `NULL` for
 * `name` as long as `Symbol` is not yet initialized. These cases only hold
 * during the initial bootstrap of the system. And in both cases, passing
 * `NULL` means that the resulting classes will need to have more complete
 * initialization performed on them more "manually."
 */
static zvalue makeClassPair(zvalue name, zvalue parent, bool isCore) {
    if (CLS_Symbol != NULL) {
        if (name == NULL) {
            die("Improper argument to `makeClassPair()`: null `name`");
        } else {
            assertHasClass(name, CLS_Symbol);
        }
    }

    if (CLS_Value != NULL) {
        if (parent == NULL) {
            die("Improper argument to `makeClassPair()`: null `parent`");
        } else {
            assertIsClass(parent);
        }
    }

    // Note: The first time this is ever called, `CLS_Metaclass` is `NULL`.
    // The class in this case is corrected by explicitly setting it after
    // the call to this function.
    zvalue metacls = datAllocValue(CLS_Metaclass, sizeof(ClassInfo));
    zvalue cls = datAllocValue(metacls, sizeof(ClassInfo));
    ClassInfo *clsInfo = getInfo(cls);
    ClassInfo *metaInfo = getInfo(metacls);

    clsInfo->isCore = isCore;
    metaInfo->isCore = isCore;

    if (name != NULL) {
        clsInfo->name = name;
        metaInfo->name = symbolCat(SYM(meta_), name);
    }

    if (parent != NULL) {
        clsInfo->parent = parent;
        metaInfo->parent = parent->cls;
    }

    if (isCore) {
        datImmortalize(cls);
    }

    return cls;
}

/**
 * Performs the initialization that would have been done in `makeClassPair()`
 * except that the required classes weren't yet initialized.
 */
static void initEarlyClass(zvalue cls, zvalue name) {
    getInfo(cls)->name = name;
    getInfo(cls->cls)->name = symbolCat(SYM(meta_), name);
}

/**
 * Performs method "reinheritance" on the given class, copying any parent
 * methods that aren't already overridden by the given class. See call site
 * for more info.
 */
static void reinheritMethods(zvalue cls) {
    ClassInfo *info = getInfo(cls);
    ClassInfo *parentInfo = getInfo(info->parent);

    for (zint i = 0; i < DAT_MAX_SYMBOLS; i++) {
        zvalue one = parentInfo->methods[i];
        if ((one != NULL) && (info->methods[i] == NULL)) {
            info->methods[i] = one;
        }
    }
}

/**
 * Gets whether the given class is a core class. This is used for ordering
 * classes.
 */
static bool isCoreClass(ClassInfo *info) {
    return info->isCore;
}

/**
 * Shared implementation of `accepts`, which doesn't check to see if `cls`
 * is in fact a class.
 */
static bool acceptsUnchecked(zvalue cls, zvalue value) {
    for (zvalue valueCls = classOf(value);
            valueCls != NULL;
            valueCls = getInfo(valueCls)->parent) {
        if (classEqUnchecked(valueCls, cls)) {
            return true;
        }
    }

    return false;
}

/**
 * Helper for `classBindMethods`, which binds just one method table (class
 * or instance).
 */
static void bindOne(zvalue cls, zvalue methods) {
    ClassInfo *info = getInfo(cls);
    zint size = (methods == NULL) ? 0 : symtabSize(methods);

    if (info->parent != NULL) {
        // Initialize the method table with whatever the parent binds.
        utilCpy(zvalue, info->methods, getInfo(info->parent)->methods,
            DAT_MAX_SYMBOLS);
    }

    if (size != 0) {
        zmapping arr[size];
        arrayFromSymtab(arr, methods);
        for (zint i = 0; i < size; i++) {
            zint index = symbolIndex(arr[i].key);
            info->methods[index] = arr[i].value;
        }
    }
}


//
// Module Definitions
//

// Documented in header.
void classBindMethods(zvalue cls, zvalue classMethods,
        zvalue instanceMethods) {
    bindOne(cls->cls, classMethods);
    bindOne(cls, instanceMethods);
}

// Documented in header.
zvalue classFindMethodUnchecked(zvalue cls, zint index) {
    return getInfo(cls)->methods[index];
}

// Documented in header.
void callGcMark(zvalue value) {
    ClassInfo *info = getInfo(value->cls);
    zvalue func = info->methods[SYMIDX(gcMark)];

    if (func != NULL) {
        builtinCall(func, (zarray) {1, &value});
    }
}


//
// Exported Definitions
//

// This provides the non-inline version of this function.
extern void assertHasClass(zvalue value, zvalue cls);

// Documented in header.
void assertHasClass0(zvalue value, zvalue cls) {
    assertIsClass(cls);
    if (acceptsUnchecked(cls, value)) {
        die("Expected class %s; got %s of class %s.",
            cm_debugString(cls), cm_debugString(value),
            cm_debugString(classOf(value)));
    }
}

// This provides the non-inline version of this function.
extern zvalue classOf(zvalue value);

// Documented in header.
bool haveSameClass(zvalue value, zvalue other) {
    return classEqUnchecked(classOf(value), classOf(other));
}

// Documented in header.
zvalue makeClass(zvalue name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods) {
    assertIsClass(parent);

    zvalue result = makeClassPair(name, parent, false);
    classBindMethods(result, classMethods, instanceMethods);

    return result;
}

// Documented in header.
zvalue makeCoreClass(zvalue name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods) {
    assertIsClass(parent);

    zvalue result = makeClassPair(name, parent, true);
    classBindMethods(result, classMethods, instanceMethods);

    return result;
}

// Documented in header.
zvalue typeAccepts(zvalue cls, zvalue value) {
    return (METH_CALL(cls, accepts, value) != NULL) ? value : NULL;
}

// Documented in header.
zvalue typeCast(zvalue cls, zvalue value) {
    if (METH_CALL(cls, accepts, value) != NULL) {
        return value;
    }

    zvalue result = METH_CALL(value, castToward, cls);

    if (result != NULL) {
        if (METH_CALL(cls, accepts, result)) {
            return result;
        }
        value = result;
    }

    result = METH_CALL(cls, castFrom, value);

    if ((result != NULL) && METH_CALL(cls, accepts, result)) {
        return result;
    }

    return NULL;
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_1(Class, of, value) {
    return classOf(value);
}

// Documented in spec.
CMETH_IMPL_2(Class, typeAccepts, cls, value) {
    return typeAccepts(cls, value);
}

// Documented in spec.
CMETH_IMPL_2(Class, typeCast, cls, value) {
    return typeCast(cls, value);
}

// Documented in spec.
METH_IMPL_1(Class, accepts, value) {
    return acceptsUnchecked(ths, value) ? value : NULL;
}

// Documented in spec.
METH_IMPL_1(Class, castFrom, value) {
    return acceptsUnchecked(ths, value) ? value : NULL;
}

// Documented in spec.
METH_IMPL_1(Class, crossEq, other) {
    assertIsClass(other);  // Note: Not guaranteed to be a `Class`.
    return classEqUnchecked(ths, other) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(Class, crossOrder, other) {
    if (ths == other) {
        // Easy case to avoid decomposition and detailed tests.
        return SYM(same);
    }

    if (!haveSameClass(ths, other)) {
        die("Improper call to `crossOrder`: different concrete classes");
    }

    // We should only be able to make it here if given two instances of
    // `Metaclass`.
    assertHasClass(ths, CLS_Metaclass);

    // Sort by name, treating same-name as unordered (but not an error).
    ClassInfo *info1 = getInfo(ths);
    ClassInfo *info2 = getInfo(other);

    switch (cm_order(info1->name, info2->name)) {
        case ZLESS: { return SYM(less); }
        case ZMORE: { return SYM(more); }
        case ZSAME: { return NULL;      }
    }
}

// Documented in spec.
METH_IMPL_0(Class, debugString) {
    ClassInfo *info = getInfo(ths);
    zvalue nameString = cm_castFrom(CLS_String, info->name);

    if (info->isCore) {
        return nameString;
    } else {
        return cm_cat(
            stringFromUtf8(-1, "@<user class "),
            nameString,
            stringFromUtf8(-1, ">"));
    }
}

// Documented in spec.
METH_IMPL_0(Class, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(Class, gcMark) {
    ClassInfo *info = getInfo(ths);

    datMark(info->parent);
    datMark(info->name);

    for (zint i = 0; i < DAT_MAX_SYMBOLS; i++) {
        datMark(info->methods[i]);
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_0(Class, get_name) {
    return getInfo(ths)->name;
}

// Documented in spec.
METH_IMPL_0(Class, get_parent) {
    return getInfo(ths)->parent;
}

// Documented in spec.
METH_IMPL_1(Class, perOrder, other) {
    if (ths == other) {
        // Easy case to avoid decomposition and detailed tests.
        return SYM(same);
    }

    assertIsClass(other);  // Note: Not guaranteed to be a `Class`.
    ClassInfo *info1 = getInfo(ths);
    ClassInfo *info2 = getInfo(other);
    bool core1 = isCoreClass(info1);
    bool core2 = isCoreClass(info2);

    // Compare categories for major order.

    if (core1 && !core2) {
        return SYM(less);
    } else if ((!core1) && core2) {
        return SYM(more);
    }

    // Compare names for minor order.

    switch (cm_order(info1->name, info2->name)) {
        case ZLESS: { return SYM(less); }
        case ZMORE: { return SYM(more); }
        case ZSAME: {
            // Names are the same. The order is not defined given two
            // different non-core classes.
            if (core1 || core2) {
                die("Shouldn't happen: Same-name-but-different core classes.");
            }
            return NULL;
        }
    }
}

/**
 * Define `objectModel` as a module, as separate from the `Class` class. This
 * sets up the "corest of the core" classes.
 */
MOD_INIT(objectModel) {
    // Make sure that the "exposed" header is sized the same as the real one.
    if (sizeof(DatHeaderExposed) != sizeof(DatHeader)) {
        die("Mismatched exposed header size: should be %d",
            (zint) sizeof(DatHeader));
    }

    // Set up the "knotted" classes. These are the ones that have circular
    // is-a and/or heritage relationships with each other. This does *not*
    // include setting up `Symbol`, which is why the `name` of all of these
    // start out as `NULL`.

    // `NULL` for `parent` here, because the superclass of `Metaclass` is
    // `Class`, and the latter doesn't yet exist. The immediately-following
    // `cls` assignment is to set up the required circular is-a relationship
    // between `Metaclass` and its metaclass.
    CLS_Metaclass = makeClassPair(NULL, NULL, true);
    CLS_Metaclass->cls->cls = CLS_Metaclass;

    // Similarly, `NULL` for `parent` here, because the superclass of `Class`
    // is `Value`.
    CLS_Class = makeClassPair(NULL, NULL, true);

    // `NULL` for `parent` here, because `Value` per se has no superclass.
    // However, `Value`'s metaclass *does* have a superclass, `Class`, so
    // we assign it explicitly, immediately below.
    CLS_Value = makeClassPair(NULL, NULL, true);

    // Finally, set up the missing the heritage relationships.
    getInfo(CLS_Value->cls)->parent = CLS_Class;
    getInfo(CLS_Class)->parent = CLS_Value;
    getInfo(CLS_Class->cls)->parent = CLS_Value->cls;
    getInfo(CLS_Metaclass)->parent = CLS_Class;
    getInfo(CLS_Metaclass->cls)->parent = CLS_Class->cls;

    // With the "knotted" classes taken care of, now do the initial
    // special-case setup of `Core` and `Symbol`. These are required for
    // classes to have `name`s.

    // Construct these with `NULL` `name` initially.
    CLS_Core   = makeClassPair(NULL, CLS_Value, true);
    CLS_Symbol = makeClassPair(NULL, CLS_Core,  true);

    // With `Symbol` barely initialized, it's now possible to make the
    // interned instances as needed by the rest of the core.
    initCoreSymbols();

    // And with that, initialize `name` on all the classes constructed above.
    initEarlyClass(CLS_Class,     SYM(Class));
    initEarlyClass(CLS_Core,      SYM(Core));
    initEarlyClass(CLS_Metaclass, SYM(Metaclass));
    initEarlyClass(CLS_Symbol,    SYM(Symbol));
    initEarlyClass(CLS_Value,     SYM(Value));

    // Finally, construct the classes that are required in order for
    // methods to be bound to classes.

    CLS_SymbolTable = makeClassPair(SYM(SymbolTable), CLS_Core, true);
    CLS_Builtin     = makeClassPair(SYM(Builtin),     CLS_Core, true);

    // At this point, all of the "corest" classes exist but have no bound
    // methods. Their methods get bound by the following calls. The order of
    // these calls is significant, since method table setup starts by copying
    // the parent's class and metaclass tables. Instead of trying to get fancy
    // with recursive `MOD_USE()` calls (or something like that), we just use
    // an order here that works.

    bindMethodsForValue();
    bindMethodsForClass(); // See below.

    // These calls are needed because of the circular nature of classes: All
    // of these classes' metaclasses got bound before `Class` itself got its
    // methods bound, and so we need to "re-percolate" `Class`'s method
    // bindings down through them.
    reinheritMethods(CLS_Value->cls);
    reinheritMethods(CLS_Class->cls);
    reinheritMethods(CLS_Metaclass->cls);

    bindMethodsForCore();
    bindMethodsForSymbol();
    bindMethodsForSymbolTable();
    bindMethodsForBuiltin();
}

// Documented in header.
void bindMethodsForClass(void) {
    classBindMethods(CLS_Class,
        METH_TABLE(
            CMETH_BIND(Class, of),
            CMETH_BIND(Class, typeAccepts),
            CMETH_BIND(Class, typeCast)),
        METH_TABLE(
            METH_BIND(Class, accepts),
            METH_BIND(Class, castFrom),
            METH_BIND(Class, crossEq),
            METH_BIND(Class, crossOrder),
            METH_BIND(Class, debugString),
            METH_BIND(Class, debugSymbol),
            METH_BIND(Class, gcMark),
            METH_BIND(Class, get_name),
            METH_BIND(Class, get_parent),
            METH_BIND(Class, perOrder)));

    // `Metaclass` binds no methods itself. TODO: It probably wants at least
    // a couple.
    classBindMethods(CLS_Metaclass,
        NULL,
        NULL);
}

/** Initializes the module. */
MOD_INIT(Class) {
    MOD_USE(Value);

    // No class init here. That happens in `MOD_INIT(objectModel)` and
    // and `bindMethodsForClass()`.
}

// Documented in header.
zvalue CLS_Metaclass = NULL;

// Documented in header.
zvalue CLS_Class = NULL;
