/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
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
#include "util.h"


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
                makeBuiltin(minArgs, maxArgs, FUN_IMPL_NAME(name), 0, \
                    nameStr)); \
        } while(0)

    #define PRIM_DEF(name, value) \
        do { \
            zvalue nameStr = stringFromUtf8(-1, #name); \
            ctx = collPut(ctx, nameStr, value); \
        } while(0)

    #include "prim-def.h"

    // Set the final value, and make it immortal.
    PRIMITIVE_CONTEXT = ctx;
    datImmortalize(PRIMITIVE_CONTEXT);
}

/**
 * Loads the named binary file if it exists.
 */
static zvalue loadBinaryIfPossible(zvalue flatPath) {
    if (!ioFlatFileExists(flatPath)) {
        return NULL;
    }

    return datEvalBinary(PRIMITIVE_CONTEXT, flatPath);
};

/**
 * Reads and evaluates the named source file.
 */
static zvalue loadSource(zvalue flatPath) {
    if (!ioFlatFileExists(flatPath)) {
        die("Missing bootstrap library file: %s", valDebugString(flatPath));
    }

    zvalue text = ioFlatReadFileUtf8(flatPath);
    zvalue tree = langParseProgram0(text);
    return langEval0(PRIMITIVE_CONTEXT, tree);
};

/**
 * Returns a map with all the core library bindings. This is the
 * return value from loading the top-level in-language library file `main`
 * and calling its `main` function.
 */
static zvalue getLibrary(zvalue libraryFlatPath) {
    zstackPointer save = datFrameStart();
    zvalue libraryPath = ioSplitAbsolutePath(libraryFlatPath);

    // Evaluate `ModuleSystem`. Works with either source or binary.

    zvalue moduleSystemFn = loadBinaryIfPossible(
        GFN_CALL(cat, libraryFlatPath,
            stringFromUtf8(-1, "/modules/core.ModuleSystem/main.samb")));

    if (moduleSystemFn == NULL) {
        moduleSystemFn = loadSource(
            GFN_CALL(cat, libraryFlatPath,
                stringFromUtf8(-1, "/modules/core.ModuleSystem/main.sam")));
    }

    zvalue moduleSystem = FUN_CALL(moduleSystemFn);

    // Call `ModuleSystem::run` to load and evaluate the module, and call
    // the `main` function bound in the result.
    zvalue runFn = collGet(moduleSystem, STR_run);
    zvalue result = FUN_CALL(runFn,
        libraryPath, PRIMITIVE_CONTEXT, TOK_Null,
        libraryPath, PRIMITIVE_CONTEXT);

    datFrameReturn(save, result);
    return result;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue libNewContext(const char *libraryPath) {
    MOD_USE(const);
    MOD_USE(Box);
    MOD_USE(Generator);
    MOD_USE(Map);
    MOD_USE(lang);

    makePrimitiveContext();

    zstackPointer save = datFrameStart();
    zvalue result = getLibrary(stringFromUtf8(-1, libraryPath));

    datFrameReturn(save, result);

    // Force a garbage collection here, to have a maximally clean slate when
    // moving into main program execution.
    datGc();

    return result;
}
