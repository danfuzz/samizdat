/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stdlib.h>


/*
 * Helper definitions
 */

/** The next uniqlet id to issue. */
static zint theNextId = 0;

/**
 * Asserts that the given key is valid as a uniqlet key.
 */
static void assertValidUniqletKey(void *key) {
    if (key == NULL) {
        die("Invalid uniqlet key: NULL");
    }
}

/**
 * Gets a pointer to the info of a uniqlet.
 */
static UniqletInfo *uniqletInfo(zvalue uniqlet) {
    datAssertUniqlet(uniqlet);

    return &((DatUniqlet *) uniqlet)->info;
}

/**
 * Gets the id of a uniqlet.
 */
static zint uniqletId(zvalue uniqlet) {
    return uniqletInfo(uniqlet)->id;
}

/**
 * Allocates and initializes a uniqlet, without doing error-checking
 * on the arguments.
 */
static zvalue newUniqlet(void *key, void *value) {
    zvalue result = datAllocValue(DAT_UNIQLET, 0, sizeof(UniqletInfo));

    if (theNextId < 0) {
        // Shouldn't be possible, but just in case...
        die("Shouldn't happen: Way too many uniqlets!");
    }

    UniqletInfo *info = &((DatUniqlet *) result)->info;
    info->id = theNextId;
    info->key = key;
    info->value = value;
    theNextId++;

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datUniqletCompare(zvalue v1, zvalue v2) {
    zint id1 = uniqletId(v1);
    zint id2 = uniqletId(v2);

    if (id1 < id2) {
        return ZLESS;
    } else if (id1 > id2) {
        return ZMORE;
    } else {
        return ZSAME;
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datUniqlet(void) {
    return newUniqlet(NULL, NULL);
}

/* Documented in header. */
zvalue datUniqletWith(void *key, void *value) {
    assertValidUniqletKey(key);
    return newUniqlet(key, value);
}

/* Documented in header. */
bool datUniqletHasKey(zvalue uniqlet, void *key) {
    assertValidUniqletKey(key);

    return (key == uniqletInfo(uniqlet)->key);
}

/* Documented in header. */
void *datUniqletGetValue(zvalue uniqlet, void *key) {
    if (!datUniqletHasKey(uniqlet, key)) {
        die("Wrong uniqlet key for get.");
    }

    return uniqletInfo(uniqlet)->value;
}

/* Documented in header. */
void datUniqletSetValue(zvalue uniqlet, void *key, void *value) {
    if (!datUniqletHasKey(uniqlet, key)) {
        die("Wrong uniqlet key for set.");
    }

    uniqletInfo(uniqlet)->value = value;
}
