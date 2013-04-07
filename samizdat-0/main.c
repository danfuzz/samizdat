/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "sam-exec.h"
#include "tokenize.h"

#include <string.h>

/**
 * Processes a single file.
 */
static void processFile(zvalue fileContents) {
    // TODO: Remove this file dump.
    samNote("File contents:");
    zint size = samStringletUtf8Size(fileContents);
    char utf[size + 1];
    samStringletEncodeUtf8(fileContents, utf);
    utf[size] = '\0';
    samNote("%s", utf);
    samNote("[fin]");

    zvalue tokens = tokenize(fileContents);
}

/**
 * Main driver for Samizdat Layer 0. Reads in each file named as
 * an argument, parses it, and then executes the result.
 */
int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        samNote("Processing file: %s", argv[i]);

        zvalue name = samStringletFromUtf8String(argv[i], -1);
        zvalue fileContents = samReadFile(name);

        processFile(fileContents);
    }
}
