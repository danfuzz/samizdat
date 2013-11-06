/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "lib.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"
#include "util.h"

#include <stdlib.h>


/*
 * Main program
 */

/**
 * Main driver for Samizdat Layer 0. This makes a library context, and
 * uses the `CommandLine::run` function defined therein to do all the
 * real work.
 */
int main(int argc, char **argv) {
    if (argc < 1) {
        die("Too few arguments.");
    }

    char *libraryDir = getProgramDirectory(argv[0], "corelib");
    zvalue context = libNewContext(libraryDir);

    utilFree(libraryDir);

    // The arguments to `run` are the original command-line arguments (per se,
    // so not including C's `argv[0]`).
    zvalue args[argc - 1];
    for (int i = 1; i < argc; i++) {
        args[i - 1] = stringFromUtf8(-1, argv[i]);
    }
    zvalue argsList = listFromArray(argc - 1, args);

    zvalue runFunc = collGet(collGet(context, STR_CommandLine), STR_run);
    zvalue result = funApply(runFunc, argsList);

    if ((result != NULL) && (hasType(result, TYPE_Int))) {
        exit((int) zintFromInt(result));
    }

    return 0;
}
