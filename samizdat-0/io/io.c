/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "io.h"
#include "util.h"
#include "zlimits.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


/*
 * Helper definitions
 */

/**
 * Converts a list-of-strings path form into an absolute file name,
 * checking the components for sanity.
 */
static zvalue stringFromPathList(zvalue pathList) {
    zvalue result = STR_EMPTY;
    zint size = datSize(pathList);

    for (zint i = 0; i < size; i++) {
        zvalue one = datListNth(pathList, i);
        zint size = datUtf8SizeFromString(one);
        char str[size + 1];
        datUtf8FromString(size + 1, str, one);

        if (strlen(str) != size) {
            die("Invalid path component (contains null bytes): \"%s\"", str);
        } else if (strchr(str, '/') != NULL) {
            die("Invalid path component (contains slash): \"%s\"", str);
        } else if (datEq(one, STR_EMPTY) ||
            datEq(one, STR_CH_DOT) ||
            datEq(one, STR_CH_DOTDOT)) {
            die("Invalid path component: \"%s\"", str);
        }

        result = datStringAdd(result, STR_CH_SLASH);
        result = datStringAdd(result, one);
    }

    return result;
}

/**
 * Opens the file with the given name (a string), and with the
 * given `fopen()` mode. Returns the `FILE *` handle.
 */
static FILE *openFile(zvalue pathList, const char *mode) {
    zvalue pathString = stringFromPathList(pathList);
    zint pathSize = datUtf8SizeFromString(pathString);
    char path[pathSize + 1];
    datUtf8FromString(pathSize + 1, path, pathString);

    FILE *file = fopen(path, mode);
    if (file == NULL) {
        die("Trouble opening file \"%s\": %s", path, strerror(errno));
    }

    return file;
}


/*
 * Exported functions
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

    return datStringFromUtf8(-1, buf);
}

/* Documented in header. */
zvalue ioFlatReadLink(zvalue flatPath) {
    constInit();

    zint pathSize = datUtf8SizeFromString(flatPath);
    char path[pathSize + 1];
    datUtf8FromString(pathSize + 1, path, flatPath);

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

    return datStringFromUtf8(linkSize, linkPath);
}

/* Documented in header. */
zvalue ioReadFileUtf8(zvalue pathList) {
    constInit();

    char buf[IO_MAX_FILE_SIZE];
    FILE *in = openFile(pathList, "r");
    size_t amt = fread(buf, 1, sizeof(buf), in);

    if (ferror(in)) {
        die("Trouble reading file: %s", strerror(errno));
    }

    if (!feof(in)) {
        die("Overlong file: %s", strerror(errno));
    }

    fclose(in);

    return datStringFromUtf8(amt, buf);
}

/* Documented in header. */
void ioWriteFileUtf8(zvalue pathList, zvalue text) {
    constInit();

    zint utfSize = datUtf8SizeFromString(text);
    char utf[utfSize + 1];
    datUtf8FromString(utfSize + 1, utf, text);

    FILE *out = openFile(pathList, "w");
    zint amt = fwrite(utf, 1, utfSize, out);

    if (amt != utfSize) {
        die("Trouble writing file: %s", strerror(errno));
    }

    fclose(out);
}
