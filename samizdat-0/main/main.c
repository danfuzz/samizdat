/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "io.h"
#include "lang.h"
#include "lib.h"
#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Helper functions
 */

/**
 * Resolves the given possibly-relative path to an absolute one,
 * including resolving symbolic links. Returns a two-element listlet
 * of (first) the resolved path as a single stringlet, and (second) the
 * componentized path as a listlet of stringlets.
 *
 * This fails (terminating the runtime) if the resolved path turns out to
 * be non-absolute, to be too small to be useful, or to have a trailing slash.
 */
static zvalue resolvePath(char *path) {
    char resolved[FILENAME_MAX] = "";

    if (realpath(path, resolved) == NULL) {
        die("Error resolving path \"%s\": %s; trouble at \"%s\"",
            path, strerror(errno), resolved);
    }

    int resolvedLen = strlen(resolved);
    if ((resolvedLen < 2) ||
        (resolved[0] != '/') ||
        (resolved[resolvedLen - 1] == '/')) {
        die("Error resolving path \"%s\": invalid resolution \"%s\"",
            path, resolved);
    }

    // The full path.
    zvalue result = datListletAppend(
        datListletEmpty(),
        datStringletFromUtf8String(resolvedLen, resolved));

    // The components.
    zvalue components = datListletEmpty();
    char *at = resolved + 1; // +1 to skip the initial '/'.
    for (;;) {
        char *slashAt = strchr(at, '/');
        zint size = (slashAt != NULL) ? (slashAt - at) : strlen(at);

        components = datListletAppend(
            components,
            datStringletFromUtf8String(size, at));

        if (slashAt == NULL) {
            break;
        }

        at = slashAt + 1;
    }

    return datListletAppend(result, components);
}


/*
 * Main program
 */

/**
 * Main driver for Samizdat Layer 0. Takes the first argument to
 * be a file name. Reads the file, and compiles it to an executable
 * parse tree in an execution context consisting of the Samizdat Layer
 * 0 core library. It evaluates that tree, which should evaluate to
 * a function. It then calls that function, passing it its componentized
 * path as the first argument, followed by the remaining original arguments
 * as passed into this function.
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        die("Too few arguments.");
    }

    zvalue pathAndComponents = resolvePath(argv[1]);
    zvalue fileName = datListletNth(pathAndComponents, 0);
    zvalue args = datListletDelNth(pathAndComponents, 0);
    zint argCount = argc - 2;

    for (int i = 0; i < argCount; i++) {
        zvalue arg = datStringletFromUtf8String(-1, argv[i + 2]);
        args = datListletAppend(args, arg);
    }

    zcontext ctx = libNewContext();
    zvalue programText = ioReadFileUtf8(fileName);
    zvalue program = langNodeFromProgramText(programText);
    zvalue function = langEvalExpressionNode(ctx, program);
    zvalue result = langApply(function, args);

    if ((result != NULL) && (datType(result) == DAT_INTLET)) {
        exit((int) datIntFromIntlet(result));
    }
}
