/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "sam-exec.h"

#include <string.h>

/**
 * Main driver for Samizdat Layer 0. Reads in each file named as
 * an argument, parses it, and then executes the result.
 */
int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        samNote("File \"%s\":", argv[i]);
        zvalue name =
            samStringletFromUtf8String((zbyte *) argv[i], strlen(*argv));

        samNote("Reading file...");
        zvalue file = samReadFile(name);

        // TODO: Something real.
        samNote("File contents:");

        zint size = samStringletUtf8Size(file);
        zbyte utf[size + 1];
        samStringletEncodeUtf8(file, utf);
        utf[size] = '\0';
        samNote("%s", utf);
        samNote("[fin]");
    }
}
