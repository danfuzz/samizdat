/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "io.h"
#include "lang.h"
#include "type/Bitwise.h"
#include "type/Box.h"
#include "type/Builtin.h"
#include "type/Collection.h"
#include "type/Function.h"
#include "type/Generator.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Number.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Uniqlet.h"
#include "type/Value.h"

#include "util.h" // TEMP! TODO: Remove me.

/*
 * Private Definitions
 */

/** Globals map (context) containing all the primitive definitions. */
static zvalue PRIMITIVE_CONTEXT = NULL;

/**
 * Sets up `PRIMITIVE_CONTEXT`, if not already done.
 */
static void makePrimitiveContext(void) {
    if (PRIMITIVE_CONTEXT != NULL) {
        return;
    }

    zvalue ctx = EMPTY_MAP;

    // Bind all the primitive functions.

    #define PRIM_FUNC(name, minArgs, maxArgs) \
        do { \
            zvalue nameStr = stringFromUtf8(-1, #name); \
            ctx = collPut(ctx, nameStr, \
                makeBuiltin(minArgs, maxArgs, PRIM_##name, nameStr)); \
        } while(0)

    #define PRIM_DEF(name, value) \
        do { \
            zvalue nameStr = stringFromUtf8(-1, #name); \
            ctx = collPut(ctx, nameStr, value); \
        } while(0)

    #include "prim-def.h"

    // Include a mapping for a map of all the primitive bindings
    // (other than this one, since values can't self-reference).
    ctx = collPut(ctx, STR_LIBRARY, ctx);

    // Set the final value, and make it immortal.
    PRIMITIVE_CONTEXT = ctx;
    pbImmortalize(PRIMITIVE_CONTEXT);
}

/**
 * Reads a file whose name is the two given components, concatenated with
 * a `/` in the middle.
 */
static zvalue readFile(zvalue directory, zvalue name) {
    zvalue path = GFN_CALL(cat, directory, STR_CH_SLASH, name);
    return ioFlatReadFileUtf8(path);
}

/**
 * Reads and evaluates the file whose name is the two given components,
 * concatenated with a `/` in the middle. Evaluation is done using the
 * primitive-only global context.
 */
static zvalue evalFile(zvalue directory, zvalue name) {
    zstackPointer save = pbFrameStart();

    zvalue programText = readFile(directory, name);
    zvalue programTree = langParseProgram0(programText);
    zvalue result = langEval0(PRIMITIVE_CONTEXT, programTree);

    pbFrameReturn(save, result);
    return result;
}

/**
 * Sets up the mapping from names to library file contents.
 * This is what ends up bound to `LIBRARY_FILES` in the main entry
 * of `samizdat-0-lib`.
 */
static zvalue getLibraryFiles(zvalue libraryDir) {
    zvalue result = EMPTY_MAP;

    // Read the manifest file, and evaluate it. It evaluates to a map
    // that binds `BOOTSTRAP_FILES` and `MANIFEST`, and those list all
    // the ultimate library files.
    zvalue manifestName = stringFromUtf8(-1, MANIFEST_FILE_NAME);
    zvalue manifestMap = FUN_CALL(evalFile(libraryDir, manifestName));
    zvalue allFiles = GFN_CALL(cat,
        collGet(manifestMap, STR_BOOTSTRAP_FILES),
        collGet(manifestMap, STR_MANIFEST));

    for (zint i = 0; i < collSize(allFiles); i++) {
        note("=== %s", valDebugString(collNth(allFiles, i)));
    }
    // TODO: Use the manifest.

    // This adds an element to `result` for each of the embedded files,
    // and sets up the static name constants.
    #define LIB_FILE(name, ext) do { \
        extern unsigned int name##_##ext##_len; \
        extern char name##_##ext[]; \
        unsigned int len = name##_##ext##_len; \
        char *text = name##_##ext; \
        zvalue datName = stringFromUtf8(-1, #name "." #ext); \
        zvalue datText = stringFromUtf8(len, text); \
        result = collPut(result, datName, datText); \
    } while(0)

    #include "lib-def.h"
    #undef LIB_FILE

    return result;
}

/**
 * Returns a map with all the core library bindings. This is the
 * return value from running the in-language library `main`.
 */
static zvalue getLibrary(zvalue libraryDir) {
    zstackPointer save = pbFrameStart();

    zvalue libraryFiles = getLibraryFiles(libraryDir);
    zvalue mainText = collGet(libraryFiles, STR_main_sam0);
    zvalue mainProgram = langParseProgram0(mainText);

    zvalue mainFunction = langEval0(PRIMITIVE_CONTEXT, mainProgram);

    pbFrameReturn(save, mainFunction);

    // It is the responsibility of the `main` core library program
    // to return the full set of core library bindings.
    return FUN_CALL(mainFunction, libraryFiles);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue libNewContext(const char *libraryDir) {
    MOD_USE(const);
    MOD_USE(Box);
    MOD_USE(Generator);
    MOD_USE(Map);

    makePrimitiveContext();
    return getLibrary(stringFromUtf8(-1, libraryDir));
}
