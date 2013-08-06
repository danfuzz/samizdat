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
 * Finds a pre-existing type, given the name.
 */
static zvalue typeFromName(zvalue name) {
    for (zint i = 0; i < theNextId; i++) {
        zvalue one = theTypes[i];
        if (pbEq(name, typeInfo(one)->name)) {
            return one;
        }
    }

    return NULL;
}

/**
 * Initializes a type value.
 */
static void typeInit(zvalue type, zvalue name, zvalue secret) {
    if (theNextId == PB_MAX_TYPES) {
        die("Too many types!");
    }

    TypeInfo *info = typeInfo(type);

    info->name = name;
    info->secret = secret;
    info->id = theNextId;
    theTypes[theNextId] = type;

    theNextId++;
    pbImmortalize(type);
}


/*
 * Module functions
 */

/* Documented in header. */
zint indexFromType(zvalue type) {
    typeAssert(type);
    return typeInfo(type)->id;
}

/* Documented in header. */
bool typeSecretIs(zvalue type, zvalue secret) {
    typeAssert(type);
    return pbNullSafeEq(typeInfo(type)->secret, secret);
}


/*
 * Exported functions
 */

/* Documented in header. */
void pbAssertType(zvalue value, zvalue type) {
    pbAssertValid(value);

    if (!pbTypeIs(value, type)) {
        die("Expected type %s; got %s.",
            pbDebugString(type), pbDebugString(value));
    }
}

/* Documented in header. */
void typeAssert(zvalue value) {
    pbAssertType(value, TYPE_Type);
}

/* Documented in header. */
zvalue typeFrom(zvalue name, zvalue secret) {
    zvalue result = typeFromName(name);

    if (result == NULL) {
        // Need to make a new type.
        result = pbAllocValue(TYPE_Type, sizeof(TypeInfo));
        typeInit(result, name, secret);
        if (secret == NULL) {
            // Bind the default transparent value methods.
            gfnBindCore(GFN_dataOf, result, Transparent_dataOf);
            gfnBindCore(GFN_gcMark, result, Transparent_gcMark);
            gfnBindCore(GFN_order, result, Transparent_order);
        }
    } else {
        // Need to verify that the secret matches.
        zvalue alreadySecret = typeInfo(result)->secret;
        if (!pbNullSafeEq(secret, alreadySecret)) {
            die("Mismatched type secrets.");
        }
    }

    return result;
}

/* Documented in header. */
bool pbTypeIs(zvalue value, zvalue type) {
    return pbEq(pbTypeOf(value), type);
}

/* Documented in header. */
zvalue pbTypeOf(zvalue value) {
    zvalue type = value->type;
    TypeInfo *info = typeInfo(type);

    // `typeOf` on a transparent type returns its name.
    return (info->secret == NULL) ? info->name : type;
}

/* Documented in header. */
zvalue typeName(zvalue type) {
    typeAssert(type);
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

    if (info->secret != NULL) {
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
    return intFromZint(pbOrder(typeInfo(v1)->name, typeInfo(v2)->name));
}

/* Documented in header. */
void pbInitTypeSystem(void) {
    TYPE_Type = pbAllocValueUnchecked(NULL, sizeof(TypeInfo));
    TYPE_Type->type = TYPE_Type;
    TYPE_Generic = pbAllocValueUnchecked(TYPE_Type, sizeof(TypeInfo));
    TYPE_String = pbAllocValueUnchecked(TYPE_Type, sizeof(TypeInfo));

    // PB_SECRET is defined as a type value with no instances. This is
    // a hackish convenience. It should probably be a Uniqlet.
    PB_SECRET = pbAllocValueUnchecked(TYPE_Type, sizeof(TypeInfo));

    typeInit(TYPE_Type,    stringFromUtf8(-1, "Type"),    PB_SECRET);
    typeInit(TYPE_Generic, stringFromUtf8(-1, "Generic"), PB_SECRET);
    typeInit(TYPE_String,  stringFromUtf8(-1, "String"),  PB_SECRET);
    typeInit(PB_SECRET,    stringFromUtf8(-1, "SECRET"),  PB_SECRET);
}

/* Documented in header. */
void pbBindType(void) {
    gfnBindCore(GFN_debugString, TYPE_Type, Type_debugString);
    gfnBindCore(GFN_gcMark,      TYPE_Type, Type_gcMark);
    gfnBindCore(GFN_order,       TYPE_Type, Type_order);
}

/* Documented in header. */
zvalue TYPE_Type = NULL;

