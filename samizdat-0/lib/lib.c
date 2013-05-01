/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/**
 * Maplet of all core library bindings, set up by `initLibraryBindings()`.
 * Bindings include both primitive and in-language definitions.
 */
static zvalue LIBRARY_BINDINGS = NULL;

// Declarations for all the embedded library source files.
#define LIB_FILE(name) \
    extern char name##_sam0[]; \
    extern unsigned int name##_sam0_len; \
    static zvalue LIB_NAME_##name = NULL; \
    static zvalue LIB_TEXT_##name = NULL
#include "lib-def.h"
#undef LIB_FILE

/**
 * Sets up the mapping from names to library file contents.
 * This is what ends up bound to `LIBRARY_FILES` in the main entry
 * of `samizdat-0-lib`.
 */
static zvalue getLibraryFiles(void) {
    zvalue result = EMPTY_MAPLET;

    // This adds an element to `result` for each of the embedded files,
    // and sets up the static name constants.
    #define LIB_FILE(name) \
        LIB_NAME_##name = datStringletFromUtf8(-1, #name); \
        LIB_TEXT_##name = \
            datStringletFromUtf8(name##_sam0_len, name##_sam0); \
        result = datMapletPut(result, LIB_NAME_##name, LIB_TEXT_##name)
    #include "lib-def.h"
    #undef LIB_FILE

    return result;
}

/**
 * Creates a `zcontext`, and binds all the primitive definitions into it.
 */
static zcontext primitiveContext(void) {
    zcontext ctx = langCtxNew();

    // These both could have been defined in-language, but we already
    // have to make them be defined and accessible to C code, so we just
    // go ahead and bind them here.
    langCtxBind(ctx, STR_FALSE, CONST_FALSE);
    langCtxBind(ctx, STR_TRUE, CONST_TRUE);

    // Bind all the primitive functions.
    #define PRIM_FUNC(name) langCtxBindFunction(ctx, #name, prim_##name, NULL)
    #include "prim-def.h"

    // Include a binding for a maplet of all the primitive bindings
    // (other than this one, since values can't self-reference).
    langCtxBind(ctx, STR_UP_LIBRARY, langMapletFromCtx(ctx));

    return ctx;
}

/**
 * Sets up `LIBRARY_BINDINGS`.
 */
static void initLibraryBindings(void) {
    if (LIBRARY_BINDINGS != NULL) {
        return;
    }

    zvalue libraryFiles = getLibraryFiles();
    zcontext ctx = primitiveContext();
    zvalue mainProgram = langNodeFromProgramText(LIB_TEXT_main);
    zvalue mainFunction = langEvalExpressionNode(ctx, mainProgram);

    // It is the responsibility of the `main` core library program
    // to return the full set of core library bindings.
    LIBRARY_BINDINGS = langCall(mainFunction, 1, &libraryFiles);
}


/*
 * Exported functions
 */

/* Documented in header. */
zcontext libNewContext(void) {
    constInit();
    initLibraryBindings();

    zcontext result = langCtxNew();

    langCtxBindAll(result, LIBRARY_BINDINGS);
    return result;
}
