// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Data.h"
#include "type/DerivedData.h"
#include "type/Int.h"
#include "type/Jump.h"
#include "type/Object.h"
#include "type/SelectorTable.h"
#include "type/Uniqlet.h"
#include "type/define.h"
#include "util.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/** Constants identifying class category, used when sorting classes. */
typedef enum {
    CORE_CLASS = 0,
    DERIVED_DATA_CLASS,
    OPAQUE_CLASS
} ClassCategory;

/** Next class sequence number to assign. */
static zint theNextClassId = 0;

/** The `secret` value used for all core classes. */
static zvalue theCoreSecret = NULL;


/**
 * Gets a pointer to the value's info.
 */
static ClassInfo *getInfo(zvalue cls) {
    return datPayload(cls);
}

/**
 * Initializes a class value. The class is marked core, except if its parent
 * is `DerivedData` in which case it is marked as derived.
 */
static void classInit(zvalue cls, zvalue name, zvalue parent, zvalue secret) {
    if (theNextClassId == DAT_MAX_CLASSES) {
        die("Too many classes!");
    }

    if ((parent == NULL) && (cls != CLS_Value)) {
        die("Every class but `Value` needs a parent.");
    }

    ClassInfo *info = getInfo(cls);
    bool derived = (parent == CLS_DerivedData) && (CLS_DerivedData != NULL);

    info->parent = parent;
    info->name = name;
    info->secret = secret;
    info->classId = theNextClassId;

    theNextClassId++;
    datImmortalize(cls);
}

/**
 * Helper for initializing the classes built directly in this file.
 */
static void classInitHere(zvalue cls, zvalue parent, const char *name) {
    classInit(cls, stringFromUtf8(-1, name), parent, theCoreSecret);
}

/**
 * Allocates a class value.
 */
static zvalue allocClass(void) {
    return datAllocValue(CLS_Class, sizeof(ClassInfo));
}

/**
 * Gets the category of a class (given its info), for sorting.
 */
static ClassCategory categoryOf(ClassInfo *info) {
    zvalue secret = info->secret;

    if (secret == theCoreSecret) {
        return CORE_CLASS;
    } else if (secret == NULL) {
        return DERIVED_DATA_CLASS;
    } else {
        return OPAQUE_CLASS;
    }
}

/**
 * Returns `true` iff the value is a `Class`.
 */
static bool isClass(zvalue value) {
    // This is a light-weight implementation, since (a) otherwise it consumes
    // a significant amount of runtime with no real benefit, and (b) it
    // avoids infinite recursion.
    return (get_class(value) == CLS_Class);
}

/**
 * Asserts `isClass(value)`.
 */
static void assertHasClassClass(zvalue value) {
    if (!isClass(value)) {
        die("Expected class Class; got %s.", valDebugString(value));
    }
}

/**
 * Like `classEq()` but without checking up-front that the two given values
 * are actually classes. Note that this is just a `==` check, since classes
 * are all unique and effectively "selfish."
 */
static bool classEqUnchecked(zvalue cls1, zvalue cls2) {
    return (cls1 == cls2);
}


//
// Module Definitions
//

// Documented in header.
void classBindMethods(zvalue cls, zvalue classMethods,
        zvalue instanceMethods) {
    ClassInfo *info = getInfo(cls);

    if (info->parent != NULL) {
        // Initialize the instance method table with whatever the parent
        // defined.
        utilCpy(zvalue, info->methods, getInfo(info->parent)->methods,
            DAT_MAX_SELECTORS);
    }

    if (classMethods != NULL) {
        die("No class methods allowed...yet.");
    }

    if (instanceMethods != NULL) {
        zvalue methods[DAT_MAX_SELECTORS];
        arrayFromSelectorTable(methods, instanceMethods);
        for (zint i = 0; i < DAT_MAX_SELECTORS; i++) {
            zvalue one = methods[i];
            if (one != NULL) {
                info->methods[i] = one;
            }
        }
    }
}

// Documented in header.
zvalue classFindMethodBySelectorIndex(zvalue cls, zint index) {
    // TODO: Remove the heritage lookup once subclass tables get populated
    // with their superclasses' methods.

    assertHasClassClass(cls);
    zvalue result = NULL;
    bool superChecked = false;

    zvalue checkCls = cls;
    while (checkCls != NULL) {
        zvalue *methods = getInfo(checkCls)->methods;
        result = methods[index];
        if (result != NULL) {
            break;
        }
        superChecked = true;
        checkCls = classParent(checkCls);
    }

    if (superChecked && (result != NULL)) {
        getInfo(cls)->methods[index] = result;
    }

    return result;
}

// Documented in header.
extern inline zint classIndexUnchecked(zvalue cls);


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
void classAddMethod(zvalue cls, zvalue selector, zvalue function) {
    assertHasClassClass(cls);
    zint index = selectorIndex(selector);
    zvalue *methods = getInfo(cls)->methods;

    methods[index] = function;
}

// Documented in header.
void classAddPrimitiveMethod(zvalue cls, zvalue selector, zint minArgs,
        zint maxArgs, zfunction function, const char *functionName) {
    zvalue name = stringFromUtf8(-1, functionName);
    zvalue builtin = makeBuiltin(minArgs, maxArgs, function, 0, name);
    classAddMethod(cls, selector, builtin);
}

// Documented in header.
bool classEq(zvalue cls1, zvalue cls2) {
    assertHasClassClass(cls1);
    assertHasClassClass(cls1);
    return classEqUnchecked(cls1, cls2);
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
zint classIndex(zvalue cls) {
    assertHasClassClass(cls);
    return classIndexUnchecked(cls);
}

// Documented in header.
bool classIsDerived(zvalue cls) {
    return classParent(cls) == CLS_DerivedData;
}

// Documented in header.
zvalue className(zvalue cls) {
    assertHasClassClass(cls);
    return getInfo(cls)->name;
}

// Documented in header.
zvalue classParent(zvalue cls) {
    assertHasClassClass(cls);
    return getInfo(cls)->parent;
}

// Documented in header.
zint get_classIndex(zvalue value) {
    return classIndexUnchecked(get_class(value));
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
    return makeClass(stringFromUtf8(-1, name), parent, theCoreSecret,
        classMethods, instanceMethods);
}


//
// Class Definition
//

// Documented in header.
METH_IMPL(Class, debugString) {
    zvalue cls = args[0];
    ClassInfo *info = getInfo(cls);
    zvalue extraString;

    if (info->secret == theCoreSecret) {
        return info->name;
    } else if (info->secret != NULL) {
        extraString = stringFromUtf8(-1, " : opaque");
    } else if (classParent(cls) == CLS_DerivedData) {
        extraString = EMPTY_STRING;
    } else {
        die("Shouldn't happen: opaque class without secret.");
    }

    return METH_CALL(cat,
        stringFromUtf8(-1, "@@("),
        METH_CALL(debugString, info->name),
        extraString,
        stringFromUtf8(-1, ")"));
}

// Documented in header.
METH_IMPL(Class, gcMark) {
    zvalue cls = args[0];
    ClassInfo *info = getInfo(cls);

    datMark(info->name);
    datMark(info->secret);

    for (zint i = 0; i < DAT_MAX_SELECTORS; i++) {
        datMark(info->methods[i]);
    }

    return NULL;
}

// Documented in header.
METH_IMPL(Class, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `Class`.

    if (value == other) {
        // Easy case to avoid decomposition and detailed tests.
        return INT_0;
    }

    assertHasClassClass(other);
    ClassInfo *info1 = getInfo(value);
    ClassInfo *info2 = getInfo(other);
    zvalue name1 = info1->name;
    zvalue name2 = info2->name;
    ClassCategory cat1 = categoryOf(info1);
    ClassCategory cat2 = categoryOf(info2);

    // Compare categories for major order.

    if (cat1 < cat2) {
        return INT_NEG1;
    } else if (cat1 > cat2) {
        return INT_1;
    }

    // Compare names for minor order.

    zorder nameOrder = valZorder(name1, name2);
    if (nameOrder != ZSAME) {
        return intFromZint(nameOrder);
    }

    // Names are the same. The order is not defined given two different
    // opaque classes.

    return (cat1 == OPAQUE_CLASS) ? NULL : INT_0;
}

/**
 * Define `objectModel` as a module, as separate from the `Class` class.
 */
MOD_INIT(objectModel) {
    CLS_Class = allocClass();
    CLS_Class->cls = CLS_Class;

    CLS_Value         = allocClass();
    CLS_Selector      = allocClass();
    CLS_SelectorTable = allocClass();
    CLS_Data          = allocClass();
    CLS_DerivedData   = allocClass();
    CLS_Object        = allocClass();

    // The rest are in alphabetical order.
    CLS_Builtin       = allocClass();
    CLS_Jump          = allocClass();
    CLS_String        = allocClass();
    CLS_Uniqlet       = allocClass();

    theCoreSecret = makeUniqlet();
    datImmortalize(theCoreSecret);

    classInitHere(CLS_Class,         CLS_Value, "Class");
    classInitHere(CLS_Value,         NULL,      "Value");
    classInitHere(CLS_Selector,      CLS_Value, "Selector");
    classInitHere(CLS_SelectorTable, CLS_Value, "SelectorTable");
    classInitHere(CLS_Data,          CLS_Value, "Data");
    classInitHere(CLS_DerivedData,   CLS_Data,  "DerivedData");
    classInitHere(CLS_Object,        CLS_Value, "Object");

    classInitHere(CLS_Builtin,       CLS_Value, "Builtin");
    classInitHere(CLS_Jump,          CLS_Value, "Jump");
    classInitHere(CLS_String,        CLS_Data,  "String");
    classInitHere(CLS_Uniqlet,       CLS_Value, "Uniqlet");

    // Make sure that the enum constants match up with what got assigned here.
    // If not, `funCall` will break.
    if (classIndex(CLS_Builtin) != DAT_INDEX_BUILTIN) {
        die("Mismatched index for `Builtin`: should be %lld",
            classIndex(CLS_Builtin));
    } else if (classIndex(CLS_Jump) != DAT_INDEX_JUMP) {
        die("Mismatched index for `Jump`: should be %lld",
            classIndex(CLS_Jump));
    } else if (classIndex(CLS_Selector) != DAT_INDEX_SELECTOR) {
        die("Mismatched index for `Selector`: should be %lld",
            classIndex(CLS_Selector));
    }

    // Make sure that the "fake" header is sized the same as the real one.
    if (DAT_HEADER_SIZE != sizeof(DatHeader)) {
        die("Mismatched value header size: should be %lu", sizeof(DatHeader));
    }
}

/** Initializes the module. */
MOD_INIT(Class) {
    MOD_USE(OneOff);

    // Note: The `objectModel` module (directly above) initializes `CLS_Class`.
    classBindMethods(CLS_Class,
        NULL,
        selectorTableFromArgs(
            SEL_METH(Class, debugString),
            SEL_METH(Class, gcMark),
            SEL_METH(Class, totalOrder),
            NULL));
}

// Documented in header.
zvalue CLS_Class = NULL;
