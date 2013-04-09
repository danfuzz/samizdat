/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"


/*
 * Helper functions
 */

/**
 * Gets the name of a given type.
 */
static const char *typeName(ztype type) {
    switch (type) {
        case SAM_INTLET:  return "intlet";
        case SAM_LISTLET: return "listlet";
        case SAM_MAPLET:  return "maplet";
        case SAM_UNIQLET: return "uniqlet";
    }

    return "<unknown-type>";
}

/**
 * Asserts a particular type.
 */
static void assertType(zvalue value, ztype type) {
    datAssertValid(value);

    if (value->type == type) {
        return;
    }

    die("Expected type %s: (%p)->type == %s",
           typeName(type), value, typeName(value->type));
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertValid(zvalue value) {
    zint bits = (zint) (void *) value;

    if ((bits % SAM_VALUE_ALIGNMENT) != 0) {
        die("Bad alignment for value: %p", value);
    }

    if (value->magic != SAM_VALUE_MAGIC) {
        die("Incorrect magic for value: (%p)->magic == %#04x",
               value, value->magic);
    }

    switch (value->type) {
        case SAM_INTLET:
        case SAM_LISTLET:
        case SAM_MAPLET:
        case SAM_UNIQLET: {
            break;
        }
        default: {
            die("Invalid type for value: (%p)->type == %#04x",
                   value, value->type);
        }
    }
}

/* Documented in header. */
void datAssertNth(zvalue value, zint n) {
    datAssertValid(value);

    if (n < 0) {
        die("Invalid index: %lld", n);
    }

    if ((datType(value) != SAM_INTLET) && (value->size <= n)) {
        die("Invalid size for value access: (%p)->size == %lld; <= %lld",
               value, value->size, n);
    }
}

/* Documented in header. */
void datAssertIntlet(zvalue value) {
    assertType(value, SAM_INTLET);
}

/* Documented in header. */
void datAssertListlet(zvalue value) {
    assertType(value, SAM_LISTLET);
}

/* Documented in header. */
void datAssertMaplet(zvalue value) {
    assertType(value, SAM_MAPLET);
}

/* Documented in header. */
void datAssertUniqlet(zvalue value) {
    assertType(value, SAM_UNIQLET);
}
