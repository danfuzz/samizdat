// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <string.h>

#include "type/Generic.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

enum {
    /** Whether to spew to the console about map cache hits. */
    CHATTY_CACHEY = false
};

/** The cache of `mapFind` lookups. */
static MapCacheEntry mapCache[DAT_MAP_CACHE_SIZE];


//
// Module Definitions
//

/* Documented in header. */
MapCacheEntry *mapGetCacheEntry(zvalue map, zvalue key) {
    uintptr_t hash = ((uintptr_t) map >> 4) + (((uintptr_t) key) >> 4) * 31;
    hash ^= (hash >> 16) ^ (hash >> 32) ^ (hash >> 48);

    // Note: In practice there doesn't seem to be an observable performance
    // difference between using `&` and `%` to calculate the cache index, so
    // we go for `%` and a prime number cache size to get probably-better
    // cache behavior.
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


//
// Type Definition
//

/* Documented in header. */
METH_IMPL(MapCache, gcMark) {
    memset(mapCache, 0, sizeof(mapCache));
    return NULL;
}

/** Initializes the module. */
MOD_INIT(MapCache) {
    MOD_USE(Value);

    // What we're doing here is setting up a singleton instance, which
    // gets marked immortal. Its `gcMark` method gets called during gc,
    // which we use as a trigger to clear the map cache.
    zvalue TYPE_MapCache =
        makeCoreType(stringFromUtf8(-1, "MapCache"), TYPE_Value, true);
    METH_BIND(MapCache, gcMark);
    datImmortalize(datAllocValue(TYPE_MapCache, 0));
}
