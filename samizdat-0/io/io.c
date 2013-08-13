/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "io.h"
#include "util.h"
#include "zlimits.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


/*
 * Helper Definitions
 */

/**
 * Opens the file with the given name (a string), and with the
 * given `fopen()` mode. Returns the `FILE *` handle.
 */
static FILE *openFile(zvalue flatPath, const char *mode) {
    zint pathSize = utf8SizeFromString(flatPath);
    char path[pathSize + 1];
    utf8FromString(pathSize + 1, path, flatPath);

    FILE *file = fopen(path, mode);
    if (file == NULL) {
        die("Trouble opening file \"%s\": %s", path, strerror(errno));
    }

    return file;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue ioFlatCwd(void) {
    // The maximum buffer size is determined per the recommendation
    // in the Posix docs for `getcwd`.

    long maxSize = pathconf(".", _PC_PATH_MAX);
    char buf[maxSize + 1];

    if (getcwd(buf, maxSize) == NULL) {
        die("Trouble with getcwd(): %s", strerror(errno));
    }

    return stringFromUtf8(-1, buf);
}

/* Documented in header. */
zvalue ioFlatReadLink(zvalue flatPath) {
    zint pathSize = utf8SizeFromString(flatPath);
    char path[pathSize + 1];
    utf8FromString(pathSize + 1, path, flatPath);

    struct stat statBuf;
    if (lstat(path, &statBuf) != 0) {
        if ((errno == ENOENT) || (errno == ENOTDIR)) {
            // File not found or invalid component, neither of which
            // are really errors from the perspective of this function.
            return NULL;
        }
        die("Trouble with lstat(): %s", strerror(errno));
    }

    if (!S_ISLNK(statBuf.st_mode)) {
        // Not a symlink.
        return NULL;
    }

    // The required link buffer size is determined per the (oblique)
    // recommendation in the Posix docs for `readlink`.

    size_t linkSize = statBuf.st_size;
    char linkPath[linkSize];
    ssize_t linkResult = readlink(path, linkPath, linkSize);
    if (linkResult < 0) {
        die("Trouble with readlink(): %s", strerror(errno));
    } else if (linkResult != linkSize) {
        die("Strange readlink() result: %ld", (long) linkResult);
    }

    return stringFromUtf8(linkSize, linkPath);
}

/* Documented in header. */
zvalue ioFlatReadFileUtf8(zvalue flatPath) {
    char buf[IO_MAX_FILE_SIZE];
    FILE *in = openFile(flatPath, "r");
    size_t amt = fread(buf, 1, sizeof(buf), in);

    if (ferror(in)) {
        die("Trouble reading file: %s", strerror(errno));
    }

    if (!feof(in)) {
        die("Overlong file: %s", strerror(errno));
    }

    fclose(in);

    return stringFromUtf8(amt, buf);
}

/* Documented in header. */
void ioFlatWriteFileUtf8(zvalue flatPath, zvalue text) {
    zint utfSize = utf8SizeFromString(text);
    char utf[utfSize + 1];
    utf8FromString(utfSize + 1, utf, text);

    FILE *out = openFile(flatPath, "w");
    zint amt = fwrite(utf, 1, utfSize, out);

    if (amt != utfSize) {
        die("Trouble writing file: %s", strerror(errno));
    }

    fclose(out);
}
