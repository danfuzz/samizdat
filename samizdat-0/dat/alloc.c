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
    CHATTY_GC = true,
    MAX_IMMORTALS = 100
};

/** The stack base. */
static void *stackBase = NULL;

/** Array of all immortal values. */
static zvalue immortals[MAX_IMMORTALS];

/** How many immortal values there are right now. */
static zint immortalsSize = 0;

/** List head for the list of all live values. */
static GcLinks liveHead = { &liveHead, &liveHead, false };

/** List head for the list of all doomed values. */
static GcLinks doomedHead = { &doomedHead, &doomedHead, false };

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
    // Sanity check: If there have been no allocations, then there's nothing
    // to do.

    if (liveHead.next == &liveHead) {
        return;
    }

    // Start by dooming everything.

    doomedHead = liveHead;
    doomedHead.next->prev = &doomedHead;
    doomedHead.prev->next = &doomedHead;
    liveHead.next = &liveHead;
    liveHead.prev = &liveHead;

    // Starting with the roots of { immortals, stack references }, recursively
    // mark everything. This moves anything found to be alive onto the live
    // list.

    for (zint i = 0; i < immortalsSize; i++) {
        datMark(immortals[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld immortals.", immortalsSize);
    }

    zint counter = 0;

    for (void **stack = topOfStack; stack < (void **) stackBase; stack++) {
        zvalue value = datConservativeValueCast(*stack);
        if (value != NULL) {
            datMark(value);
            counter++;
        }
    }

    if (CHATTY_GC) {
        note("GC: Scanned %ld bytes of stack.",
             (char *) stackBase - (char *) topOfStack);
        note("GC: Found %lld live stack references.", counter);
    }

    // Free everything left on the doomed list.

    counter = 0;
    for (GcLinks *item = doomedHead.next; item != &doomedHead; /*next*/) {
        GcLinks *next = item->next;

        if (datType((zvalue) item) == DAT_UNIQLET) {
            datUniqletFree((zvalue) item);
        }

        zfree(item);
        item = next;
        counter++;
    }

    doomedHead.next = &doomedHead;
    doomedHead.prev = &doomedHead;

    if (CHATTY_GC) {
        note("GC: Freed %lld dead values.", counter);
    }

    // Unmark the live list.

    counter = 0;
    for (GcLinks *item = liveHead.next; item != &liveHead; /*next*/) {
        item->marked = false;
        item = item->next;
        counter++;
    }

    if (CHATTY_GC) {
        note("GC: %lld live values remain.", counter);
    }
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
void datMark(zvalue value) {
    if ((value == NULL) || value->links.marked) {
        return;
    }

    value->links.marked = true;
    enlist(&liveHead, value);

    switch (value->type) {
        case DAT_LISTLET: { datListletMark(value); break; }
        case DAT_MAPLET:  { datMapletMark(value);  break; }
        case DAT_UNIQLET: { datUniqletMark(value); break; }
        case DAT_HIGHLET: { datHighletMark(value); break; }
        default: {
            // Nothing to do here. The other types don't need sub-marking.
        }
    }
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
