/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "io.h"
#include "util.h"
#include "zlimits.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
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
 * Converts a simple string *absolute* path to the lists-of-strings form.
 * This doesn't resolve symlinks, but it does handle double-slashes, `.`
 * components, and `..` components. A trailing slash is represented in the
 * result as an empty path component.
 */
static zvalue pathListFromAbsolute(const char *path) {
    if (path[0] == '\0') {
        return EMPTY_LISTLET;
    } else if (path[0] != '/') {
        die("Invalid absolute path: \"%s\"", path);
    }

    zvalue result = EMPTY_LISTLET;
    const char *at = path + 1; // +1 to skip the initial '/'.
    for (;;) {
        const char *slashAt = strchr(at, '/');
        zint size = (slashAt != NULL) ? (slashAt - at) : strlen(at);
        zvalue one = datStringFromUtf8(size, at);

        if (datEq(one, STR_CH_DOTDOT)) {
            zint rsize = datSize(result);
            if (datSize(result) == 0) {
                die("Invalid `..` component in path: \"%s\"", path);
            }
            result = datListDelNth(result, rsize -1);
        } else if (!(datEq(one, STR_EMPTY) || datEq(one, STR_CH_DOT))) {
            result = datListAppend(result, one);
        }

        if (slashAt == NULL) {
            break;
        }

        at = slashAt + 1;
    }

    if (path[strlen(path) - 1] == '/') {
        // Represent a trailing slash as an empty path component.
        result = datListAppend(result, STR_EMPTY);
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
zvalue ioPathListFromUtf8(const char *path) {
    constInit();

    if (path[0] == '/') {
        return pathListFromAbsolute(path);
    }

    // Concatenate the given path onto the current working directory.
    int size = strlen(path) + FILENAME_MAX + 1; // +1 for the '/'.
    char buf[size];

    if (getcwd(buf, size) == NULL) {
        die("Can't get cwd: %s", strerror(errno));
    }

    strcat(buf, "/");
    strcat(buf, path);
    return pathListFromAbsolute(buf);
}

/* Documented in header. */
zvalue ioReadLink(zvalue pathList) {
    constInit();

    zvalue pathString = stringFromPathList(pathList);
    zint pathSize = datUtf8SizeFromString(pathString);
    char path[pathSize + 4 + FILENAME_MAX + 1];

    datUtf8FromString(pathSize + 1, path, pathString);

    char linkPath[FILENAME_MAX + 1];
    ssize_t size = readlink(path, linkPath, FILENAME_MAX);

    if (size < 0) {
        if ((errno == EINVAL) || (errno == ENOENT) || (errno == ENOTDIR)) {
            // Not a symlink, or file not found, or invalid component, none
            // of which are really errors from the perspective of this function.
            return NULL;
        }
        die("Trouble with readlink: %s", strerror(errno));
    }

    linkPath[size] = '\0';

    if (linkPath[0] == '/') {
        // The link is absolute. Just use it, ignoring the path that led
        // up to it.
        return ioPathListFromUtf8(linkPath);
    } else {
        // The link is relative. Need to use the passed-in prefix.
        strcat(path, "/../"); // To elide the name of the link in the result.
        strcat(path, linkPath);
        return ioPathListFromUtf8(path);
    }
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
