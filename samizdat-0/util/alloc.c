/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
 * Helper definitions
 */

enum {
    /** Maximum number of disjoint page ranges allowed. */
    MAX_PAGE_RANGES = 20
};

/**
 * Page address range. Each range indicates a section of memory that
 * has been known to have heap allocations in it. The bounds are always page
 * boundaries.
 */
typedef struct AddressRange {
    /** Start (inclusive). */
    intptr_t start;

    /** End (exclusive). */
    intptr_t end;
} PageRange;

/** Page size.  */
static intptr_t PAGE_SIZE = 0;

/** Mask for clipping addresses to page boundaries.  */
static intptr_t PAGE_MASK = 0;

/** Array of observed page ranges. */
static PageRange ranges[MAX_PAGE_RANGES];

/** Number of active page ranges. */
static zint rangesSize = 0;

/**
 * Convert address range to page range.
 */
static PageRange pageRangeFromAddressRange(void *startPtr, void *endPtr) {
    if (PAGE_MASK == 0) {
        PAGE_SIZE = getpagesize();
        PAGE_MASK = ~(PAGE_SIZE - 1);
    }

    intptr_t start = ((intptr_t) startPtr) & PAGE_MASK;
    intptr_t end = ((intptr_t) endPtr + PAGE_SIZE - 1) & PAGE_MASK;

    PageRange result = { start, end };
    return result;
}

/**
 * Comparison function for `PageRange`s.
 */
static int compareRanges(const void *r1, const void *r2) {
    const PageRange *range1 = r1;
    const PageRange *range2 = r2;

    if (range1->start < range2->start) {
        return -1;
    } else if (range1->start > range2->start) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Adds the page(s) of the given address range to the list of known-active
 * pages.
 */
static void addPages(void *start, void *end) {
    PageRange range = pageRangeFromAddressRange(start, end);
    bool needNew = true;

    for (zint i = 0; i < rangesSize; i++) {
        PageRange *one = &ranges[i];
        if ((range.end < one->start) || (range.start > one->end)) {
            // Totally disjoint with this one.
            continue;
        }

        bool changed = false;

        if (range.start < one->start) {
            one->start = range.start;
            changed = true;
        }

        if (range.end > one->end) {
            one->end = range.end;
            changed = true;
        }

        if (!changed) {
            return;
        }

        needNew = false;
        break;
    }

    if (needNew) {
        // Need to add a new range.
        if (rangesSize >= MAX_PAGE_RANGES) {
            die("Too many heap page ranges!");
        }

        ranges[rangesSize] = range;
        rangesSize++;
        mergesort(ranges, rangesSize, sizeof(PageRange), compareRanges);
        note("=== now %lld", rangesSize);
    }

    // Combine adjacent ranges (if any).

    zint at = 1;
    for (zint i = 1; i < rangesSize; i++) {
        PageRange *prev = &ranges[at - 1];
        PageRange *one = &ranges[i];

        if (prev->end == one->start) {
            prev->end = one->end;
        } else {
            ranges[at] = *one;
            at++;
        }
    }

    rangesSize = at;
}



/** Lowest observed (inclusive) valid heap address. */
static void *heapBottom = NULL;

/** Highest observed (exclusive) valid heap address. */
static void *heapTop = NULL;


/*
 * Exported functions
 */

/* Documented in header. */
void *zalloc(zint size) {
    if (size < 0) {
        die("Invalid allocation size: %lld", size);
    }

    void *result = malloc(size);

    if (result == NULL) {
        die("Failed: malloc(%#llx).", size);
    }

    memset(result, 0, size);
    addPages(result, ((char *) result) + size);

    return result;
}

/* Documented in header. */
void zfree(void *memory) {
    free(memory);
}

/* Documented in header. */
bool utilIsHeapAllocated(void *memory) {
    intptr_t address = (intptr_t) memory;

    for (zint i = 0; i < rangesSize; i++) {
        PageRange *one = &ranges[i];
        if ((address >= one->start) && (address < one->end)) {
            return true;
        }
    }

    return false;
}
