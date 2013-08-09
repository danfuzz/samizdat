/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stdlib.h>


/*
 * Helper definitions
 */

/**
 * Payload struct for type `Type`.
 */
typedef struct {
    /** Parent type. Only allowed to be `NULL` for `Value`. */
    zvalue parent;

    /** Name of the type. Arbitrary value. */
    zvalue name;

    /** Access secret of the type. Optional, and arbitrary if present. */
    zvalue secret;

    /** Whether the type is derived. `false` indicates a core type. */
    bool derived;

    /**
     * Type identifier / index. Assigned upon initialization, in sequential
     * order.
     */
    zint id;
} TypeInfo;

/** Next type sequence number to assign. */
static zint theNextId = 0;

/** Array of all existing types, in sort order (possibly stale). */
static zvalue theTypes[PB_MAX_TYPES];

/** Whether `theTypes` needs a sort. */
static bool theNeedSort = true;

/** The `secret` value used for all core types. */
static zvalue coreSecret = NULL;


/**
 * Gets a pointer to the value's info.
 */
static TypeInfo *typeInfo(zvalue type) {
    return pbPayload(type);
}

/**
 * Initializes a type value.
 */
static void typeInit(zvalue type, zvalue parent, zvalue name, zvalue secret) {
    if (theNextId == PB_MAX_TYPES) {
        die("Too many types!");
    }

    if ((parent == NULL) && (type != TYPE_Value)) {
        die("Every type but `Value` needs a parent.");
    }

    TypeInfo *info = typeInfo(type);

    info->parent = parent;
    info->name = name;
    info->secret = secret;
    info->id = theNextId;
    info->derived = (secret != coreSecret);

    theTypes[theNextId] = type;
    theNeedSort = true;
    theNextId++;
    pbImmortalize(type);
}

/**
 * Allocates a type value.
 */
static zvalue allocType(void) {
    return pbAllocValue(TYPE_Type, sizeof(TypeInfo));
}

/**
 * Creates and returns a new type with the given name and secret. The type
 * is marked derived *unless* the given secret is `coreSecret`.
 */
static zvalue newType(zvalue name, zvalue secret) {
    zvalue result = allocType();
    typeInit(result, TYPE_Value, name, secret);
    return result;
}

/**
 * Compares an explicit name and secret with a type. Common function used
 * for searching, sorting, and ordering.
 */
static int typeCompare(zvalue name1, zvalue secret1, zvalue v2) {
    TypeInfo *info2 = typeInfo(v2);
    zvalue name2 = info2->name;
    zvalue secret2 = info2->secret;
    bool derived1 = (secret1 != coreSecret);
    bool derived2 = (secret2 != coreSecret);

    if (derived1 != derived2) {
        return derived2 ? ZLESS : ZMORE;
    }

    bool hasSecret1 = (secret1 != NULL);
    bool hasSecret2 = (secret2 != NULL);

    if (hasSecret1 != hasSecret2) {
        return hasSecret2 ? ZLESS : ZMORE;
    }

    zorder nameOrder = pbOrder(name1, name2);

    if (nameOrder != ZSAME) {
        return nameOrder;
    }

    // This is the case of two different opaque derived types with the
    // same name.
    if (secret1 == secret2) {
        // Handles the case of both `NULL`.
        return ZSAME;
    } else {
        return pbOrder(secret1, secret2);
    }
}

/**
 * Compares two types. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue v1 = *(zvalue *) vptr1;
    zvalue v2 = *(zvalue *) vptr2;
    TypeInfo *info1 = typeInfo(v1);

    return typeCompare(info1->name, info1->secret, v2);
}

/**
 * Compares a name/secret pair with a type. Used for searching.
 */
static int searchOrder(const void *key, const void *vptr) {
    zvalue *searchFor = (zvalue *) key;
    zvalue name = searchFor[0];
    zvalue secret = searchFor[1];

    return typeCompare(name, secret, *(zvalue *) vptr);
}

/**
 * Finds an existing type with the given name and secret, if any.
 */
static zvalue findType(zvalue name, zvalue secret) {
    if (theNeedSort) {
        if (PB_1 == NULL) {
            // The system isn't yet booted enough to have ints. Therefore,
            // sorting and searching won't work, but more to the point, we
            // know we'll only be getting new types anyway.
            return NULL;
        }
        mergesort(theTypes, theNextId, sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue searchFor[2] = { name, secret };
    zvalue *found = (zvalue *)
        bsearch(searchFor, theTypes, theNextId, sizeof(zvalue), searchOrder);

    return (found == NULL) ? NULL : *found;
}

/**
 * Returns `true` iff the value is a `Type`.
 */
static bool isType(zvalue value) {
    // This is a light-weight implementation, since (a) otherwise it consumes
    // a significant amount of runtime with no real benefit, and (b) it
    // avoids infinite recursion.
    return (value->type == TYPE_Type);
}

/**
 * Asserts that the value is a `Type`.
 */
static void assertHasTypeType(zvalue value) {
    if (!isType(value)) {
        // Upon failure, use the regular implementation to produce the error.
        assertHasType(value, TYPE_Type);
    }
}


/*
 * Module functions
 */

/* Documented in header. */
zint indexFromType(zvalue type) {
    assertHasTypeType(type);
    return typeInfo(type)->id;
}

/* Documented in header. */
zvalue transparentTypeFromName(zvalue name) {
    zvalue result = findType(name, NULL);

    if (result == NULL) {
        result = newType(name, NULL);

        // Bind the default derived value methods.
        gfnBindCore(GFN_eq,     result, Deriv_eq);
        gfnBindCore(GFN_gcMark, result, Deriv_gcMark);
        gfnBindCore(GFN_order,  result, Deriv_order);
    }

    return result;
}

/* Documented in header. */
zvalue trueTypeOf(zvalue value) {
    zvalue type = value->type;

    if (!isType(type)) {
        type = transparentTypeFromName(type);
        value->type = type;
    }

    return type;
}

/* Documented in header. */
bool typeIsDerived(zvalue type) {
    return isType(type) ? typeInfo(type)->derived : true;
}

/* Documented in header. */
bool typeSecretIs(zvalue type, zvalue secret) {
    zvalue typeSecret = isType(type) ? typeInfo(type)->secret : NULL;
    return pbEq(typeSecret, secret);
}


/*
 * Exported functions
 */

/* Documented in header. */
void assertHasType(zvalue value, zvalue type) {
    // This tries doing `!=` a first test, to keep the usual case speedy.
    if (   (value != NULL)
        && (value->type != type)
        && !hasType(value, type)) {
        die("Expected type %s; got %s.",
            pbDebugString(type), pbDebugString(value));
    }
}

/* Documented in header. */
void assertHaveSameType(zvalue v1, zvalue v2) {
    pbAssertValid(v1);
    pbAssertValid(v2);

    if (!haveSameType(v1, v2)) {
        die("Mismatched types: %s, %s",
            pbDebugString(v1), pbDebugString(v2));
    }
}

/* Documented in header. */
zvalue coreTypeFromName(zvalue name) {
    zvalue result = findType(name, coreSecret);

    if (result == NULL) {
        result = newType(name, coreSecret);
    }

    return result;
}

/* Documented in header. */
bool hasType(zvalue value, zvalue type) {
    return pbEq(typeOf(value), type);
}

/* Documented in header. */
bool haveSameType(zvalue v1, zvalue v2) {
    // Use `==` to handle the common cases quickly.
    if (v1->type == v2->type) {
        return true;
    } else {
        return pbEq(typeOf(v1), typeOf(v2));
    }
}

/* Documented in header. */
zvalue typeName(zvalue type) {
    return isType(type) ? typeInfo(type)->name : type;
}

/* Documented in header. */
zvalue typeOf(zvalue value) {
    pbAssertValid(value);

    zvalue type = value->type;
    if (isType(type)) {
        TypeInfo *info = typeInfo(type);
        // `typeOf` on a transparent type returns its name.
        return (info->secret == NULL) ? info->name : type;
    } else {
        // It is a transparent type.
        return type;
    }
}

/* Documented in header. */
zvalue typeParent(zvalue type) {
    return isType(type) ? typeInfo(type)->parent : TYPE_Value;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Type_debugString(zvalue state,
        zint argCount, const zvalue *args) {
    zvalue type = args[0];
    TypeInfo *info = typeInfo(type);

    zvalue result = stringFromUtf8(-1, "@(Type ");
    result = stringAdd(result, fnCall(GFN_debugString, 1, &info->name));

    if (!info->derived) {
        result = stringAdd(result, stringFromUtf8(-1, " /*core*/"));
    } else if (info->secret != NULL) {
        result = stringAdd(result, stringFromUtf8(-1, " /*opaque*/"));
    }

    result = stringAdd(result, stringFromUtf8(-1, ")"));
    return result;
}

/* Documented in header. */
static zvalue Type_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue box = args[0];
    TypeInfo *info = typeInfo(box);

    pbMark(info->name);
    pbMark(info->secret);

    return NULL;
}

/* Documented in header. */
static zvalue Type_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    TypeInfo *info1 = typeInfo(v1);

    return intFromZint(typeCompare(info1->name, info1->secret, v2));
}

/* Documented in header. */
void pbInitTypeSystem(void) {
    TYPE_Type = allocType();
    TYPE_Type->type = TYPE_Type;
    TYPE_Value = allocType();
    TYPE_Function = allocType();
    TYPE_Generic = allocType();
    TYPE_String = allocType();

    // `coreSecret` is defined as a type value with no instances. This is
    // a hackish convenience. It should probably be a Uniqlet.
    coreSecret = allocType();

    typeInit(TYPE_Type,     TYPE_Value, stringFromUtf8(-1, "Type"),     coreSecret);
    typeInit(TYPE_Value,    NULL,       stringFromUtf8(-1, "Value"),    coreSecret);
    typeInit(TYPE_Function, TYPE_Value, stringFromUtf8(-1, "Function"), coreSecret);
    typeInit(TYPE_Generic,  TYPE_Value, stringFromUtf8(-1, "Generic"),  coreSecret);
    typeInit(TYPE_String,   TYPE_Value, stringFromUtf8(-1, "String"),   coreSecret);
    typeInit(coreSecret,    TYPE_Value, stringFromUtf8(-1, "SECRET"),   coreSecret);

    // Make sure that the enum constants match up with what got assigned here.
    // If not, `fnCall` will break.
    if (indexFromType(TYPE_Function) != PB_INDEX_FUNCTION) {
        die("Mismatched index for `Function`: should be %lld",
            indexFromType(TYPE_Function));
    } else if (indexFromType(TYPE_Generic) != PB_INDEX_GENERIC) {
        die("Mismatched index for `Generic`: should be %lld",
            indexFromType(TYPE_Generic));
    }

    // Make sure that the "fake" header is sized the same as the real one.
    if (sizeof(PbFakeHeader) != sizeof(PbHeader)) {
        die("Mismatched header size: should be %lu", sizeof(PbHeader));
    }
}

/* Documented in header. */
void pbBindType(void) {
    gfnBindCore(GFN_debugString, TYPE_Type, Type_debugString);
    gfnBindCore(GFN_gcMark,      TYPE_Type, Type_gcMark);
    gfnBindCore(GFN_order,       TYPE_Type, Type_order);
}

/* Documented in header. */
zvalue TYPE_Type = NULL;
