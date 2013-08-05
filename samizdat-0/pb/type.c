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
 * Payload struct.
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
    theNextId++;
    pbImmortalize(type);
}


/*
 * Module functions
 */

// ---------------------
// BEGIN DEPRECATED CODE

/* Documented in header. */
zint indexFromZtype(ztype type) {
    zint compl = type->seqNumCompl;

    if (compl == 0) {
        if (theNextId == PB_MAX_TYPES) {
            die("Too many types!");
        }

        compl = ~theNextId;
        ((PbType *) type)->seqNumCompl = compl;  // Cast to discard `const`.
        theNextId++;
    }

    return ~compl;
}

/* Documented in header. */
zvalue typeFromZtype(ztype type) {
    zvalue result = type->nameValue;

    if (result == NULL) {
        result = stringFromUtf8(-1, type->name);
        ((PbType *) type)->nameValue = result;  // Cast to discard `const`.
        pbImmortalize(result);
    }

    return result;
}

// END DEPRECATED CODE
// -------------------

/*
 * Exported functions
 */

/* Documented in header. */
void typeAssert(zvalue value) {
    pbAssertType(value, PB_Type);
}

/* Documented in header. */
zvalue typeFrom(zvalue name, zvalue secret) {
    zvalue result = typeFromName(name);

    if (result == NULL) {
        // Need to make a new type.
        result = pbAllocValue(PB_Type, sizeof(TypeInfo));
        typeInit(result, name, secret);
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
zvalue typeName(zvalue type) {
    typeAssert(type);
    TypeInfo *info = typeInfo(type);
    return info->name;
}

/* Documented in header. */
bool pbCoreTypeIs(zvalue value, ztype type) {
    return value->type == type;
}


/*
 * Type binding
 */

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
    TYPE_Type = pbAllocTypeType(sizeof(TypeInfo));
    TYPE_String = pbAllocValue(TYPE_Type, sizeof(TypeInfo));
    typeInit(TYPE_Type, stringFromUtf8(-1, "Type"), NULL);
    typeInit(TYPE_String, stringFromUtf8(-1, "String"), NULL);
}

/* Documented in header. */
void pbBindType(void) {
    gfnBindCore(GFN_gcMark, PB_Type, Type_gcMark);
    gfnBindCore(GFN_order,  PB_Type, Type_order);
}

/* Documented in header. */
static PbType INFO_Type = {
    .name = "Type"
};
ztype PB_Type = &INFO_Type;
zvalue TYPE_Type = NULL;
