/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


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

/**
 * Returns whether the given pointer is properly aligned to be a
 * value.
 */
static bool isAligned(void *maybeValue) {
    intptr_t bits = (intptr_t) (void *) maybeValue;
    return ((bits & (DAT_VALUE_ALIGNMENT - 1)) == 0);
}


/*
 * Module functions
 */

/* Documented in header. */
bool datHasNth(zvalue value, zint n) {
    return (n >= 0) && (n < datSize(value));
}

/* Documented in header. */
zvalue datConservativeValueCast(void *maybeValue) {
    if (maybeValue == NULL) {
        return NULL;
    }

    if (!(isAligned(maybeValue) && utilIsHeapAllocated(maybeValue))) {
        return NULL;
    }

    zvalue value = maybeValue;
    GcLinks *links = &value->links;

    if (!(value->magic == DAT_VALUE_MAGIC) &&
          isAligned(links->next) &&
          isAligned(links->prev) &&
          (links == links->next->prev) &&
          (links == links->prev->next)) {
        return NULL;
    }

    return value;
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertValid(zvalue value) {
    if (value == NULL) {
        die("Null value.");
    }

    if (datConservativeValueCast(value) == NULL) {
        die("Invalid value pointer: %p", value);
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

    if (!datHasNth(value, n)) {
        die("Invalid index: %lld; size %lld", n, value->size);
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

/* Documented in header. */
zint datSize(zvalue value) {
    datAssertValid(value);
    return value->size;
}

/* Documented in header. */
ztype datType(zvalue value) {
    datAssertValid(value);
    return value->type;
}

/* Documented in header. */
bool datTypeIs(zvalue value, ztype type) {
    datAssertValid(value);
    return value->type == type;
}
