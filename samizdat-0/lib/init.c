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
#include "util.h"


/*
 * Private Definitions
 */

/** Globals map (context) containing all the primitive definitions. */
static zvalue PRIMITIVE_CONTEXT = NULL;

/**
 * Posix-defined list of environment variables. This variable is not declared
 * in a standard header, surprisingly.
 */
extern char **environ;

/**
 * Makes an `ENVIRONMENT` map based on `environ` (that is, the list
 * of all environment variables passed into this process).
 */
static zvalue makeEnvironment(void) {
    zvalue result = EMPTY_MAP;

    for (char **envp = environ; *envp; envp++) {
        char *one = *envp;
        char *equalAt = strchr(one, '=');

        if (equalAt == NULL) {
            note("Odd environment line: %s", one);
            continue;
        }

        zvalue key = stringFromUtf8(equalAt - one, one);
        zvalue value = stringFromUtf8(-1, equalAt + 1);
        result = collPut(result, key, value);
    }

    return result;
}

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

    // Add a mapping for `ENVIRONMENT`.
    ctx = collPut(ctx, STR_ENVIRONMENT, makeEnvironment());

    // Set the final value, and make it immortal.
    PRIMITIVE_CONTEXT = ctx;
    pbImmortalize(PRIMITIVE_CONTEXT);
}

/**
 * Reads and evaluates the file whose name is the two given components,
 * concatenated with a `/` in the middle. Evaluation is done using the
 * primitive-only global context.
 */
static zvalue evalFile(zvalue directory, zvalue name) {
    zstackPointer save = pbFrameStart();

    zvalue path = GFN_CALL(cat, directory, STR_CH_SLASH, name);
    zvalue programText = ioFlatReadFileUtf8(path);
    zvalue programTree = langParseProgram0(programText);
    zvalue result = langEval0(PRIMITIVE_CONTEXT, programTree);

    pbFrameReturn(save, result);
    return result;
}

/**
 * Returns a map with all the core library bindings. This is the
 * return value from running the in-language library file `main`.
 */
static zvalue getLibrary(zvalue libraryDir) {
    zvalue mainFunction = evalFile(libraryDir, STR_main_sam);

    // It is the responsibility of the `main` core library program
    // to return the full set of core library bindings.
    return FUN_CALL(mainFunction, PRIMITIVE_CONTEXT, libraryDir);
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
