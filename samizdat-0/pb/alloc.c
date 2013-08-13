/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/*
 * Private Definitions
 */

enum {
    /** Whether to spew to the console during gc. */
    CHATTY_GC = false,

    /** Whether to be paranoid about corruption checks. */
    THEYRE_OUT_TO_GET_ME = false
};

/** Array of all immortal values. */
static zvalue immortals[PB_MAX_IMMORTALS];

/** How many immortal values there are right now. */
static zint immortalsSize = 0;

/**
 * Stack of references. This is what is scanned in lieu of scanning
 * the "real" stack during gc.
 */
static zvalue stack[PB_MAX_STACK];

/** Current stack size. */
static zint stackSize = 0;

/** List head for the list of all live values. */
static PbHeader liveHead = {
    .next = &liveHead,
    .prev = &liveHead,
    .magic = 0,
    .marked = false,
    .type = NULL
};

/** List head for the list of all doomed values. */
static PbHeader doomedHead = {
    .next = &doomedHead,
    .prev = &doomedHead,
    .magic = 0,
    .marked = false,
    .type = NULL
};

/** Number of allocations since the last garbage collection. */
static zint allocationCount = 0;

/**
 * Returns whether the given pointer is properly aligned to be a
 * value.
 */
static bool isAligned(void *maybeValue) {
    intptr_t bits = (intptr_t) (void *) maybeValue;
    return ((bits & (PB_VALUE_ALIGNMENT - 1)) == 0);
}

/**
 * Asserts that the value is valid, with thorough (and slow) checking.
 */
static void thoroughlyValidate(zvalue maybeValue) {
    if (maybeValue == NULL) {
        die("Invalid value: NULL");
    }

    if (!isAligned(maybeValue)) {
        die("Invalid value (mis-aligned): %p", maybeValue);
    }

    if (!utilIsHeapAllocated(maybeValue)) {
        die("Invalid value (not in heap): %p", maybeValue);
    }

    if (maybeValue->magic != PB_VALUE_MAGIC) {
        die("Invalid value (incorrect magic): %p", maybeValue);
    }

    if (!(isAligned(maybeValue->next) &&
          isAligned(maybeValue->prev) &&
          (maybeValue == maybeValue->next->prev) &&
          (maybeValue == maybeValue->prev->next))) {
        die("Invalid value (invalid links): %p", maybeValue);
    }
}

/**
 * Sanity check the circular list with the given head.
 */
static void sanityCheckList(PbHeader *head) {
    for (zvalue item = head->next; item != head; item = item->next) {
        thoroughlyValidate(item);
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
static void enlist(PbHeader *head, zvalue value) {
    if (value->next != NULL) {
        zvalue next = value->next;
        zvalue prev = value->prev;
        next->prev = prev;
        prev->next = next;
    }

    zvalue headNext = head->next;

    value->prev = head;
    value->next = headNext;
    headNext->prev = value;
    head->next = value;
}

/**
 * Main garbage collection function.
 */
static void doGc(void) {
    zint counter; // Used throughout.

    if (GFN_eq == NULL) {
        die("`pb` module not yet initialized.");
    }

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

    // The root set consists of immortals and the stack. Recursively mark
    // those, which causes anything found to be alive to be linked into
    // the live list.

    for (zint i = 0; i < immortalsSize; i++) {
        pbMark(immortals[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld immortals.", immortalsSize);
    }

    for (zint i = 0; i < stackSize; i++) {
        pbMark(stack[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld stack values.", stackSize);
    }

    // Free everything left on the doomed list.

    sanityCheck(false);

    counter = 0;
    for (zvalue item = doomedHead.next; item != &doomedHead; /*next*/) {
        if (item->marked) {
            die("Marked item on doomed list!");
        }

        // Need to grab `item->next` before freeing the item.
        zvalue next = item->next;

        // Prevent this from being mistaken for a live value.
        item->next = item->prev = NULL;
        item->marked = 999;
        item->magic = 999;
        item->type = NULL;

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
    for (zvalue item = liveHead.next; item != &liveHead; /*next*/) {
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
 * Exported Definitions
 */

/* Documented in header. */
zvalue pbAllocValue(zvalue type, zint extraBytes) {
    if (allocationCount >= PB_ALLOCATIONS_PER_GC) {
        pbGc();
    } else {
        sanityCheck(false);
    }

    zvalue result = utilAlloc(sizeof(PbHeader) + extraBytes);
    result->magic = PB_VALUE_MAGIC;
    result->type = type;

    allocationCount++;
    enlist(&liveHead, result);
    pbFrameAdd(result);
    sanityCheck(false);

    return result;
}

/* Documented in header. */
void pbAssertValid(zvalue value) {
    if (value == NULL) {
        die("Null value.");
    }

    if (value->magic != PB_VALUE_MAGIC) {
        die("Invalid value (incorrect magic): %p", value);
    }

    if (value->type == NULL) {
        die("Invalid value (null type): %p", value);
    }
}

/* Documented in header. */
void pbAssertValidOrNull(zvalue value) {
    if (value != NULL) {
        pbAssertValid(value);
    }
}

/* Documented in header. */
zstackPointer pbFrameStart(void) {
    return &stack[stackSize];
}

/* Documented in header. */
void pbFrameAdd(zvalue value) {
    if (stackSize >= PB_MAX_STACK) {
        // As a hail-mary, do a forced gc and then clear the value stack, in
        // the hope that a gc won't end up being done while producing the
        // dying stack trace.
        pbGc();
        stackSize = 0;
        die("Value stack overflow.");
    }

    stack[stackSize] = value;
    stackSize++;
}

/* Documented in header. */
void pbFrameReset(zstackPointer savedStack, zvalue stackedValue) {
    // The difference between this function and `pbFrameReturn` is
    // one of intent, even though the implementation is (blatantly)
    // identical.
    pbFrameReturn(savedStack, stackedValue);
}

/* Documented in header. */
void pbFrameReturn(zstackPointer savedStack, zvalue returnValue) {
    zint returnSize = savedStack - stack;

    if (returnSize > stackSize) {
        die("Cannot return to deeper frame.");
    }

    stackSize = returnSize;

    if (returnValue != NULL) {
        pbFrameAdd(returnValue);
    }
}

/* Documented in header. */
void pbGc(void) {
    allocationCount = 0;
    doGc();
}

/* Documented in header. */
void pbImmortalize(zvalue value) {
    if (immortalsSize == PB_MAX_IMMORTALS) {
        die("Too many immortal values!");
    }

    pbAssertValid(value);

    immortals[immortalsSize] = value;
    immortalsSize++;
}

/* Documented in header. */
void pbMark(zvalue value) {
    if (value == NULL) {
        return;
    }

    pbAssertValid(value);

    if (value->marked) {
        return;
    }

    value->marked = true;
    enlist(&liveHead, value);

    zvalue marker = gfnFind(GFN_gcMark, value);
    if (marker != NULL) {
        funCall(marker, 1, &value);
    }

    // Opaque types are all immortalized, but transparent types need to
    // be marked.
    pbMark(value->type);
}
