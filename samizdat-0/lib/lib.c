/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/**
 * Sets up the mapping from names to library file contents.
 * This is what ends up bound to `LIBRARY_FILES` in the main entry
 * of `samizdat-0-lib`.
 */
static zvalue getLibraryFiles(void) {
    zvalue result = EMPTY_MAP;

    // This adds an element to `result` for each of the embedded files,
    // and sets up the static name constants.
    #define LIB_FILE(name) \
        extern char name##_sam0[]; \
        extern unsigned int name##_sam0_len; \
        zvalue LIB_NAME_##name = datStringFromUtf8(-1, #name ".sam0"); \
        zvalue LIB_TEXT_##name = \
            datStringFromUtf8(name##_sam0_len, name##_sam0); \
        result = datMapPut(result, LIB_NAME_##name, LIB_TEXT_##name)
    #include "lib-def.h"
    #undef LIB_FILE

    return result;
}

/**
 * Creates a context map with all the primitive definitions bound into it.
 */
static zvalue primitiveContext(void) {
    zvalue ctx = EMPTY_MAP;

    // These both could have been defined in-language, but we already
    // have to make them be defined and accessible to C code, so we just
    // go ahead and bind them here.
    ctx = datMapPut(ctx, STR_FALSE, CONST_FALSE);
    ctx = datMapPut(ctx, STR_TRUE, CONST_TRUE);

    // Bind all the primitive functions.
    #define PRIM_FUNC(name) \
        ctx = datMapPut(ctx, \
                        datStringFromUtf8(-1, #name), \
                        langDefineFunction(prim_##name, NULL))
    #include "prim-def.h"

    // Include a binding for a map of all the primitive bindings
    // (other than this one, since values can't self-reference).
    ctx = datMapPut(ctx, STR_UP_LIBRARY, ctx);

    return ctx;
}

/**
 * Returns a map with all the core library bindings. This is the
 * return value from running the in-language library `main`.
 */
static zvalue getLibrary(void) {
    zvalue libraryFiles = getLibraryFiles();
    zvalue mainText = datMapGet(libraryFiles, STR_MAIN_SAM0);
    zvalue mainProgram = langTree0(mainText);

    zvalue ctx = primitiveContext();
    zvalue mainFunction = langEval0(ctx, mainProgram);

    // It is the responsibility of the `main` core library program
    // to return the full set of core library bindings.
    return langCall(mainFunction, 1, &libraryFiles);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue libNewContext(void) {
    constInit();
    return getLibrary();
}
