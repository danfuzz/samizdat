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
int main(int argc, char **argv) {
    if (argc < 2) {
        die("Too few arguments.");
    }

    zvalue pathListlet = ioPathListletFromUtf8(argv[1]);
    zvalue args = datListletAppend(EMPTY_LISTLET, pathListlet);
    zint argCount = argc - 2;

    for (int i = 0; i < argCount; i++) {
        zvalue arg = datStringletFromUtf8String(-1, argv[i + 2]);
        args = datListletAppend(args, arg);
    }

    zcontext ctx = libNewContext();
    zvalue programText = ioReadFileUtf8(pathListlet);
    zvalue program = langNodeFromProgramText(programText);
    zvalue function = langEvalExpressionNode(ctx, program);
    zvalue result = langApply(function, args);

    if ((result != NULL) && (datType(result) == DAT_INTLET)) {
        exit((int) datIntFromIntlet(result));
    }
}
