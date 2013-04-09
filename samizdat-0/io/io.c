/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "io.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

enum {
    /** Maximum readable file size, in bytes. */
    MAX_FILE_SIZE = 100000
};


/*
 * Helper functions
 */

/**
 * Opens the file with the given name (a stringlet), and with the
 * given `fopen()` mode. Returns the `FILE *` handle.
 */
FILE *openFile(zvalue fileName, const char *mode) {
    zint nameSize = datStringletUtf8Size(fileName);
    char nameUtf[nameSize + 1];

    datStringletEncodeUtf8(fileName, nameUtf);
    nameUtf[nameSize] = '\0';

    FILE *file = fopen(nameUtf, "r");
    if (file == NULL) {
        die("Trouble opening file \"%s\": %s", nameUtf, strerror(errno));
    }

    return file;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue readFile(zvalue fileName) {
    char buf[MAX_FILE_SIZE];
    FILE *in = openFile(fileName, "r");
    size_t amt = fread(buf, 1, sizeof(buf), in);

    if (ferror(in)) {
        die("Trouble reading file: %s", strerror(errno));
    }

    if (!feof(in)) {
        die("Overlong file: %s", strerror(errno));
    }

    fclose(in);

    return datStringletFromUtf8String(buf, amt);
}

/* Documented in header. */
void writeFile(zvalue fileName, zvalue text) {
    zint textSize = datStringletUtf8Size(text);
    char textUtf[textSize];

    datStringletEncodeUtf8(text, textUtf);

    FILE *out = openFile(fileName, "r");
    zint amt = fwrite(textUtf, 1, textSize, out);

    if (amt != textSize) {
        die("Trouble writing file: %s", strerror(errno));
    }

    fclose(out);
}
