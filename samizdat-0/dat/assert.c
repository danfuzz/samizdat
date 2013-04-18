/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stdlib.h>


/*
 * Helper functions
 */

/**
 * Gets the name of a given type.
 */
static const char *typeName(ztype type) {
    switch (type) {
        case DAT_INTLET:    return "intlet";
        case DAT_STRINGLET: return "stringlet";
        case DAT_LISTLET:   return "listlet";
        case DAT_MAPLET:    return "maplet";
        case DAT_UNIQLET:   return "uniqlet";
        case DAT_HIGHLET:   return "highlet";
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
    if (value == NULL) {
        die("Null value.");
    }

    zint bits = (zint) (void *) value;

    if ((bits % DAT_VALUE_ALIGNMENT) != 0) {
        die("Bad alignment for value: %p", value);
    }

    if (value->magic != DAT_VALUE_MAGIC) {
        die("Incorrect magic for value: (%p)->magic == %#04x",
            value, value->magic);
    }

    switch (value->type) {
        case DAT_INTLET:
        case DAT_STRINGLET:
        case DAT_LISTLET:
        case DAT_MAPLET:
        case DAT_UNIQLET:
        case DAT_HIGHLET: {
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

    if ((datType(value) != DAT_INTLET) && (value->size <= n)) {
        die("Invalid size for value access: (%p)->size == %lld; <= %lld",
            value, value->size, n);
    }
}

/* Documented in header. */
void datAssertIntlet(zvalue value) {
    assertType(value, DAT_INTLET);
}

/* Documented in header. */
void datAssertStringlet(zvalue value) {
    assertType(value, DAT_STRINGLET);
}

/* Documented in header. */
void datAssertListlet(zvalue value) {
    assertType(value, DAT_LISTLET);
}

/* Documented in header. */
void datAssertMaplet(zvalue value) {
    assertType(value, DAT_MAPLET);
}

/* Documented in header. */
void datAssertUniqlet(zvalue value) {
    assertType(value, DAT_UNIQLET);
}

/* Documented in header. */
void datAssertHighlet(zvalue value) {
    assertType(value, DAT_HIGHLET);
}
