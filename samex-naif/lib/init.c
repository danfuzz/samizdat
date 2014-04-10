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
#include "type/Data.h"
#include "type/DerivedData.h"
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

/** Globals map (environment) containing all the primitive definitions. */
static zvalue PRIMITIVE_ENVIRONMENT = NULL;

/**
 * Sets up `PRIMITIVE_ENVIRONMENT`, if not already done.
 */
static void makePrimitiveEnvironment(void) {
    if (PRIMITIVE_ENVIRONMENT != NULL) {
        return;
    }

    zvalue env = EMPTY_MAP;

    // Bind all the primitive functions.

    #define PRIM_FUNC(name, minArgs, maxArgs) \
        do { \
            zvalue nameStr = stringFromUtf8(-1, #name); \
            env = collPut(env, nameStr, \
                makeBuiltin(minArgs, maxArgs, FUN_IMPL_NAME(name), 0, \
                    nameStr)); \
        } while(0)

    #define PRIM_DEF(name, value) \
        do { \
            zvalue nameStr = stringFromUtf8(-1, #name); \
            env = collPut(env, nameStr, value); \
        } while(0)

    #include "prim-def.h"

    // Set the final value, and make it immortal.
    PRIMITIVE_ENVIRONMENT = env;
    datImmortalize(PRIMITIVE_ENVIRONMENT);
}

/**
 * Loads the named binary file if it exists.
 */
static zvalue loadBinaryIfPossible(zvalue path) {
    if (!valEqNullOk(ioFileType(path), STR_file)) {
        return NULL;
    }

    return datEvalBinary(PRIMITIVE_ENVIRONMENT, path);
};

/**
 * Reads and evaluates the named source file.
 */
static zvalue loadSource(zvalue path) {
    if (!valEqNullOk(ioFileType(path), STR_file)) {
        die("Missing bootstrap library file: %s", valDebugString(path));
    }

    zvalue text = ioReadFileUtf8(path);
    zvalue tree = langParseProgram0(text);
    return langEval0(PRIMITIVE_ENVIRONMENT, tree);
};

/**
 * Returns a map with all the core library bindings. This is the
 * return value from loading the top-level in-language library file `main`
 * and calling its `main` function.
 */
static zvalue getLibrary(zvalue libraryPath) {
    zstackPointer save = datFrameStart();

    // Evaluate `ModuleSystem`. Works with either source or binary.

    zvalue moduleSystemFn = loadBinaryIfPossible(
        GFN_CALL(cat, libraryPath,
            stringFromUtf8(-1, "/modules/core.ModuleSystem/main.samb")));

    if (moduleSystemFn == NULL) {
        moduleSystemFn = loadSource(
            GFN_CALL(cat, libraryPath,
                stringFromUtf8(-1, "/modules/core.ModuleSystem/main.sam")));
    }

    zvalue moduleSystem = FUN_CALL(moduleSystemFn);

    // Call `$ModuleSystem::run` to load and evaluate the module, and call
    // the `main` function bound in the result.
    zvalue runFn = get(moduleSystem, STR_run);
    zvalue result = FUN_CALL(runFn,
        libraryPath, PRIMITIVE_ENVIRONMENT, TOK_Null,
        libraryPath, PRIMITIVE_ENVIRONMENT);

    datFrameReturn(save, result);
    return result;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue libNewEnvironment(const char *libraryPath) {
    MOD_USE(const);
    MOD_USE(Box);
    MOD_USE(Generator);
    MOD_USE(Map);
    MOD_USE(lang);

    makePrimitiveEnvironment();

    zstackPointer save = datFrameStart();
    zvalue result = getLibrary(stringFromUtf8(-1, libraryPath));

    datFrameReturn(save, result);

    // Force a garbage collection here, to have a maximally clean slate when
    // moving into main program execution.
    datGc();

    return result;
}
