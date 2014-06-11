// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

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


//
// Private Definitions
//

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
 * Loads and evaluates the named file if it exists (or fail trying). This
 * suffixes the name first with `.samb` to look for a binary file and then
 * with `.sam` for a text source file.
 */
static zvalue loadFile(zvalue path) {
    zvalue binPath = GFN_CALL(cat, path, stringFromUtf8(-1, ".samb"));
    zvalue func;

    if (valEq(ioFileType(binPath), STR_file)) {
        // We found a binary file.
        func = datEvalBinary(PRIMITIVE_ENVIRONMENT, binPath);
    } else {
        zvalue srcPath = GFN_CALL(cat, path, stringFromUtf8(-1, ".sam"));
        if (valEq(ioFileType(srcPath), STR_file)) {
            // We found a source text file.
            zvalue text = ioReadFileUtf8(srcPath);
            zvalue tree = langSimplify0(langParseProgram0(text), NULL);
            func = langEval0(PRIMITIVE_ENVIRONMENT, tree);
        } else {
            die("Missing bootstrap library file: %s", valDebugString(path));
        }
    }

    return FUN_CALL(func);
}

/**
 * Returns a map with all the core library bindings. This is the
 * return value from loading the top-level in-language library file `main`
 * and calling its `main` function.
 */
static zvalue getLibrary(zvalue libraryPath) {
    // Evaluate `ModuleSystem`. Works with either source or binary.
    zvalue moduleSystem = loadFile(
        GFN_CALL(cat, libraryPath,
            stringFromUtf8(-1, "/modules/core.ModuleSystem/main")));

    // Call `ModuleSystem::exportsmain` to load and evaluate the core library.
    zvalue mainFn = get(get(moduleSystem, STR_exports), STR_main);
    return FUN_CALL(mainFn, libraryPath, PRIMITIVE_ENVIRONMENT);
}


//
// Exported Definitions
//

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
