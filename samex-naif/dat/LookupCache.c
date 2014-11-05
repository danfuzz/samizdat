// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <string.h>

#include "type/define.h"

#include "LookupCache.h"
#include "impl.h"


//
// Private Definitions
//

/** The cache. */
static LookupCacheEntry theCache[DAT_LOOKUP_CACHE_SIZE];


//
// Module Definitions
//

// Documented in header.
LookupCacheEntry *lookupCacheFind(zvalue container, zvalue key) {
    uintptr_t hash =
        ((uintptr_t) container >> 4) + (((uintptr_t) key) >> 4) * 31;
    hash ^= (hash >> 16) ^ (hash >> 32) ^ (hash >> 48);

    // Note: In practice there doesn't seem to be an observable performance
    // difference between using `&` and `%` to calculate the cache index, so
    // we go for `%` and a prime number cache size to get probably-better
    // cache behavior.
    LookupCacheEntry *entry = &theCache[hash % DAT_LOOKUP_CACHE_SIZE];

    if (DAT_CHATTY_LOOKUP_CACHE) {
        static int hits = 0;
        static int total = 0;
        if ((entry->container == container) && (entry->key == key)) {
            hits++;
        }
        total++;
        if ((total % 1000000) == 0) {
            note("Map Cache: Hit rate %d/%d == %5.2f%%", hits, total,
                (100.0 * hits) / total);
        }
    }

    return entry;
}


//
// Class Definition
//

// Documented in header.
METH_IMPL_0(LookupCache, gcMark) {
    memset(theCache, 0, sizeof(theCache));
    return NULL;
}

/** Initializes the module. */
MOD_INIT(LookupCache) {
    MOD_USE(Core);

    // What we're doing here is setting up a singleton instance, which
    // gets marked immortal. Its `gcMark` method gets called during gc,
    // which we use as a trigger to clear the cache.
    zvalue CLS_LookupCache = makeCoreClass(SYM(LookupCache), CLS_Core,
        NULL,
        METH_TABLE(
            METH_BIND(LookupCache, gcMark)));

    datImmortalize(datAllocValue(CLS_LookupCache, 0));
}
