/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

enum {
    MAX_IMMORTALS = 100
};

/** The stack base. */
static void *stackBase = NULL;

/** Array of all immortal values. */
static zvalue immortals[MAX_IMMORTALS];

/** How many immortal values there are right now. */
static zint immortalsSize = 0;

/** List head for the list of all live objects. */
static GcLinks liveHead = { &liveHead, &liveHead };

/**
 * Links the given value into the given list, removing it from its
 * previous list (if any).
 */
static void enlist(GcLinks *head, zvalue value) {
    GcLinks *vLinks = &value->links;

    if (vLinks->next != NULL) {
        GcLinks *next = vLinks->next;
        GcLinks *prev = vLinks->prev;
        next->prev = prev;
        prev->next = next;
    }

    GcLinks *headNext = head->next;

    vLinks->prev = head;
    vLinks->next = headNext;
    headNext->prev = vLinks;
    head->next = vLinks;
}

/**
 * Main garbage collection function.
 */
static void doGc(void *topOfStack) {
    // Nothing to do...yet.
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue datAllocValue(ztype type, zint size, zint extraBytes) {
    if (size < 0) {
        die("Invalid value size: %lld", size);
    }

    zvalue result = zalloc(sizeof(DatValue) + extraBytes);
    enlist(&liveHead, result);
    result->magic = DAT_VALUE_MAGIC;
    result->type = type;
    result->size = size;

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
void datImmortalize(zvalue value) {
    if (immortalsSize == MAX_IMMORTALS) {
        die("Too many immortal values");
    }

    immortals[immortalsSize] = value;
    immortalsSize++;
}

/* Documented in header. */
void datSetStackBase(void *base) {
    if (stackBase != NULL) {
        die("Stack base already set.");
    }

    stackBase = base;
}

/* Documented in header. */
void datGc(void) {
    int topOfStack = 0;
    doGc(&topOfStack);
}
