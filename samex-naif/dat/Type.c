// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Builtin.h"
#include "type/DerivedData.h"
#include "type/Generic.h"
#include "type/Data.h"
#include "type/Int.h"
#include "type/Jump.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Uniqlet.h"
#include "type/Value.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/** Next type sequence number to assign. */
static zint theNextTypeId = 0;

/** Array of all existing types, in sort order (possibly stale). */
static zvalue theTypes[DAT_MAX_TYPES];

/** Whether `theTypes` needs a sort. */
static bool theNeedSort = true;

/** The `secret` value used for all core types. */
static zvalue coreSecret = NULL;


/**
 * Gets a pointer to the value's info.
 */
static TypeInfo *getInfo(zvalue type) {
    return datPayload(type);
}

/**
 * Initializes a type value. The type is marked core, except if its parent is
 * `DerivedData` in which case it is marked as derived.
 */
static void typeInit(zvalue type, zvalue parent, zvalue name, bool selfish) {
    if (theNextTypeId == DAT_MAX_TYPES) {
        die("Too many types!");
    }

    if ((parent == NULL) && (type != TYPE_Value)) {
        die("Every type but `Value` needs a parent.");
    }

    TypeInfo *info = getInfo(type);
    bool derived = (parent == TYPE_DerivedData) && (TYPE_DerivedData != NULL);

    info->parent = parent;
    info->name = name;
    info->secret = derived ? NULL : coreSecret;
    info->typeId = theNextTypeId;
    info->derived = derived;
    info->selfish = selfish;

    theTypes[theNextTypeId] = type;
    theNeedSort = true;
    theNextTypeId++;
    datImmortalize(type);
}

/**
 * Allocates a type value.
 */
static zvalue allocType(void) {
    return datAllocValue(TYPE_Type, sizeof(TypeInfo));
}

/**
 * Creates and returns a new type with the given name. The type is marked
 * core, except if its parent is `DerivedData` in which case it is marked as
 * derived.
 */
static zvalue makeType(zvalue name, zvalue parent, bool selfish) {
    zvalue result = allocType();
    typeInit(result, parent, name, selfish);
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

    zorder nameOrder = valZorder(name1, name2);

    if ((nameOrder != ZSAME) || !hasSecret1) {
        return nameOrder;
    }

    // This is the case of two different opaque derived types with the
    // same name.
    return valZorder(secret1, secret2);
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
        if (INT_1 == NULL) {
            // The system isn't yet booted enough to have ints. Therefore,
            // sorting and searching won't work, but more to the point, we
            // know we'll only be getting new types anyway.
            return NULL;
        }
        mergesort(theTypes, theNextTypeId, sizeof(zvalue), sortOrder);
        theNeedSort = false;
    }

    zvalue searchFor[2] = { name, secret };
    zvalue *found = (zvalue *) bsearch(
        searchFor, theTypes, theNextTypeId, sizeof(zvalue), searchOrder);

    return (found == NULL) ? NULL : *found;
}

/**
 * Returns `true` iff the value is a `Type`.
 */
static bool isType(zvalue value) {
    // This is a light-weight implementation, since (a) otherwise it consumes
    // a significant amount of runtime with no real benefit, and (b) it
    // avoids infinite recursion.
    return (get_type(value) == TYPE_Type);
}

/**
 * Asserts `isType(value)`.
 */
static void assertHasTypeType(zvalue value) {
    if (!isType(value)) {
        die("Expected type Type; got %s.", valDebugString(value));
    }
}

/**
 * Compares two types (per se) for equality. This is just `==` since
 * types are all "selfish."
 */
static bool typeEq(zvalue type1, zvalue type2) {
    return (type1 == type2);
}


//
// Module Definitions
//

// Documented in header.
extern inline zint typeIndexUnchecked(zvalue type);


//
// Exported Definitions
//

// Documented in header.
void assertHasType(zvalue value, zvalue type) {
    if (!hasType(value, type)) {
        die("Expected type %s; got %s.",
            valDebugString(type), valDebugString(value));
    }
}

// Documented in header.
zint get_typeIndex(zvalue value) {
    return typeIndexUnchecked(get_type(value));
}

// Documented in header.
bool hasType(zvalue value, zvalue type) {
    assertHasTypeType(type);

    for (zvalue valueType = get_type(value);
            valueType != NULL;
            valueType = getInfo(valueType)->parent) {
        if (typeEq(valueType, type)) {
            return true;
        }
    }

    return false;
}

// Documented in header.
bool haveSameType(zvalue value, zvalue other) {
    return typeEq(get_type(value), get_type(other));
}

// Documented in header.
zvalue makeCoreType(zvalue name, zvalue parent, bool selfish) {
    if (findType(name, coreSecret) != NULL) {
        die("Core type already created.");
    }

    return makeType(name, parent, selfish);
}

// Documented in header.
zvalue makeDerivedDataType(zvalue name) {
    zvalue result = findType(name, NULL);

    if (result == NULL) {
        result = makeType(name, TYPE_DerivedData, false);
    }

    return result;
}

// Documented in header.
zint typeIndex(zvalue type) {
    assertHasTypeType(type);
    return typeIndexUnchecked(type);
}

// Documented in header.
bool typeIsDerived(zvalue type) {
    assertHasTypeType(type);
    return getInfo(type)->derived;
}

// Documented in header.
bool typeIsSelfish(zvalue type) {
    assertHasTypeType(type);
    return getInfo(type)->selfish;
}

// Documented in header.
zvalue typeName(zvalue type) {
    assertHasTypeType(type);
    return getInfo(type)->name;
}

// Documented in header.
zvalue typeParent(zvalue type) {
    assertHasTypeType(type);
    return getInfo(type)->parent;
}


//
// Type Definition
//

// Documented in header.
METH_IMPL(Type, debugString) {
    zvalue type = args[0];
    TypeInfo *info = getInfo(type);
    zvalue extraString;

    if (info->secret == coreSecret) {
        return info->name;
    } else if (info->secret != NULL) {
        extraString = stringFromUtf8(-1, " : opaque");
    } else if (typeParent(type) == TYPE_DerivedData) {
        extraString = EMPTY_STRING;
    } else {
        die("Shouldn't happen: opaque type without secret.");
    }

    return GFN_CALL(cat,
        stringFromUtf8(-1, "@@("),
        GFN_CALL(debugString, info->name),
        extraString,
        stringFromUtf8(-1, ")"));
}

// Documented in header.
METH_IMPL(Type, gcMark) {
    zvalue type = args[0];
    TypeInfo *info = getInfo(type);

    datMark(info->name);
    datMark(info->secret);

    return NULL;
}

// Documented in header.
METH_IMPL(Type, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];

    if (value == other) {
        // Easy case to avoid decomposition and detailed tests.
        return INT_0;
    }

    TypeInfo *info = getInfo(value);
    return intFromZint(typeCompare(info->name, info->secret, other));
}

/**
 * Define `typeSystem` as a module, as separate from the `Type` type.
 */
MOD_INIT(typeSystem) {
    TYPE_Type = allocType();
    TYPE_Type->type = TYPE_Type;

    TYPE_Value       = allocType();
    TYPE_Data        = allocType();
    TYPE_DerivedData = allocType();

    // The rest are in alphabetical order.
    TYPE_Builtin     = allocType();
    TYPE_Generic     = allocType();
    TYPE_Jump        = allocType();
    TYPE_String      = allocType();
    TYPE_Uniqlet     = allocType();

    coreSecret = makeUniqlet();
    datImmortalize(coreSecret);

    typeInit(TYPE_Type,        TYPE_Value, stringFromUtf8(-1, "Type"),        false);
    typeInit(TYPE_Value,       NULL,       stringFromUtf8(-1, "Value"),       false);
    typeInit(TYPE_Data,        TYPE_Value, stringFromUtf8(-1, "Data"),        false);
    typeInit(TYPE_DerivedData, TYPE_Data,  stringFromUtf8(-1, "DerivedData"), false);

    typeInit(TYPE_Builtin,     TYPE_Value, stringFromUtf8(-1, "Builtin"),     true);
    typeInit(TYPE_Generic,     TYPE_Value, stringFromUtf8(-1, "Generic"),     true);
    typeInit(TYPE_Jump,        TYPE_Value, stringFromUtf8(-1, "Jump"),        true);
    typeInit(TYPE_String,      TYPE_Data,  stringFromUtf8(-1, "String"),      false);
    typeInit(TYPE_Uniqlet,     TYPE_Value, stringFromUtf8(-1, "Uniqlet"),     true);

    // Make sure that the enum constants match up with what got assigned here.
    // If not, `funCall` will break.
    if (typeIndex(TYPE_Builtin) != DAT_INDEX_BUILTIN) {
        die("Mismatched index for `Builtin`: should be %lld",
            typeIndex(TYPE_Builtin));
    } else if (typeIndex(TYPE_Generic) != DAT_INDEX_GENERIC) {
        die("Mismatched index for `Generic`: should be %lld",
            typeIndex(TYPE_Generic));
    } else if (typeIndex(TYPE_Jump) != DAT_INDEX_JUMP) {
        die("Mismatched index for `Jump`: should be %lld",
            typeIndex(TYPE_Jump));
    }

    // Make sure that the "fake" header is sized the same as the real one.
    if (DAT_HEADER_SIZE != sizeof(DatHeader)) {
        die("Mismatched value header size: should be %lu", sizeof(DatHeader));
    }
}

/** Initializes the module. */
MOD_INIT(Type) {
    MOD_USE(OneOff);

    // Note: The `typeSystem` module (directly above) initializes `TYPE_Type`.

    METH_BIND(Type, debugString);
    METH_BIND(Type, gcMark);
    METH_BIND(Type, totalOrder);
}

// Documented in header.
zvalue TYPE_Type = NULL;
