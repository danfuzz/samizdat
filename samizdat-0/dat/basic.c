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
        case DAT_INT:     return "integer";
        case DAT_STRING:  return "string";
        case DAT_LIST:    return "list";
        case DAT_MAP:     return "map";
        case DAT_UNIQLET: return "uniqlet";
        case DAT_HIGHLET: return "highlet";
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
 * Module functions
 */

/* Documented in header. */
bool datHasNth(zvalue value, zint n) {
    return (n >= 0) && (n < datSize(value));
}


/*
 * Exported functions
 */

/* Documented in header. */
void datAssertNth(zvalue value, zint n) {
    datAssertValid(value);

    if (!datHasNth(value, n)) {
        die("Invalid index: %lld; size %lld", n, value->size);
    }
}

/* Documented in header. */
void datAssertInt(zvalue value) {
    assertType(value, DAT_INT);
}

/* Documented in header. */
void datAssertString(zvalue value) {
    assertType(value, DAT_STRING);
}

/* Documented in header. */
void datAssertList(zvalue value) {
    assertType(value, DAT_LIST);
}

/* Documented in header. */
void datAssertMap(zvalue value) {
    assertType(value, DAT_MAP);
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
    return value->size;
}

/* Documented in header. */
ztype datType(zvalue value) {
    return value->type;
}

/* Documented in header. */
bool datTypeIs(zvalue value, ztype type) {
    return value->type == type;
}
