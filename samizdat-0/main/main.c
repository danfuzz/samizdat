/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "io.h"
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
 * uses the `samCommandLine` function defined therein to do all the
 * real work.
 */
int main(int argc, char **argv) {
    if (argc < 1) {
        die("Too few arguments.");
    }

    // TODO: Pass this in when creating the library.
    char *dir = getProgramDirectory(argv[0]);
    utilFree(dir);

    zstackPointer save = pbFrameStart();
    zvalue context = libNewContext();

    // The first argument to `samCommandLine` is the context. The
    // rest are the original command-line arguments (per se, so not
    // including C's `argv[0]`).

    zvalue args[argc];

    args[0] = context;
    for (int i = 1; i < argc; i++) {
        args[i] = stringFromUtf8(-1, argv[i]);
    }
    zvalue argsList = listFromArray(argc, args);

    // `argsList` refers to `context`, so it's safe to keep referring to
    // the latter, immediately below.
    pbFrameReturn(save, argsList);

    // Force a garbage collection here, to have a maximally clean slate when
    // moving into main program execution.
    pbGc();

    zvalue samCommandLine = collGet(context, STR_samCommandLine);
    zvalue result = funApply(samCommandLine, argsList);

    if ((result != NULL) && (hasType(result, TYPE_Int))) {
        exit((int) zintFromInt(result));
    }

    return 0;
}
