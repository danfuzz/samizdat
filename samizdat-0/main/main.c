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
 * Main driver for Samizdat Layer 0. Takes the first argument to
 * be a file name. Reads the file, and compiles it to an executable
 * parse tree in an execution context consisting of the Samizdat Layer
 * 0 core library. It evaluates that tree, which should evaluate to
 * a function. It then calls that function, passing it the original
 * file's componentized path as the first argument, followed by the
 * remaining original arguments as passed into this function.
 */
static void realMain(int argc, char **argv) {
    if (argc < 2) {
        die("Too few arguments.");
    }

    zvalue pathList = ioPathListFromUtf8(argv[1]);
    zint argCount = argc - 1;
    zvalue args[argCount];

    args[0] = pathList;
    for (int i = 1; i < argCount; i++) {
        args[i] = datStringFromUtf8(-1, argv[i + 1]);
    }

    zvalue ctx = libNewContext();
    zvalue programText = ioReadFileUtf8(pathList);
    zvalue program = langNodeFromProgramText(programText);
    zvalue function = langEvalExpressionNode(ctx, program);

    datGc();

    zvalue result = langApply(function, datListFromArray(argCount, args));

    if ((result != NULL) && (datType(result) == DAT_INTEGER)) {
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
