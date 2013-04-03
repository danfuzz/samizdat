/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"
#include "util.h"


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
    samAssertValid(value);

    if (value->type == type) {
	return;
    }

    samDie("Expected type %s: (%p)->type == %s",
	   typeName(type), value, typeName(value->type));
}


/*
 * API implementation
 */

/** Documented in API header. */
void samAssertValid(zvalue value) {
    zint bits = (zint) (void *) value;

    if ((bits & ((1 << SAM_VALUE_ALIGNMENT) - 1)) != 0) {
	samDie("Bad alignment for value: %p", value);
    }

    if (value->magic != SAM_VALUE_MAGIC) {
	samDie("Incorrect magic for value: (%p)->magic == %04x",
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
	    samDie("Invalid type for value: (%p)->type == %04x",
		   value, value->type);
	}
    }
}

/** Documented in API header. */
void samAssertNth(zvalue value, zint n) {
    samAssertValid(value);

    if (n < 0) {
	samDie("Invalid index: %lld", n);
    }

    if (value->size <= n) {
	samDie("Invalid size for value access: (%p)->size == %lld; <= %lld",
	       value, value->size, n);
    }
}

/** Documented in API header. */
void samAssertIntlet(zvalue value) {
    assertType(value, SAM_INTLET);
}

/** Documented in API header. */
void samAssertListlet(zvalue value) {
    assertType(value, SAM_LISTLET);
}

/** Documented in API header. */
void samAssertMaplet(zvalue value) {
    assertType(value, SAM_MAPLET);
}

/** Documented in API header. */
void samAssertUniqlet(zvalue value) {
    assertType(value, SAM_UNIQLET);
}
