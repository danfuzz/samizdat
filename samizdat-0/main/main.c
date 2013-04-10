/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "io.h"
#include "lang.h"
#include "lib.h"
#include "util.h"


/**
 * Main driver for Samizdat Layer 0. Reads in each file named as
 * an argument, parses it, and then executes the result.
 */
int main(int argc, char **argv) {
    zcontext ctx = libNewContext();
    
    for (int i = 1; i < argc; i++) {
        note("Processing file: %s", argv[i]);

        zvalue name = datStringletFromUtf8String(argv[i], -1);
        zvalue fileContents = readFile(name);
        zvalue program = langCompile(fileContents);

        langExecute(ctx, program);
    }
}
