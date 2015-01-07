// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <time.h>  // Used for "chatty gc."

#include "type/Class.h"
#include "type/Value.h"

#include "impl.h"


//
// Private Definitions
//

/** Array of all immortal values. */
static zvalue immortals[DAT_MAX_IMMORTALS];

/** How many immortal values there are right now. */
static zint immortalsSize = 0;

/** List head for the list of all live values. Double-linked circular list. */
static DatHeader liveHead = {
    .next = &liveHead,
    .prev = &liveHead,
    .mark = MARK_AZURE,
    .cls = NULL
};

/**
 * List head for the list of all doomed values. Double-linked circular list.
 */
static DatHeader doomedHead = {
    .next = &doomedHead,
    .prev = &doomedHead,
    .mark = MARK_AZURE,
    .cls = NULL
};

/** Current color that represents live values. */
static zmarkColor liveColor = MARK_MAUVE;

/** Number of allocations since the last garbage collection. */
static zint allocationCount = 0;

/** Number of gcs performed. */
static zint gcCount = 0;

/** Total number live objects. Only used when being chatty. */
static zint liveCount = 0;

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
static bool thoroughlyValidate(zvalue maybeValue, zmarkColor expectedColor) {
    if (maybeValue == NULL) {
        die("Invalid value: NULL");
    }

    if (!isAligned(maybeValue)) {
        note("Invalid value (mis-aligned): %p", maybeValue);
        return false;
    }

    if (!utilIsHeapAllocated(maybeValue)) {
        note("Invalid value (not in heap): %p", maybeValue);
        return false;
    }

    if (maybeValue->mark != expectedColor) {
        note("Invalid value (wrong color): %p", maybeValue);
        return false;
    }

    if (!(isAligned(maybeValue->next) &&
          isAligned(maybeValue->prev) &&
          (maybeValue == maybeValue->next->prev) &&
          (maybeValue == maybeValue->prev->next))) {
        note("Invalid value (invalid links): %p", maybeValue);
        return false;
    }

    return true;
}

/**
 * Sanity check the circular list with the given head.
 */
static bool sanityCheckList(DatHeader *head, zmarkColor expectedColor) {
    for (zvalue item = head->next; item != head; item = item->next) {
        if (!thoroughlyValidate(item, expectedColor)) {
            return false;
        }
    }

    return true;
}

/**
 * Sanity check the links and tables.
 */
static void sanityCheck(bool force) {
    if (!(force || DAT_MEMORY_PARANOIA)) {
        return;
    }

    for (zint i = 0; i < immortalsSize; i++) {
        if (!thoroughlyValidate(immortals[i], liveColor)) {
            die("...at immortal #%d", i);
        }
    }

    if (!sanityCheckList(&liveHead, liveColor)) {
        die("...on live list.");
    }

    if (!sanityCheckList(&doomedHead, liveColor ^ 1)) {
        die("...on doomed list.");
    }
}

/**
 * Links the given value onto the end of the given list, removing it from its
 * previous list (if any).
 */
static void enlist(DatHeader *head, zvalue value) {
    if (value->next != NULL) {
        zvalue next = value->next;
        zvalue prev = value->prev;
        next->prev = prev;
        prev->next = next;
    }

    zvalue headPrev = head->prev;

    value->prev = headPrev;
    value->next = head;
    headPrev->next = value;
    head->prev = value;
}

/**
 * Main garbage collection function.
 */
static void doGc(void) {
    zint counter;  // Used throughout.

    if (SYM(gcMark) == NULL) {
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

    liveColor = liveColor ^ 1;

    // The root set consists of immortals and the stack. Recursively mark
    // those, which causes anything found to be alive to be linked into
    // the live list.

    for (zint i = 0; i < immortalsSize; i++) {
        datMark(immortals[i]);
    }

    if (DAT_CHATTY_GC) {
        note("GC: Marked %d immortals.", immortalsSize);
    }

    counter = markFrameStack();

    if (DAT_CHATTY_GC) {
        note("GC: Marked %d stack values.", counter);
    }

    // Calls to `datMark()` just place items on the live list but do not call
    // through to mark their innards. The following loop walks down the live
    // list doing that marking, which can cause yet more items to be enlisted.
    // Since new items are added to the end of the list, there's nothing
    // special to do to handle such new entries.

    for (zvalue item = liveHead.next; item != &liveHead; item = item->next) {
        callGcMark(item);
    }

    // Free everything left on the doomed list.

    sanityCheck(false);

    if (DAT_CHATTY_GC) {
        counter = 0;
    }

    for (zvalue item = doomedHead.next; item != &doomedHead; /*next*/) {
        if (item->mark == liveColor) {
            die("Live item on doomed list!");
        }

        // Need to grab `item->next` before freeing the item.
        zvalue next = item->next;

        // Prevent this from being mistaken for a live value.
        item->next = item->prev = NULL;
        item->cls = NULL;

        utilFree(item);
        item = next;

        if (DAT_CHATTY_GC) {
            counter++;
            liveCount--;
        }
    }

    doomedHead.next = &doomedHead;
    doomedHead.prev = &doomedHead;

    if (DAT_CHATTY_GC) {
        note("GC: Freed %d dead values.", counter);
        note("GC: %d live values remain.", liveCount);
    }

    // Occasional sanity check.

    gcCount++;
    if (DAT_MEMORY_PARANOIA || ((gcCount & 0x3f) == 0)) {
        sanityCheck(true);
    }
}


//
// Exported Definitions
//

// Documented in header.
zvalue datAllocValue(zvalue cls, zint extraBytes) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        if (CLS_Class != NULL) {
            assertValid(cls);
        }
    }

    if (allocationCount >= DAT_ALLOCATIONS_PER_GC) {
        datGc();
    } else {
        sanityCheck(false);
    }

    zvalue result = utilAlloc(sizeof(DatHeader) + extraBytes);
    result->mark  = liveColor;
    result->cls   = cls;

    allocationCount++;
    enlist(&liveHead, result);
    datFrameAdd(result);
    sanityCheck(false);

    if (DAT_CHATTY_GC) {
        liveCount++;
    }

    return result;
}

// Documented in header.
void assertValid(zvalue value) {
    if (value == NULL) {
        die("Null value.");
    }

    if (value->mark != liveColor) {
        die("Invalid value (wrong color): %p", value);
    }

    if (value->cls == NULL) {
        die("Invalid value (null class): %p", value);
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

    if (DAT_CHATTY_GC) {
        static double totalSec = 0;
        clock_t startTime = clock();

        note("GC: Cycle #%d.", gcCount);
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
zvalue datImmortalize(zvalue value) {
    if (immortalsSize == DAT_MAX_IMMORTALS) {
        die("Too many immortal values!");
    }

    assertValid(value);

    immortals[immortalsSize] = value;
    immortalsSize++;
    return value;
}

// Documented in header.
void datMark(zvalue value) {
    if (value == NULL) {
        return;
    }

    // Mark the value, and iterate to mark its class (and then metaclass,
    // etc.). The loop is needed since classes are not all immortal.
    for (/*value*/; value->mark != liveColor; value = value->cls) {
        value->mark = liveColor;
        enlist(&liveHead, value);
    }
}
