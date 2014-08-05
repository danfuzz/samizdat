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
#include "type/Uniqlet.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

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
 * Compares an explicit name and secret with a class.
 */
static int classCompare(zvalue name1, zvalue secret1, zvalue v2) {
    ClassInfo *info2 = getInfo(v2);
    zvalue name2 = info2->name;
    zvalue secret2 = info2->secret;
    bool derived1 = (secret1 != theCoreSecret);
    bool derived2 = (secret2 != theCoreSecret);

    if (derived1 != derived2) {
        return derived2 ? ZLESS : ZMORE;
    }

    bool hasSecret1 = (secret1 != NULL);
    bool hasSecret2 = (secret2 != NULL);

    if (hasSecret1 != hasSecret2) {
        return hasSecret2 ? ZLESS : ZMORE;
    }

    zorder nameOrder = valZorder(name1, name2);

    if ((nameOrder != ZSAME) || !hasSecret1) {
        return nameOrder;
    }

    // This is the case of two different opaque derived classes with the
    // same name.
    return valZorder(secret1, secret2);
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
zvalue makeClass(zvalue name, zvalue parent, zvalue secret) {
    assertHasClass(name, CLS_String);
    assertHasClassClass(parent);

    zvalue result = allocClass();
    classInit(result, name, parent, secret);
    return result;
}

// Documented in header.
zvalue makeCoreClass(zvalue name, zvalue parent) {
    assertHasClassClass(parent);

    zvalue result = allocClass();
    classInit(result, name, parent, theCoreSecret);
    return result;
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
    ClassInfo *info = getInfo(value);
    return intFromZint(classCompare(info->name, info->secret, other));
}

/**
 * Define `objectModel` as a module, as separate from the `Class` class.
 */
MOD_INIT(objectModel) {
    CLS_Class = allocClass();
    CLS_Class->cls = CLS_Class;

    CLS_Value       = allocClass();
    CLS_Data        = allocClass();
    CLS_DerivedData = allocClass();
    CLS_Object      = allocClass();

    // The rest are in alphabetical order.
    CLS_Builtin     = allocClass();
    CLS_Generic     = allocClass();
    CLS_Jump        = allocClass();
    CLS_String      = allocClass();
    CLS_Uniqlet     = allocClass();

    theCoreSecret = makeUniqlet();
    datImmortalize(theCoreSecret);

    classInitHere(CLS_Class,       CLS_Value, "Class");
    classInitHere(CLS_Value,       NULL,      "Value");
    classInitHere(CLS_Data,        CLS_Value, "Data");
    classInitHere(CLS_DerivedData, CLS_Data,  "DerivedData");
    classInitHere(CLS_Object,      CLS_Value, "Object");

    classInitHere(CLS_Builtin,     CLS_Value, "Builtin");
    classInitHere(CLS_Generic,     CLS_Value, "Generic");
    classInitHere(CLS_Jump,        CLS_Value, "Jump");
    classInitHere(CLS_String,      CLS_Data,  "String");
    classInitHere(CLS_Uniqlet,     CLS_Value, "Uniqlet");

    // Make sure that the enum constants match up with what got assigned here.
    // If not, `funCall` will break.
    if (classIndex(CLS_Builtin) != DAT_INDEX_BUILTIN) {
        die("Mismatched index for `Builtin`: should be %lld",
            classIndex(CLS_Builtin));
    } else if (classIndex(CLS_Generic) != DAT_INDEX_GENERIC) {
        die("Mismatched index for `Generic`: should be %lld",
            classIndex(CLS_Generic));
    } else if (classIndex(CLS_Jump) != DAT_INDEX_JUMP) {
        die("Mismatched index for `Jump`: should be %lld",
            classIndex(CLS_Jump));
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

    METH_BIND(Class, debugString);
    METH_BIND(Class, gcMark);
    METH_BIND(Class, totalOrder);
}

// Documented in header.
zvalue CLS_Class = NULL;
