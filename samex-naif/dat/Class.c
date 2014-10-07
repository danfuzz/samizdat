// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Core.h"
#include "type/Int.h"
#include "type/Jump.h"
#include "type/Object.h"
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

    /** Access secret of the class. Optional, and arbitrary if present. */
    zvalue secret;

    /**
     * Bindings from method symbols to functions, keyed off of symbol
     * index number.
     */
    zvalue methods[DAT_MAX_SYMBOLS];
} ClassInfo;

/** The `secret` value used for all core classes. */
static zvalue theCoreSecret = NULL;


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
    zvalue cls = get_class(value);

    while (cls != NULL) {
        if (classEqUnchecked(cls, CLS_Class)) {
            return;
        }
        cls = getInfo(cls)->parent;
    }

    die("Expected a class; got %s.", valDebugString(value));
}

/**
 * Allocates and partially initializes a class. Initialization includes
 * allocating a metaclass, setting up the correct is-a and heritage
 * relationships, and setting names and secrets. This does *not* set up
 * any method tables. `secret` is allowed to be `NULL`, but neither of the
 * other two arguments can be `NULL`.
 *
 * As special cases, it is valid to pass `NULL` for the `parent` as long
 * as `Value` is not yet initialized, and it is valid to pass `NULL` for
 * `name` as long as `Symbol` is not yet initialized. These cases only hold
 * during the initial bootstrap of the system. And in both cases, passing
 * `NULL` means that the resulting classes will need to have more complete
 * initialization performed on them more "manually."
 */
static zvalue makeClassPair(zvalue name, zvalue parent, zvalue secret) {
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

    clsInfo->secret = secret;
    metaInfo->secret = secret;

    if (name != NULL) {
        clsInfo->name = name;
        metaInfo->name = symbolCat(SYM(meta_), name);
    }

    if (parent != NULL) {
        clsInfo->parent = parent;
        metaInfo->parent = parent->cls;
    }

    // TODO: Consider *not* making all classes immortal.
    datImmortalize(cls);

    return cls;
}

/**
 * Performs the initialization that would have been done in `makeClassPair()`
 * except that the required classes weren't yet initialized. This includes
 * setting up the name and secret for both a class and its metaclass.
 */
static void initEarlyClass(zvalue cls, zvalue name) {
    ClassInfo *clsInfo = getInfo(cls);
    ClassInfo *metaInfo = getInfo(cls->cls);

    clsInfo->secret = theCoreSecret;
    metaInfo->secret = theCoreSecret;

    clsInfo->name = name;
    metaInfo->name = symbolCat(SYM(meta_), name);
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
    return info->secret == theCoreSecret;
}


//
// Module Definitions
//

// Documented in header.
void classBindMethods(zvalue cls, zvalue classMethods,
        zvalue instanceMethods) {
    ClassInfo *clsInfo = getInfo(cls);
    ClassInfo *metaInfo = getInfo(cls->cls);
    zint cmethSize;
    zint imethSize;

    if (classMethods == NULL) {
        cmethSize = 0;
    } else {
        assertHasClass(classMethods, CLS_SymbolTable);
        cmethSize = symbolTableSize(classMethods);
    }

    if (instanceMethods == NULL) {
        imethSize = 0;
    } else {
        imethSize = symbolTableSize(instanceMethods);
    }

    if (clsInfo->parent != NULL) {
        // Initialize the method tables with whatever the parent defined.
        utilCpy(zvalue, clsInfo->methods, getInfo(clsInfo->parent)->methods,
            DAT_MAX_SYMBOLS);
        utilCpy(zvalue, metaInfo->methods, getInfo(metaInfo->parent)->methods,
            DAT_MAX_SYMBOLS);
    }

    if (cmethSize != 0) {
        die("No class methods allowed...yet.");
    }

    if (imethSize != 0) {
        zmapping methods[imethSize];
        arrayFromSymbolTable(methods, instanceMethods);
        for (zint i = 0; i < imethSize; i++) {
            zvalue sym = methods[i].key;
            zint index = symbolIndex(methods[i].key);
            clsInfo->methods[index] = methods[i].value;
        }
    }
}

// Documented in header.
zvalue classFindMethodUnchecked(zvalue cls, zint index) {
    return getInfo(cls)->methods[index];
}


//
// Exported Definitions
//

// Documented in header.
void assertHasClass(zvalue value, zvalue cls) {
    if (!hasClass(value, cls)) {
        die("Expected class %s; got %s.",
            valDebugString(cls), valDebugString(value));
    }
}

// Documented in header.
bool classHasParent(zvalue cls, zvalue parent) {
    assertIsClass(cls);
    assertIsClass(parent);
    return classEqUnchecked(getInfo(cls)->parent, parent);
}

// Documented in header.
bool classHasSecret(zvalue cls, zvalue secret) {
    assertIsClass(cls);

    ClassInfo *info = getInfo(cls);

    // Note: It's important to pass `info->secret` first, so that it's the
    // one whose `totalEq` method is used. The given `secret` can't be
    // trusted to behave.
    return valEq(info->secret, secret);
}

// Documented in header.
bool hasClass(zvalue value, zvalue cls) {
    assertIsClass(cls);

    for (zvalue valueCls = get_class(value);
            valueCls != NULL;
            valueCls = getInfo(valueCls)->parent) {
        if (classEqUnchecked(valueCls, cls)) {
            return true;
        }
    }

    return false;
}

// Documented in header.
bool haveSameClass(zvalue value, zvalue other) {
    return classEqUnchecked(get_class(value), get_class(other));
}

// Documented in header.
zvalue makeClass(zvalue name, zvalue parent, zvalue secret,
        zvalue classMethods, zvalue instanceMethods) {
    assertIsClass(parent);

    zvalue result = makeClassPair(name, parent, secret);
    classBindMethods(result, classMethods, instanceMethods);

    return result;
}

// Documented in header.
zvalue makeCoreClass(const char *name, zvalue parent,
        zvalue classMethods, zvalue instanceMethods) {
    return makeClass(symbolFromUtf8(-1, name), parent, theCoreSecret,
        classMethods, instanceMethods);
}


//
// Class Definition
//

// Documented in header.
METH_IMPL_0(Class, debugString) {
    ClassInfo *info = getInfo(ths);
    const char *label;

    if (info->secret == theCoreSecret) {
        return valToString(info->name);
    } else if (info->secret != NULL) {
        label = "object";
    } else {
        die("Shouldn't happen: non-core class without secret.");
    }

    return METH_CALL(cat,
        stringFromUtf8(-1, "@<"),
        stringFromUtf8(-1, label),
        stringFromUtf8(-1, " class "),
        METH_CALL(debugString, valToString(info->name)),
        stringFromUtf8(-1, ">"));
}

// Documented in header.
METH_IMPL_0(Class, debugSymbol) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(Class, gcMark) {
    ClassInfo *info = getInfo(ths);

    datMark(info->name);
    datMark(info->secret);

    for (zint i = 0; i < DAT_MAX_SYMBOLS; i++) {
        datMark(info->methods[i]);
    }

    return NULL;
}

// Documented in header.
METH_IMPL_0(Class, get_name) {
    return getInfo(ths)->name;
}

// Documented in header.
METH_IMPL_0(Class, get_parent) {
    return getInfo(ths)->parent;
}

// Documented in header.
METH_IMPL_1(Class, totalEq, other) {
    assertIsClass(other);  // Note: Not guaranteed to be a `Class`.
    return classEqUnchecked(ths, other) ? ths : NULL;
}

// Documented in header.
METH_IMPL_1(Class, totalOrder, other) {
    if (ths == other) {
        // Easy case to avoid decomposition and detailed tests.
        return INT_0;
    }

    assertIsClass(other);  // Note: Not guaranteed to be a `Class`.
    ClassInfo *info1 = getInfo(ths);
    ClassInfo *info2 = getInfo(other);
    zvalue name1 = info1->name;
    zvalue name2 = info2->name;
    bool core1 = isCoreClass(info1);
    bool core2 = isCoreClass(info2);

    // Compare categories for major order.

    if (core1 && !core2) {
        return INT_NEG1;
    } else if ((!core1) && core2) {
        return INT_1;
    }

    // Compare names for minor order.

    zorder nameOrder = valZorder(name1, name2);
    if (nameOrder != ZSAME) {
        return intFromZint(nameOrder);
    }

    // Names are the same. The order is not defined given two different
    // non-core classes.

    if (core1 || core2) {
        die("Shouldn't happen: Same-name-but-different core classes.");
    }

    return NULL;
}

/**
 * Define `objectModel` as a module, as separate from the `Class` class. This
 * sets up the "corest of the core" classes.
 */
MOD_INIT(objectModel) {
    // Make sure that the "exposed" header is sized the same as the real one.
    if (sizeof(DatHeaderExposed) != sizeof(DatHeader)) {
        die("Mismatched exposed header size: should be %lu",
            sizeof(DatHeader));
    }

    // Set up the "knotted" classes. These are the ones that have circular
    // is-a and/or heritage relationships with each other. This does *not*
    // include setting up `Symbol`, which is why the `name` and `secret` of
    // all of these start out as `NULL`.

    // `NULL` for `parent` here, because the superclass of `Metaclass` is
    // `Class`, and the latter doesn't yet exist. The immediately-following
    // `cls` assignment is to set up the required circular is-a relationship
    // between `Metaclass` and its metaclass.
    CLS_Metaclass = makeClassPair(NULL, NULL, NULL);
    CLS_Metaclass->cls->cls = CLS_Metaclass;

    // Similarly, `NULL` for `parent` here, because the superclass of `Class`
    // is `Value`.
    CLS_Class = makeClassPair(NULL, NULL, NULL);

    // `NULL` for `parent` here, because `Value` per se has no superclass.
    // However, `Value`'s metaclass *does* have a superclass, `Class`, so
    // we assign it explicitly, immediately below.
    CLS_Value = makeClassPair(NULL, NULL, NULL);

    // Finally, set up the missing the heritage relationships.
    getInfo(CLS_Value->cls)->parent = CLS_Class;
    getInfo(CLS_Class)->parent = CLS_Value;
    getInfo(CLS_Class->cls)->parent = CLS_Value->cls;
    getInfo(CLS_Metaclass)->parent = CLS_Class;
    getInfo(CLS_Metaclass->cls)->parent = CLS_Class->cls;

    // With the "knotted" classes taken care of, now do the initial
    // special-case setup of `Core` and `Symbol`. These are required for
    // classes to have `name`s and `secret`s.

    // Construct these with `NULL` `name` and `secret` initially.
    CLS_Core = makeClassPair(NULL, CLS_Value, NULL);
    CLS_Symbol = makeClassPair(NULL, CLS_Core, NULL);

    // With `Symbol` barely initialized, it's now possible to make an
    // instance of it to use as the core library secret.
    theCoreSecret = unlistedSymbolFromUtf8(-1, "coreSecret");
    datImmortalize(theCoreSecret);

    // And to make the prefix used for all metaclasses.
    SYM_INIT(meta_);

    // And with that, initialize `name` and `secret` on all the classes
    // constructed above.
    initEarlyClass(CLS_Class,     symbolFromUtf8(-1, "Class"));
    initEarlyClass(CLS_Core,      symbolFromUtf8(-1, "Core"));
    initEarlyClass(CLS_Metaclass, symbolFromUtf8(-1, "Metaclass"));
    initEarlyClass(CLS_Symbol,    symbolFromUtf8(-1, "Symbol"));
    initEarlyClass(CLS_Value,     symbolFromUtf8(-1, "Value"));

    // Finally, construct the classes that are required in order for
    // methods to be bound to classes.

    CLS_SymbolTable =
        makeClassPair(symbolFromUtf8(-1, "SymbolTable"), CLS_Core, theCoreSecret);
    CLS_Builtin =
        makeClassPair(symbolFromUtf8(-1, "Builtin"), CLS_Core, theCoreSecret);

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
    SYM_INIT(get_parent);

    classBindMethods(CLS_Class,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Class, debugString),
            METH_BIND(Class, debugSymbol),
            METH_BIND(Class, gcMark),
            METH_BIND(Class, get_name),
            METH_BIND(Class, get_parent),
            METH_BIND(Class, totalEq),
            METH_BIND(Class, totalOrder),
            NULL));

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

// Documented in header.
SYM_DEF(get_parent);

// Documented in header.
SYM_DEF(meta_);
