/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>
#include <string.h>


/*
 * Helper definitions
 */

enum {
    /** Whether to spew to the console during gc. */
    CHATTY_GC = false,

    /** Whether to be paranoid about corruption checks. */
    THEYRE_OUT_TO_GET_ME = false
};

/** The stack base. */
static void *stackBase = NULL;

/** Array of all immortal values. */
static zvalue immortals[DAT_MAX_IMMORTALS];

/** How many immortal values there are right now. */
static zint immortalsSize = 0;

/**
 * Array of recent allocations. These are exempted from gc (that is,
 * declared "live") in order to deal with the possibility that their
 * liveness is only by virtue of being in registers that didn't spill
 * "in time" for the gc.
 */
static zvalue newbies[DAT_NEWBIES_SIZE];

/** Next index to use when storing to `newbies`. `-1` before initialized. */
static zint newbiesNext = -1;

/** List head for the list of all live values. */
static GcLinks liveHead = { &liveHead, &liveHead, false };

/** List head for the list of all doomed values. */
static GcLinks doomedHead = { &doomedHead, &doomedHead, false };

/** Number of allocations since the last garbage collection. */
static zint allocationCount = 0;

/**
 * Returns whether the given pointer is properly aligned to be a
 * value.
 */
static bool isAligned(void *maybeValue) {
    intptr_t bits = (intptr_t) (void *) maybeValue;
    return ((bits & (DAT_VALUE_ALIGNMENT - 1)) == 0);
}

/**
 * Check the gc links on a value.
 */
static void checkLinks(zvalue value) {
    GcLinks *links = &value->links;

    if ((links->next->prev != links) || (links->prev->next != links)) {
        die("Link corruption.");
    }
}

/**
 * Asserts that the value is valid, with thorough (and slow)
 * checking.
 */
static void thoroughlyValidate(zvalue value) {
    if (value == NULL) {
        die("NULL value.");
    }

    if (datConservativeValueCast(value) == NULL) {
        die("Invalid value pointer: %p", value);
    }
}

/**
 * Sanity check the circular list with the given head.
 */
static void sanityCheckList(GcLinks *head) {
    for (GcLinks *item = head->next; item != head; item = item->next) {
        zvalue one = (zvalue) item;
        thoroughlyValidate(one);
    }
}

/**
 * Sanity check the links and tables.
 */
static void sanityCheck(bool force) {
    if (!(force || THEYRE_OUT_TO_GET_ME)) {
        return;
    }

    for (zint i = 0; i < immortalsSize; i++) {
        zvalue one = immortals[i];
        thoroughlyValidate(one);
    }

    for (zint i = 0; i < DAT_NEWBIES_SIZE; i++) {
        zvalue one = newbies[i];
        if (one != NULL) {
            thoroughlyValidate(one);
        }
    }

    sanityCheckList(&liveHead);
    sanityCheckList(&doomedHead);
}

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
    zint counter; // Used throughout.

    sanityCheck(false);

    // Quick check: If there have been no allocations, then there's nothing
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

    // The root set consists of immortals, newbies, and the stack (scanned
    // conservatively). Recursively mark thosee, which causes anything found
    // to be alive onto the live list.

    for (zint i = 0; i < immortalsSize; i++) {
        datMark(immortals[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld immortals.", immortalsSize);
    }

    counter = 0;
    for (zint i = 0; i < DAT_NEWBIES_SIZE; i++) {
        zvalue one = newbies[i];
        if (one != NULL) {
            datMark(one);
            counter++;
        }
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld newbies.", counter);
    }

    // Align the stack pointer.
    topOfStack = (void *) ((intptr_t) topOfStack & ~(sizeof(void *) - 1));

    if (CHATTY_GC) {
        note("GC: Stack: %p .. %p", topOfStack, stackBase);
    }

    counter = 0;
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

    sanityCheck(false);

    counter = 0;
    for (GcLinks *item = doomedHead.next; item != &doomedHead; /*next*/) {
        if (item->marked) {
            die("Marked item on doomed list!");
        }

        // Need to grab `item->next` before freeing the item.
        GcLinks *next = item->next;
        zvalue one = (zvalue) item;

        if (datTypeIs(one, DAT_UNIQLET)) {
            // Link the item to itself, so that its sanity check will
            // still pass.
            item->next = item->prev = item;
            datUniqletFree(one);
        }

        // Prevent this from being mistaken for a live value.
        item->next = item->prev = NULL;
        item->marked = 999;
        one->magic = 999;
        one->type = 999;

        utilFree(item);
        item = next;
        counter++;
    }

    doomedHead.next = &doomedHead;
    doomedHead.prev = &doomedHead;

    if (CHATTY_GC) {
        note("GC: Freed %lld dead values.", counter);
    }

    // Unmark the live list.

    sanityCheck(false);

    counter = 0;
    for (GcLinks *item = liveHead.next; item != &liveHead; /*next*/) {
        item->marked = false;
        item = item->next;
        counter++;
    }

    if (CHATTY_GC) {
        note("GC: %lld live values remain.", counter);
    }

    sanityCheck(true);
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue datAllocValue(ztype type, zint size, zint extraBytes) {
    if (size < 0) {
        die("Invalid value size: %lld", size);
    }

    if (allocationCount >= DAT_ALLOCATIONS_PER_GC) {
        datGc();
    } else {
        sanityCheck(false);
    }

    zvalue result = utilAlloc(sizeof(DatValue) + extraBytes);
    enlist(&liveHead, result);
    result->magic = DAT_VALUE_MAGIC;
    result->type = type;
    result->size = size;

    allocationCount++;

    if (newbiesNext == -1) {
        memset(newbies, 0, sizeof(newbies));
        newbiesNext = 0;
    }

    newbies[newbiesNext] = result;
    newbiesNext = (newbiesNext + 1) % DAT_NEWBIES_SIZE;

    sanityCheck(false);

    return result;
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

    if (!((value->magic == DAT_VALUE_MAGIC) &&
          isAligned(links->next) &&
          isAligned(links->prev) &&
          (links == links->next->prev) &&
          (links == links->prev->next))) {
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

    switch (value->type) {
        case DAT_INTEGER:
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
void datImmortalize(zvalue value) {
    if (immortalsSize == DAT_MAX_IMMORTALS) {
        die("Too many immortal values!");
    }

    datAssertValid(value);

    immortals[immortalsSize] = value;
    immortalsSize++;
}

/* Documented in header. */
void datMark(zvalue value) {
    if (value == NULL) {
        return;
    }

    datAssertValid(value);

    if (value->links.marked) {
        return;
    }

    value->links.marked = true;
    enlist(&liveHead, value);

    switch (value->type) {
        case DAT_LISTLET: { datListMark(value); break; }
        case DAT_MAPLET:  { datMapMark(value);  break; }
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

    allocationCount = 0;
    doGc(&topOfStack);
}
