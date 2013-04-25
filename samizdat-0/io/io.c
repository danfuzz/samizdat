/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
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
    const char *nameUtf = datStringletEncodeUtf8(fileName, NULL);

    FILE *file = fopen(nameUtf, mode);
    if (file == NULL) {
        die("Trouble opening file \"%s\": %s", nameUtf, strerror(errno));
    }

    return file;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue ioReadFile(zvalue fileName) {
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

    return datStringletFromUtf8String(amt, buf);
}

/* Documented in header. */
void ioWriteFile(zvalue fileName, zvalue text) {
    zint utfSize;
    const char *utf = datStringletEncodeUtf8(text, &utfSize);

    FILE *out = openFile(fileName, "w");
    zint amt = fwrite(utf, 1, utfSize, out);

    if (amt != utfSize) {
        die("Trouble writing file: %s", strerror(errno));
    }

    fclose(out);
}
