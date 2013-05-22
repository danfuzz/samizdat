/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "io.h"
#include "lang.h"
#include "lib.h"
#include "util.h"

#include <stdlib.h>


/*
 * Main program
 */

/**
 * Main driver for Samizdat Layer 0. This makes a library context, and
 * uses the `samCommandLine` function defined therein to do all the
 * real work.
 */
static void realMain(int argc, char **argv) {
    if (argc < 1) {
        die("Too few arguments.");
    }

    zvalue context = libNewContext();

    // The first argumengt to `samCommandLine` is the context. The
    // rest are the original command-line arguments (per se, so not
    // including C's `argv[0]`).

    zvalue args[argc];

    args[0] = context;
    for (int i = 1; i < argc; i++) {
        args[i] = datStringFromUtf8(-1, argv[i]);
    }
    zvalue argsList = datListFromArray(argc, args);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();

    zvalue samCommandLine = datMapGet(context, STR_SAM_COMMAND_LINE);
    zvalue result = langApply(samCommandLine, argsList);

    if ((result != NULL) && (datType(result) == DAT_INT)) {
        exit((int) datZintFromInt(result));
    }
}

/**
 * Recursively calls either this function or the "real" main function.
 * The recursion is done to (attempt to) guarantee that the compiler
 * won't try to inline the "real" main, which could mess up the stack
 * base calculation.
 */
static void recurseIntoRealMain(int recurseCount, int argc, char **argv) {
    if (recurseCount == 0) {
        realMain(argc, argv);
    } else {
        recurseIntoRealMain(recurseCount - 1, argc, argv);
    }
}

/**
 * Top-level main, which just does the requisite stack setup, and calls
 * into the real main function.
 */
int main(int argc, char **argv) {
    datSetStackBase(&argc);
    recurseIntoRealMain(1, argc, argv);
}
