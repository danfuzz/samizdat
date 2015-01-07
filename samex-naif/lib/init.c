// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "impl.h"
#include "io.h"
#include "lang.h"
#include "type/Bool.h"
#include "type/Box.h"
#include "type/Builtin.h"
#include "type/Class.h"
#include "type/Cmp.h"
#include "type/Core.h"
#include "type/Generator.h"
#include "type/If.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Null.h"
#include "type/Object.h"
#include "type/Record.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "type/String.h"
#include "type/define.h"
#include "util.h"


//
// Private Definitions
//

/** Environment (a symbol table) containing all the primitive definitions. */
static zvalue PRIMITIVE_ENVIRONMENT = NULL;

/**
 * Sets up `PRIMITIVE_ENVIRONMENT`, if not already done.
 */
static void makePrimitiveEnvironment(void) {
    if (PRIMITIVE_ENVIRONMENT != NULL) {
        return;
    }

    // Count the definitions.

    zint size = 0;

    #define PRIM_DEF(name, value) size++
    #define PRIM_FUNC(name, minArgs, maxArgs) size++
    #include "prim-def.h"
    #undef PRIM_DEF
    #undef PRIM_FUNC

    // Make an environment with them all.

    zmapping defs[size];
    size = 0;

    #define PRIM_DEF(name, value) \
        do { \
            zvalue nameSymbol = symbolFromUtf8(-1, #name); \
            defs[size] = (zmapping) {nameSymbol, value}; \
            size++; \
        } while(0)

    #define PRIM_FUNC(name, minArgs, maxArgs) \
        do { \
            zvalue nameSymbol = symbolFromUtf8(-1, #name); \
            zvalue value = makeBuiltin(minArgs, maxArgs, FUN_IMPL_NAME(name), \
                0, nameSymbol); \
            defs[size] = (zmapping) {nameSymbol, value}; \
            size++; \
        } while(0)

    #include "prim-def.h"

    // Set the final value, and make it immortal.
    PRIMITIVE_ENVIRONMENT =
        datImmortalize(symtabFromZassoc((zassoc) {size, defs}));
}

/**
 * Loads and evaluates the named file if it exists (or fail trying). This
 * suffixes the name first with `.samb` to look for a binary file and then
 * with `.sam` for a text source file.
 */
static zvalue loadFile(zvalue path) {
    zvalue binPath = cm_cat(path, stringFromUtf8(-1, ".samb"));
    zvalue func;

    if (cmpEq(ioFileType(binPath, true), SYM(file))) {
        // We found a binary file.
        func = datEvalBinary(PRIMITIVE_ENVIRONMENT, binPath);
    } else {
        zvalue srcPath = cm_cat(path, stringFromUtf8(-1, ".sam"));
        if (cmpEq(ioFileType(srcPath, true), SYM(file))) {
            // We found a source text file.
            zvalue text = ioReadFileUtf8(srcPath);
            zvalue tree = langSimplify0(langParseProgram0(text), NULL);
            func = langEval0(PRIMITIVE_ENVIRONMENT, tree);
        } else {
            die("Missing bootstrap library file: %s", cm_debugString(path));
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
        cm_cat(libraryPath,
            stringFromUtf8(-1, "/modules/core.ModuleSystem/main")));

    // Call `ModuleSystem::exports::main` to load and evaluate the
    // core library.

    zvalue exports = cm_get(moduleSystem, SYM(exports));
    if (exports == NULL) {
        die("Missing bootstrap `exports` binding.");
    }

    zvalue mainFn = cm_get(exports, SYM(main));
    if (mainFn == NULL) {
        die("Missing bootstrap `main` binding");
    }

    return FUN_CALL(mainFn, libraryPath, PRIMITIVE_ENVIRONMENT);
}


//
// Exported Definitions
//

// Documented in header.
zvalue libNewEnvironment(const char *libraryPath) {
    MOD_USE(lib);

    zstackPointer save = datFrameStart();
    zvalue result = getLibrary(stringFromUtf8(-1, libraryPath));

    datFrameReturn(save, result);

    // Force a garbage collection here, to have a maximally clean slate when
    // moving into main program execution.
    datGc();

    return result;
}

/** Initializes the module. */
MOD_INIT(lib) {
    MOD_USE(Value);
    MOD_USE(cls);
    MOD_USE(io);
    MOD_USE(lang);

    SYM_INIT(runCommandLine);

    makePrimitiveEnvironment();
}

// Documented in header.
SYM_DEF(runCommandLine);
