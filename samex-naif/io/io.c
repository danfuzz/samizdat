/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "const.h"
#include "io.h"
#include "type/String.h"
#include "type/Value.h"
#include "util.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

/**
 * Opens the file with the given name (a string), and with the
 * given `fopen()` mode. Returns the `FILE *` handle.
 */
static FILE *openFile(zvalue path, const char *mode) {
    ioCheckPath(path);
    zint sz = utf8SizeFromString(path);
    char str[sz + 1];
    utf8FromString(sz + 1, str, path);

    FILE *file = fopen(str, mode);
    if (file == NULL) {
        die("Trouble opening file \"%s\": %s", str, strerror(errno));
    }

    return file;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue ioCwd(void) {
    char *dir = utilCwd();
    zvalue result = stringFromUtf8(-1, dir);

    utilFree(dir);
    return result;
}

/* Documented in header. */
zvalue ioFileType(zvalue path) {
    ioCheckPath(path);
    zint sz = utf8SizeFromString(path);
    char str[sz + 1];
    utf8FromString(sz + 1, str, path);

    struct stat statBuf;
    if (stat(str, &statBuf) != 0) {
        if ((errno == ENOENT) || (errno == ENOTDIR)) {
            // File not found or invalid component, neither of which
            // are really errors from the perspective of this function.
            return STR_absent;
        }
        die("Trouble with stat(): %s", strerror(errno));
    }

    if (S_ISREG(statBuf.st_mode))      { return STR_file;      }
    else if (S_ISDIR(statBuf.st_mode)) { return STR_directory; }
    else                               { return STR_other;     }
}

/* Documented in header. */
zvalue ioReadFileUtf8(zvalue path) {
    char buf[IO_MAX_FILE_SIZE];
    FILE *in = openFile(path, "r");
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
zvalue ioReadLink(zvalue path) {
    ioCheckPath(path);
    zint sz = utf8SizeFromString(path);
    char str[sz + 1];
    utf8FromString(sz + 1, str, path);

    struct stat statBuf;
    if (lstat(str, &statBuf) != 0) {
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

    size_t linkSz = statBuf.st_size;
    char linkStr[linkSz];
    ssize_t linkResult = readlink(str, linkStr, linkSz);
    if (linkResult < 0) {
        die("Trouble with readlink(): %s", strerror(errno));
    } else if (linkResult != linkSz) {
        die("Strange readlink() result: %ld", (long) linkResult);
    }

    return stringFromUtf8(linkSz, linkStr);
}

/* Documented in header. */
void ioWriteFileUtf8(zvalue path, zvalue text) {
    zint utfSize = utf8SizeFromString(text);
    char utf[utfSize + 1];
    utf8FromString(utfSize + 1, utf, text);

    FILE *out = openFile(path, "w");
    zint amt = fwrite(utf, 1, utfSize, out);

    if (amt != utfSize) {
        die("Trouble writing file: %s", strerror(errno));
    }

    fclose(out);
}
