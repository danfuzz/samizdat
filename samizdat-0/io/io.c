/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "io.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


/*
 * Helper definitions
 */

enum {
    /** Maximum readable file size, in bytes. */
    MAX_FILE_SIZE = 100000
};

/**
 * Converts a listlet-of-stringlets path form into an absolute file name,
 * checking the components for sanity.
 */
static const char *utf8FromPathListlet(zvalue pathListlet) {
    zvalue result = STR_EMPTY;
    zint size = datSize(pathListlet);

    for (zint i = 0; i < size; i++) {
        zvalue one = datListletNth(pathListlet, i);
        zint encodeSize = 0;
        const char *str = datStringletEncodeUtf8(one, &encodeSize);

        if (strlen(str) != encodeSize) {
            die("Invalid path component (contains null bytes): \"%s\"", str);
        } else if (strchr(str, '/') != NULL) {
            die("Invalid path component (contains slash): \"%s\"", str);
        } else if ((datOrder(one, STR_EMPTY) == 0) ||
            (datOrder(one, STR_CH_DOT) == 0) ||
            (datOrder(one, STR_CH_DOTDOT) == 0)) {
            die("Invalid path component: \"%s\"", str);
        }

        result = datStringletAdd(result, STR_CH_SLASH);
        result = datStringletAdd(result, one);
    }

    return datStringletEncodeUtf8(result, NULL);
}

/**
 * Converts a simple string *absolute* path to the listlets-of-stringlets form.
 * This doesn't resolve symlinks, but it does handle double-slashes, `.`
 * components, and `..` components. A trailing slash is represented in the
 * result as an empty path component.
 */
static zvalue pathListletFromAbsolute(const char *path) {
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
        zvalue one = datStringletFromUtf8String(size, at);

        if (datOrder(one, STR_CH_DOTDOT) == 0) {
            zint rsize = datSize(result);
            if (datSize(result) == 0) {
                die("Invalid `..` component in path: \"%s\"", path);
            }
            result = datListletDelNth(result, rsize -1);
        } else if (!((datOrder(one, STR_EMPTY) == 0) ||
                     (datOrder(one, STR_CH_DOT) == 0))) {
            result = datListletAppend(result, one);
        }

        if (slashAt == NULL) {
            break;
        }

        at = slashAt + 1;
    }

    if (path[strlen(path) - 1] == '/') {
        // Represent a trailing slash as an empty path component.
        result = datListletAppend(result, STR_EMPTY);
    }

    return result;
}

/**
 * Opens the file with the given name (a stringlet), and with the
 * given `fopen()` mode. Returns the `FILE *` handle.
 */
static FILE *openFile(zvalue pathListlet, const char *mode) {
    const char *path = utf8FromPathListlet(pathListlet);

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
zvalue ioPathListletFromUtf8(const char *path) {
    constInit();

    if (path[0] != '/') {
        // Concatenate the given path onto the current working directory.
        int size = strlen(path) + FILENAME_MAX + 1; // +1 for the '/'.
        char *buf = zalloc(size);

        if (getcwd(buf, size) == NULL) {
            die("Can't get cwd: %s", strerror(errno));
        }

        strcat(buf, "/");
        strcat(buf, path);
        path = buf;
    }

    return pathListletFromAbsolute(path);
}

/* Documented in header. */
zvalue ioReadLink(zvalue pathListlet) {
    constInit();

    const char *path = utf8FromPathListlet(pathListlet);
    size_t pathLen = strlen(path);
    char result[pathLen + 4 + FILENAME_MAX + 1];

    strcpy(result, path);
    strcat(result, "/../"); // To elide the name of the link in the result.

    char *linkAt = strchr(result, '\0');
    ssize_t size = readlink(path, linkAt, FILENAME_MAX);

    if (size < 0) {
        if ((errno == EINVAL) || (errno == ENOENT) || (errno == ENOTDIR)) {
            // Not a symlink, or file not found, or invalid component, none
            // of which are really errors from the perspective of this function.
            return NULL;
        }
        die("Trouble with readlink: %s", strerror(errno));
    }

    linkAt[size] = '\0';

    if (linkAt[0] == '/') {
        // The link is absolute. Just use it, ignoring the path that led
        // up to it.
        return ioPathListletFromUtf8(linkAt);
    } else {
        // The link is relative. Need to use the prefix we set up.
        return ioPathListletFromUtf8(result);
    }
}

/* Documented in header. */
zvalue ioReadFileUtf8(zvalue pathListlet) {
    constInit();

    char buf[MAX_FILE_SIZE];
    FILE *in = openFile(pathListlet, "r");
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
void ioWriteFileUtf8(zvalue pathListlet, zvalue text) {
    constInit();

    zint utfSize;
    const char *utf = datStringletEncodeUtf8(text, &utfSize);

    FILE *out = openFile(pathListlet, "w");
    zint amt = fwrite(utf, 1, utfSize, out);

    if (amt != utfSize) {
        die("Trouble writing file: %s", strerror(errno));
    }

    fclose(out);
}
