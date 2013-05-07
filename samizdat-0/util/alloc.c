/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "util.h"
#include "zlimits.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
 * Helper definitions
 */

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
static PageRange ranges[UTIL_MAX_PAGE_RANGES];

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

    if (utilIsHeapAllocated((void *) range.start) &&
        utilIsHeapAllocated((void *) (range.end - 1))) {
        return;
    }

    // Need to add a new range or extend an existing one.

    if (rangesSize >= UTIL_MAX_PAGE_RANGES) {
        die("Too many heap page ranges!");
    }

    ranges[rangesSize] = range;
    rangesSize++;
    mergesort(ranges, rangesSize, sizeof(PageRange), compareRanges);

    // Combine adjacent ranges (if possible).

    zint at = 1;
    for (zint i = 1; i < rangesSize; i++) {
        PageRange *prev = &ranges[at - 1];
        PageRange *one = &ranges[i];

        if (prev->start == one->start) {
            if (prev->end < one->end) {
                prev->end = one->end;
            }
        } else if (prev->end >= one->start) {
            prev->end = one->end;
        } else {
            if (at != i) {
                ranges[at] = *one;
            }
            at++;
        }
    }

    rangesSize = at;
}


/*
 * Exported functions
 */

/* Documented in header. */
void *utilAlloc(zint size) {
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
void utilFree(void *memory) {
    free(memory);
}

/* Documented in header. */
bool utilIsHeapAllocated(void *memory) {
    intptr_t address = (intptr_t) memory;

    zint min = 0;
    zint max = rangesSize - 1;

    // Binary search for the range that covers the address.
    while (min <= max) {
        zint guess = (min + max) / 2;
        PageRange *one = &ranges[guess];

        if (one->start > address) {
            max = guess - 1;
        } else if (one->end <= address) {
            min = guess + 1;
        }

        if ((one->start <= address) && (one->end > address)) {
            return true;
        }
    }

    return false;
}
