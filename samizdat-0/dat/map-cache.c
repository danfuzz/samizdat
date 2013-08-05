/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper definitions
 */

enum {
    /** Whether to spew to the console about map cache hits. */
    CHATTY_CACHEY = false
};

/** The cache of `mapFind` lookups. */
static MapCacheEntry mapCache[DAT_MAP_CACHE_SIZE];


/*
 * Module functions
 */

/* Documented in header. */
void mapClearCache(void) {
    memset(mapCache, 0, sizeof(mapCache));
}

/* Documented in header. */
MapCacheEntry *mapGetCacheEntry(zvalue map, zvalue key) {
    uintptr_t hash = ((uintptr_t) map >> 4) + (((uintptr_t) key) >> 4) * 31;
    hash ^= (hash >> 16) ^ (hash >> 32) ^ (hash >> 48);

    // Note: In practice there doesn't seem to be an observable difference
    // between using `&` and `%` to calculate the cache index, so we go
    // for `%` and a prime number cache size to get probably-better cache
    // behavior.
    MapCacheEntry *entry = &mapCache[hash % DAT_MAP_CACHE_SIZE];

    if (CHATTY_CACHEY) {
        static int hits = 0;
        static int total = 0;
        if ((entry->map == map) && (entry->key == key)) {
            hits++;
        }
        total++;
        if ((total % 10000000) == 0) {
            note("Map Cache: Hit rate %d/%d == %5.2f%%", hits, total,
                (100.0 * hits) / total);
        }
    }

    return entry;
}
