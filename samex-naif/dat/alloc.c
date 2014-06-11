// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <time.h>  // Used for "chatty gc."

#include "type/Function.h"
#include "type/Value.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

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

/** List head for the list of all live values. Double-linked circular list. */
static DatHeader liveHead = {
    .next = &liveHead,
    .prev = &liveHead,
    .magic = 0,
    .marked = false,
    .type = NULL
};

/**
 * List head for the list of all doomed values. Double-linked circular list.
 */
static DatHeader doomedHead = {
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
    return ((bits & (DAT_VALUE_ALIGNMENT - 1)) == 0);
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

    if (maybeValue->magic != DAT_VALUE_MAGIC) {
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
static void sanityCheckList(DatHeader *head) {
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

    sanityCheckList(&liveHead);
    sanityCheckList(&doomedHead);
}

/**
 * Links the given value into the given list, removing it from its
 * previous list (if any).
 */
static void enlist(DatHeader *head, zvalue value) {
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
    zint counter;  // Used throughout.

    if (GFN_totalEq == NULL) {
        die("`dat` module not yet initialized.");
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
        datMark(immortals[i]);
    }

    if (CHATTY_GC) {
        note("GC: Marked %lld immortals.", immortalsSize);
    }

    zint count = markFrameStack();

    if (CHATTY_GC) {
        note("GC: Marked %lld stack values.", count);
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


//
// Exported Definitions
//

// Documented in header.
zvalue datAllocValue(zvalue type, zint extraBytes) {
    if (allocationCount >= DAT_ALLOCATIONS_PER_GC) {
        datGc();
    } else {
        sanityCheck(false);
    }

    zvalue result = utilAlloc(sizeof(DatHeader) + extraBytes);
    result->magic = DAT_VALUE_MAGIC;
    result->type = type;

    allocationCount++;
    enlist(&liveHead, result);
    datFrameAdd(result);
    sanityCheck(false);

    return result;
}

// Documented in header.
void assertValid(zvalue value) {
    if (value == NULL) {
        die("Null value.");
    }

    if (value->magic != DAT_VALUE_MAGIC) {
        die("Invalid value (incorrect magic): %p", value);
    }

    if (value->type == NULL) {
        die("Invalid value (null type): %p", value);
    }
}

// Documented in header.
void assertValidOrNull(zvalue value) {
    if (value != NULL) {
        assertValid(value);
    }
}

// Documented in header.
void datGc(void) {
    allocationCount = 0;

    if (CHATTY_GC) {
        static double totalSec = 0;
        clock_t startTime = clock();
        doGc();
        double elapsedSec = (double) (clock() - startTime) / CLOCKS_PER_SEC;
        totalSec += elapsedSec;
        note("GC: %g msec this cycle. %g sec overall.",
            elapsedSec * 1000, totalSec);
    } else {
        doGc();
    }
}

// Documented in header.
void datImmortalize(zvalue value) {
    if (immortalsSize == DAT_MAX_IMMORTALS) {
        die("Too many immortal values!");
    }

    assertValid(value);

    immortals[immortalsSize] = value;
    immortalsSize++;
}

// Documented in header.
void datMark(zvalue value) {
    if ((value == NULL) || value->marked) {
        return;
    }

    value->marked = true;
    enlist(&liveHead, value);

    GFN_CALL(gcMark, value);

    // As of this writing, types are all immortal, but that may change. This
    // `datMark` call has negligible cost and safeguards against that possible
    // change.
    datMark(value->type);
}
