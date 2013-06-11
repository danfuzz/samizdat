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
int main(int argc, char **argv) {
    if (argc < 1) {
        die("Too few arguments.");
    }

    zstackPointer save = datFrameStart();
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

    // `argsList` refers to `context`, so it's safe to keep referring to
    // the latter, immediately below.
    datFrameReturn(save, argsList);

    // Force a garbage collection here, mainly to get a reasonably early
    // failure if gc is broken.
    datGc();

    zvalue samCommandLine = datMapGet(context, STR_SAM_COMMAND_LINE);
    zvalue result = langApply(samCommandLine, argsList);

    if ((result != NULL) && (datType(result) == DAT_INT)) {
        exit((int) datZintFromInt(result));
    }

    return 0;
}
