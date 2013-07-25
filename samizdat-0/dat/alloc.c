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

/** Array of all immortal values. */
static zvalue immortals[DAT_MAX_IMMORTALS];

/** How many immortal values there are right now. */
static zint immortalsSize = 0;

/**
 * Stack of references. This is what is scanned in lieu of scanning
 * the "real" stack during gc.
 */
static zvalue stack[DAT_MAX_STACK];

/** Current stack size. */
static zint stackSize = 0;

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
 * Asserts that the value is valid, with thorough (and slow) checking.
 */
static void thoroughlyValidate(zvalue maybeValue) {
    if (maybeValue == NULL) {
        die("Invalid value pointer: NULL");
    }

    if (!isAligned(maybeValue)) {
        die("Invalid value pointer (mis-aligned): %p", maybeValue);
    }

    if (!utilIsHeapAllocated(maybeValue)) {
        die("Invalid value pointer (not in heap): %p", maybeValue);
    }

    if (maybeValue->magic != DAT_VALUE_MAGIC) {
        die("Invalid value pointer (incorrect magic): %p", maybeValue);
    }

    GcLinks *links = &maybeValue->links;

    if (!(isAligned(links->next) &&
          isAligned(links->prev) &&
          (links == links->next->prev) &&
          (links == links->prev->next))) {
        die("Invalid value pointer (invalid links): %p", maybeValue);
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
        thoroughlyValidate(immortals[i]);
    }

    for (zint i = 0; i < stackSize; i++) {
        thoroughlyValidate(stack[i]);
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
static void doGc(void) {
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

    // Clear the map cache, so that it doesn't contain garbage pointers.
    // Note: In practice there is no significant measurable difference
    // between clearing the cache and marking everything in it, so
    // simplicity wins here.

    datMapClearCache();

    if (CHATTY_GC) {
        note("GC: Cleared map cache.");
    }

    // The root set consists of immortals and the stack. Recursively mark
    // those, which causes anything found to be alive to be linked into
    // the live list.

    for (zint i = 0; i < immortalsSize; i++) {
        datMark(immortals[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld immortals.", immortalsSize);
    }

    for (zint i = 0; i < stackSize; i++) {
        datMark(stack[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld stack values.", stackSize);
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

    zvalue result = utilAlloc(sizeof(DatHeader) + extraBytes);
    result->magic = DAT_VALUE_MAGIC;
    result->type = type;
    result->size = size;

    enlist(&liveHead, result);
    datFrameAdd(result);

    allocationCount++;

    sanityCheck(false);

    return result;
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
        case DAT_INT:
        case DAT_LIST:
        case DAT_MAP:
        case DAT_STRING:
        case DAT_DERIV:
        case DAT_UNIQLET: {
            break;
        }
        default: {
            die("Invalid type for value: (%p)->type == %#04x",
                value, value->type);
        }
    }
}

/* Documented in header. */
zstackPointer datFrameStart(void) {
    return &stack[stackSize];
}

/* Documented in header. */
void datFrameAdd(zvalue value) {
    if (stackSize >= DAT_MAX_STACK) {
        die("Value stack overflow.");
    }

    stack[stackSize] = value;
    stackSize++;
}

/* Documented in header. */
void datFrameReset(zstackPointer savedStack, zvalue stackedValue) {
    // The difference between this function and `datFrameReturn` is
    // one of intent, even though the implementation is (blatantly)
    // identical.
    datFrameReturn(savedStack, stackedValue);
}

/* Documented in header. */
void datFrameReturn(zstackPointer savedStack, zvalue returnValue) {
    zint returnSize = savedStack - stack;

    if (returnSize > stackSize) {
        die("Cannot return to deeper frame.");
    }

    stackSize = returnSize;

    if (returnValue != NULL) {
        datFrameAdd(returnValue);
    }
}

/* Documented in header. */
void datGc(void) {
    allocationCount = 0;
    doGc();
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
        case DAT_LIST:    { datListMark(value);    break; }
        case DAT_MAP:     { datMapMark(value);     break; }
        case DAT_DERIV:   { datDerivMark(value);   break; }
        case DAT_UNIQLET: { datUniqletMark(value); break; }
        default: {
            // Nothing to do here. The other types don't need sub-marking.
        }
    }
}
