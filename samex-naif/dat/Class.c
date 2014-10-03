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
 * Asserts that `value` is an instance of `Class`, that is that
 * `hasClass(value, CLS_Class)` is true.
 */
static void assertHasClassClass(zvalue value) {
    if (!classEqUnchecked(get_class(value), CLS_Class)) {
        die("Expected class Class; got %s.", valDebugString(value));
    }
}

/**
 * Initializes a class value.
 */
static void classInit(zvalue cls, zvalue name, zvalue parent, zvalue secret) {
    assertHasClass(name, CLS_Symbol);

    if ((parent == NULL) && !classEqUnchecked(cls, CLS_Value)) {
        die("Every class but `Value` needs a parent.");
    }

    ClassInfo *info = getInfo(cls);
    info->parent = parent;
    info->name = name;
    info->secret = secret;

    datImmortalize(cls);
}

/**
 * Helper for initializing the classes built directly in this file.
 */
static void classInitHere(zvalue cls, zvalue parent, const char *name) {
    classInit(cls, symbolFromUtf8(-1, name), parent, theCoreSecret);
}

/**
 * Allocates a class value.
 */
static zvalue allocClass(void) {
    return datAllocValue(CLS_Class, sizeof(ClassInfo));
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
    ClassInfo *info = getInfo(cls);
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

    if (info->parent != NULL) {
        // Initialize the instance method table with whatever the parent
        // defined.
        utilCpy(zvalue, info->methods, getInfo(info->parent)->methods,
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
            info->methods[index] = methods[i].value;
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
    assertHasClassClass(cls);
    assertHasClassClass(parent);
    return classEqUnchecked(getInfo(cls)->parent, parent);
}

// Documented in header.
bool classHasSecret(zvalue cls, zvalue secret) {
    assertHasClassClass(cls);

    ClassInfo *info = getInfo(cls);

    // Note: It's important to pass `info->secret` first, so that it's the
    // one whose `totalEq` method is used. The given `secret` can't be
    // trusted to behave.
    return valEq(info->secret, secret);
}

// Documented in header.
bool hasClass(zvalue value, zvalue cls) {
    assertHasClassClass(cls);

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
    assertHasClassClass(parent);

    zvalue result = allocClass();
    ClassInfo *info = getInfo(result);
    classInit(result, name, parent, secret);
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
METH_IMPL_1(Class, totalOrder, other) {
    if (ths == other) {
        // Easy case to avoid decomposition and detailed tests.
        return INT_0;
    }

    assertHasClassClass(other);  // Note: Not guaranteed to be a `Class`.
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
 * Define `objectModel` as a module, as separate from the `Class` class.
 */
MOD_INIT(objectModel) {
    // Make sure that the "exposed" header is sized the same as the real one.
    if (sizeof(DatHeaderExposed) != sizeof(DatHeader)) {
        die("Mismatched exposed header size: should be %lu",
            sizeof(DatHeader));
    }

    CLS_Class = allocClass();
    CLS_Class->cls = CLS_Class;

    CLS_Value       = allocClass();
    CLS_Core        = allocClass();
    CLS_Symbol      = allocClass();
    CLS_SymbolTable = allocClass();
    CLS_Builtin     = allocClass();

    theCoreSecret = unlistedSymbolFromUtf8(-1, "coreSecret");
    datImmortalize(theCoreSecret);

    classInitHere(CLS_Class,       CLS_Value, "Class");
    classInitHere(CLS_Value,       NULL,      "Value");
    classInitHere(CLS_Core,        CLS_Value, "Core");
    classInitHere(CLS_Symbol,      CLS_Value, "Symbol");
    classInitHere(CLS_SymbolTable, CLS_Value, "SymbolTable");
    classInitHere(CLS_Builtin,     CLS_Value, "Builtin");
}

/** Initializes the module. */
MOD_INIT(Class) {
    MOD_USE(Value);

    SYM_INIT(get_parent);

    // Note: The `objectModel` module (directly above) initializes `CLS_Class`.
    classBindMethods(CLS_Class,
        NULL,
        symbolTableFromArgs(
            METH_BIND(Class, debugString),
            METH_BIND(Class, gcMark),
            METH_BIND(Class, get_name),
            METH_BIND(Class, get_parent),
            METH_BIND(Class, totalOrder),
            NULL));
}

// Documented in header.
zvalue CLS_Class = NULL;

// Documented in header.
SYM_DEF(get_parent);
