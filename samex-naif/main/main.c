// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "lib.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Class.h"
#include "util.h"

#include "impl.h"


//
// Main program
//

/**
 * Main driver for Samizdat Layer 0. This makes a library environment, and
 * uses the `CommandLine::runCommandLine` function defined therein to do
 * all the real work.
 */
int main(int argc, char **argv) {
    if (argc < 1) {
        die("Too few arguments.");
    }

    char *libraryDir = getProgramDirectory(argv[0], "corelib");
    zvalue env = libNewEnvironment(libraryDir);

    utilFree(libraryDir);

    // The arguments to `run` are the original command-line arguments (per se,
    // so not including C's `argv[0]`).
    zvalue args[argc - 1];
    for (int i = 1; i < argc; i++) {
        args[i - 1] = stringFromUtf8(-1, argv[i]);
    }
    zvalue argsList = listFromZarray((zarray) {argc - 1, args});

    zvalue runFunc = cm_get(env, SYM(runCommandLine));
    if (runFunc == NULL) {
        die("Missing `runCommandLine`.");
    }

    zvalue result = METH_APPLY(runFunc, call, argsList);

    if ((result != NULL) && (typeAccepts(CLS_Int, result))) {
        exit((int) zintFromInt(result));
    }

    return 0;
}
