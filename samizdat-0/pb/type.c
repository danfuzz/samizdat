/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/** Next type sequence number to assign. */
static zint theNextId = 0;

/** Array of all existing types. Sorted by index (not name). */
static zvalue theTypes[PB_MAX_TYPES];

/**
 * Payload struct for type `Type`.
 */
typedef struct {
    /**
     * Name of the type. Arbitrary other than that it must be unique
     * among all types.
     */
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

/**
 * Gets a pointer to the value's info.
 */
static TypeInfo *typeInfo(zvalue type) {
    return pbPayload(type);
}

/**
 * Initializes a type value.
 */
static void typeInit(zvalue type, zvalue name, zvalue secret, bool derived) {
    if (theNextId == PB_MAX_TYPES) {
        die("Too many types!");
    }

    TypeInfo *info = typeInfo(type);

    info->name = name;
    info->secret = secret;
    info->id = theNextId;
    info->derived = derived;
    theTypes[theNextId] = type;

    theNextId++;
    pbImmortalize(type);
}

/**
 * Allocates a type value.
 */
static zvalue allocType(void) {
    if (PB_SECRET == NULL) {
        // We end up here during bootstrap.
        return pbAllocValueUnchecked(TYPE_Type, sizeof(TypeInfo));
    } else {
        return pbAllocValue(TYPE_Type, sizeof(TypeInfo));
    }
}

/**
 * Creates an returns a new type.
 */
static zvalue newType(zvalue name, zvalue secret, bool derived) {
    zvalue result = allocType();
    typeInit(result, name, secret, derived);
    return result;
}

/**
 * Asserts that the value is a `Type`.
 */
void assertTypeIsType(zvalue value) {
    assertTypeIs(value, TYPE_Type);
}


/*
 * Module functions
 */

/* Documented in header. */
zint indexFromType(zvalue type) {
    assertTypeIsType(type);
    return typeInfo(type)->id;
}

/* Documented in header. */
zvalue transparentTypeFromName(zvalue name) {
    // TODO: Linear search is probably a bad idea here.
    for (zint i = 0; i < theNextId; i++) {
        zvalue one = theTypes[i];
        TypeInfo *info = typeInfo(one);
        if (info->derived && (info->secret == NULL) && pbEq(info->name, name)) {
            return one;
        }
    }

    zvalue result = newType(name, NULL, true);

    // Bind the default derived value methods.
    gfnBindCore(GFN_eq,     result, Deriv_eq);
    gfnBindCore(GFN_gcMark, result, Deriv_gcMark);
    gfnBindCore(GFN_order,  result, Deriv_order);

    return result;
}

/* Documented in header. */
bool typeSecretIs(zvalue type, zvalue secret) {
    assertTypeIsType(type);
    return pbNullSafeEq(typeInfo(type)->secret, secret);
}


/*
 * Exported functions
 */

/* Documented in header. */
void assertTypeIs(zvalue value, zvalue type) {
    pbAssertValid(value);

    if (!hasType(value, type)) {
        die("Expected type %s; got %s.",
            pbDebugString(type), pbDebugString(value));
    }
}

/* Documented in header. */
zvalue coreTypeFromName(zvalue name) {
    return newType(name, PB_SECRET, false);
}

/* Documented in header. */
bool hasType(zvalue value, zvalue type) {
    return pbEq(typeOf(value), type);
}

/* Documented in header. */
bool typeIsDerived(zvalue type) {
    assertTypeIsType(type);
    TypeInfo *info = typeInfo(type);
    return info->derived;
}

/* Documented in header. */
zvalue typeOf(zvalue value) {
    zvalue type = value->type;
    TypeInfo *info = typeInfo(type);

    // `typeOf` on a transparent type returns its name.
    return (info->secret == NULL) ? info->name : type;
}

/* Documented in header. */
zvalue typeName(zvalue type) {
    assertTypeIsType(type);
    TypeInfo *info = typeInfo(type);
    return info->name;
}


/*
 * Type binding
 */

/* Documented in header. */
zvalue PB_SECRET = NULL;

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
    TypeInfo *info2 = typeInfo(v2);

    if (info1->derived != info2->derived) {
        return info2->derived ? PB_NEG1 : PB_1;
    }

    bool secret1 = info1->secret != NULL;
    bool secret2 = info2->secret != NULL;

    if (secret1 != secret2) {
        return secret2 ? PB_NEG1 : PB_1;
    }

    zorder nameOrder = pbOrder(typeInfo(v1)->name, typeInfo(v2)->name);

    if (nameOrder != ZSAME) {
        return intFromZint(nameOrder);
    }

    // This is the case of two different opaque derived types with the
    // same name.
    return (info1->id < info2->id) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
void pbInitTypeSystem(void) {
    TYPE_Type = allocType();
    TYPE_Type->type = TYPE_Type;
    TYPE_Generic = allocType();
    TYPE_String = allocType();

    // PB_SECRET is defined as a type value with no instances. This is
    // a hackish convenience. It should probably be a Uniqlet.
    PB_SECRET = allocType();

    typeInit(TYPE_Type,    stringFromUtf8(-1, "Type"),    PB_SECRET, false);
    typeInit(TYPE_Generic, stringFromUtf8(-1, "Generic"), PB_SECRET, false);
    typeInit(TYPE_String,  stringFromUtf8(-1, "String"),  PB_SECRET, false);
    typeInit(PB_SECRET,    stringFromUtf8(-1, "SECRET"),  PB_SECRET, false);
}

/* Documented in header. */
void pbBindType(void) {
    gfnBindCore(GFN_debugString, TYPE_Type, Type_debugString);
    gfnBindCore(GFN_gcMark,      TYPE_Type, Type_gcMark);
    gfnBindCore(GFN_order,       TYPE_Type, Type_order);
}

/* Documented in header. */
zvalue TYPE_Type = NULL;

