/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Function.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Uniqlet.h"
#include "type/Value.h"
#include "zlimits.h"

#include <stdlib.h>


/*
 * Private Definitions
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
    bool derived : 1;

    /**
     * Whether the type is "identified". `true` indicates that
     * `valIdentityOf` will work on values of the type.
     */
    bool identified : 1;

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
static TypeInfo *getInfo(zvalue type) {
    return pbPayload(type);
}

/**
 * Initializes a type value.
 */
static void typeInit(zvalue type, zvalue parent, zvalue name, zvalue secret,
        bool identified) {
    if (theNextId == PB_MAX_TYPES) {
        die("Too many types!");
    }

    if ((parent == NULL) && (type != TYPE_Value)) {
        die("Every type but `Value` needs a parent.");
    }

    TypeInfo *info = getInfo(type);

    info->parent = parent;
    info->name = name;
    info->secret = secret;
    info->id = theNextId;
    info->derived = (secret != coreSecret);
    info->identified = identified;

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
static zvalue makeType(zvalue name, zvalue secret, bool identified) {
    zvalue result = allocType();
    typeInit(result, TYPE_Value, name, secret, identified);
    return result;
}

/**
 * Compares an explicit name and secret with a type. Common function used
 * for searching, sorting, and ordering.
 */
static int typeCompare(zvalue name1, zvalue secret1, zvalue v2) {
    TypeInfo *info2 = getInfo(v2);
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

    zorder nameOrder = valOrder(name1, name2);

    if ((nameOrder != ZSAME) || !hasSecret1) {
        return nameOrder;
    }

    // This is the case of two different opaque derived types with the
    // same name.
    return valOrder(secret1, secret2);
}

/**
 * Compares two types. Used for sorting.
 */
static int sortOrder(const void *vptr1, const void *vptr2) {
    zvalue v1 = *(zvalue *) vptr1;
    zvalue v2 = *(zvalue *) vptr2;
    TypeInfo *info1 = getInfo(v1);

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
 * Returns the `Type` per se for the given in-model "type", the latter which
 * may be an arbitrary value representing a transparent type by name.
 */
static zvalue trueTypeFromTypeOrName(zvalue typeOrName) {
    if (isType(typeOrName)) {
        return typeOrName;
    }

    zvalue result = findType(typeOrName, NULL);

    if (result == NULL) {
        result = makeType(typeOrName, NULL, false);
        derivBind(result);
    }

    return result;
}

/**
 * Compares two types (per se) for equality. This is just `==` since
 * types are all "identified".
 */
static bool typeEq(zvalue type1, zvalue type2) {
    return (type1 == type2);
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
 * Module Definitions
 */

/* Documented in header. */
zint indexFromTrueType(zvalue type) {
    assertHasTypeType(type);
    return getInfo(type)->id;
}

/* Documented in header. */
zvalue trueTypeOf(zvalue value) {
    return value->type;
}

/* Documented in header. */
zvalue typeFromTypeAndSecret(zvalue typeOrName, zvalue secret) {
    zvalue type = trueTypeFromTypeOrName(typeOrName);
    return typeSecretIs(type, secret) ? type : NULL;
}

/* Documented in header. */
bool typeIsDerived(zvalue type) {
    return isType(type) ? getInfo(type)->derived : true;
}

/* Documented in header. */
bool typeSecretIs(zvalue type, zvalue secret) {
    zvalue typeSecret = isType(type) ? getInfo(type)->secret : NULL;
    return valEq(typeSecret, secret);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
void assertAllHaveSameType(zint argCount, const zvalue *args) {
    if (argCount == 0) {
        // Trivially true.
        return;
    }

    zvalue arg0 = args[0];
    assertValid(arg0);
    zvalue type0 = arg0->type;

    for (zint i = 1; i < argCount; i++) {
        zvalue one = args[i];
        assertValid(one);
        if (!typeEq(type0, one->type)) {
            die("Mismatched types: %s, %s",
                valDebugString(arg0), valDebugString(one));
        }
    }
}

/* Documented in header. */
void assertHasType(zvalue value, zvalue type) {
    assertValid(value);
    if (!hasType(value, type)) {
        die("Expected type %s; got %s.",
            valDebugString(type), valDebugString(value));
    }
}

/* Documented in header. */
void assertHaveSameType(zvalue v1, zvalue v2) {
    assertValid(v1);
    assertValid(v2);

    if (!haveSameType(v1, v2)) {
        die("Mismatched types: %s, %s",
            valDebugString(v1), valDebugString(v2));
    }
}

/* Documented in header. */
zvalue coreTypeFromName(zvalue name, bool identified) {
    zvalue result = findType(name, coreSecret);

    if (result == NULL) {
        result = makeType(name, coreSecret, identified);
    } else if (identified != getInfo(result)->identified) {
        die("Mismatch on `identified`.");
    }

    return result;
}

/* Documented in header. */
bool hasType(zvalue value, zvalue type) {
    return typeEq(value->type, trueTypeFromTypeOrName(type));
}

/* Documented in header. */
bool haveSameType(zvalue v1, zvalue v2) {
    return typeEq(v1->type, v2->type);
}

/* Documented in header. */
extern void *pbPayload(zvalue value);

/* Documented in header. */
zint typeIndex(zvalue typeOrName) {
    return indexFromTrueType(trueTypeFromTypeOrName(typeOrName));
}

/* Documented in header. */
zint typeIndexOf(zvalue value) {
    return indexFromTrueType(value->type);
}

/* Documented in header. */
bool typeIsIdentified(zvalue type) {
    if (!isType(type)) {
        // Transparent types are not identified.
        return false;
    }

    return getInfo(type)->identified;
}

/* Documented in header. */
zvalue typeName(zvalue type) {
    return isType(type) ? getInfo(type)->name : type;
}

/* Documented in header. */
zvalue typeOf(zvalue value) {
    assertValid(value);

    zvalue type = value->type;
    TypeInfo *info = getInfo(type);

    // `typeOf` on a transparent type returns its name.
    return (info->secret == NULL) ? info->name : type;
}

/* Documented in header. */
zvalue typeParent(zvalue type) {
    return isType(type) ? getInfo(type)->parent : TYPE_Value;
}


/*
 * Type Definition
 */

/* Documented in header. */
METH_IMPL(Type, debugString) {
    zvalue type = args[0];
    TypeInfo *info = getInfo(type);
    zvalue extraString;

    if (!info->derived) {
        extraString = stringFromUtf8(-1, " /*core*/");
    } else if (info->secret != NULL) {
        extraString = stringFromUtf8(-1, " /*opaque*/");
    } else {
        extraString = EMPTY_STRING;
    }

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@(Type "),
        GFN_CALL(debugString, info->name),
        extraString,
        stringFromUtf8(-1, ")"));
}

/* Documented in header. */
METH_IMPL(Type, gcMark) {
    zvalue type = args[0];
    TypeInfo *info = getInfo(type);

    pbMark(info->name);
    pbMark(info->secret);

    return NULL;
}

/* Documented in header. */
METH_IMPL(Type, perOrder) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];

    if (v1 == v2) {
        // Easy case to avoid decomposition and detailed tests.
        return PB_0;
    }

    TypeInfo *info1 = getInfo(v1);
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
    TYPE_Uniqlet = allocType();

    coreSecret = makeUniqlet();
    pbImmortalize(coreSecret);

    typeInit(TYPE_Type,     TYPE_Value, stringFromUtf8(-1, "Type"),     coreSecret, false);
    typeInit(TYPE_Value,    NULL,       stringFromUtf8(-1, "Value"),    coreSecret, false);
    typeInit(TYPE_Function, TYPE_Value, stringFromUtf8(-1, "Function"), coreSecret, true);
    typeInit(TYPE_Generic,  TYPE_Value, stringFromUtf8(-1, "Generic"),  coreSecret, true);
    typeInit(TYPE_String,   TYPE_Value, stringFromUtf8(-1, "String"),   coreSecret, false);
    typeInit(TYPE_Uniqlet,  TYPE_Value, stringFromUtf8(-1, "Uniqlet"),  coreSecret, true);

    // Make sure that the enum constants match up with what got assigned here.
    // If not, `funCall` will break.
    if (indexFromTrueType(TYPE_Function) != PB_INDEX_FUNCTION) {
        die("Mismatched index for `Function`: should be %lld",
            indexFromTrueType(TYPE_Function));
    } else if (indexFromTrueType(TYPE_Generic) != PB_INDEX_GENERIC) {
        die("Mismatched index for `Generic`: should be %lld",
            indexFromTrueType(TYPE_Generic));
    }

    // Make sure that the "fake" header is sized the same as the real one.
    if (PB_HEADER_SIZE != sizeof(PbHeader)) {
        die("Mismatched value header size: should be %lu", sizeof(PbHeader));
    }
}

/* Documented in header. */
void pbBindType(void) {
    METH_BIND(Type, debugString);
    METH_BIND(Type, gcMark);
    METH_BIND(Type, perOrder);
}

/* Documented in header. */
zvalue TYPE_Type = NULL;
