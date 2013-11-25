/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "util.h"


static zvalue lookup(zvalue context, const char *name) {
    return collGet(context, stringFromUtf8(-1, name));
}

static void init(void) __attribute__((constructor));
static void init(void) {
    note("Test module: In `init()`.");
}

zvalue eval(zvalue context) {
    note("Test module: In `eval()`.");

    zvalue moduleUse = lookup(context, "moduleUse");

    zvalue langModuleName[2] = {
        stringFromUtf8(-1, "core"),
        stringFromUtf8(-1, "Lang2")
    };

    zvalue langModuleSpec =
        collPut(
            EMPTY_MAP,
            stringFromUtf8(-1, "name"),
            listFromArray(2, langModuleName));

    zvalue langModule = FUN_CALL(moduleUse, langModuleSpec);
    zvalue parseProgram = lookup(langModule, "parseProgram");
    zvalue evalFn = lookup(langModule, "eval");

    zvalue progStr = stringFromUtf8(-1, "<> {TEST: \"TEST\"}");

    note("Parsing...");
    zvalue progTree = FUN_CALL(parseProgram, progStr);
    note("Evalling...");
    zvalue evalResult = FUN_CALL(evalFn, context, progTree);
    note("Done!");

    return evalResult;
}
