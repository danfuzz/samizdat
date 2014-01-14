/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#ifndef _DAT_TYPE_H_
#define _DAT_TYPE_H_

/**
 * Arbitrary value. The contents of a value are *not* directly
 * accessible through instances of this type via the API. You
 * have to use the various accessor functions.
 */
typedef struct DatHeader *zvalue;

/** Type for local value stack pointers. */
typedef zvalue *zstackPointer;

enum {
    /**
     * (Private) Size of the `DatHeader` struct; used so that `datPayload`
     * can be an inline function.
     */
    DAT_HEADER_SIZE =
        (sizeof(zvalue) * 3) + (sizeof(int32_t) * 2) + sizeof(zint)
};

#endif
