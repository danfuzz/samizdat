// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// I/O Functions
//

// Required for `lstat()` and `readlink()` when using glibc.
#define _XOPEN_SOURCE 700

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"


//
// Exported Definitions
//

// Documented in header.
char *utilCwd(void) {
    // The maximum buffer size is determined per the recommendation
    // in the Posix docs for `getcwd`.

    long maxSize = pathconf(".", _PC_PATH_MAX);
    char buf[maxSize + 1];

    if (getcwd(buf, maxSize) == NULL) {
        die("Trouble with `getcwd`: %s", strerror(errno));
    }

    return utilStrdup(buf);
}

// Documented in header.
char *utilReadLink(const char *path) {
    struct stat statBuf;

    if (lstat(path, &statBuf) != 0) {
        if ((errno == ENOENT) || (errno == ENOTDIR)) {
            // File not found or invalid component, neither of which are
            // really errors from the perspective of this function.
            errno = 0;
            return NULL;
        }
        die("Trouble with `lstat`: %s", strerror(errno));
    } else if (!S_ISLNK(statBuf.st_mode)) {
        // Not a symlink.
        errno = 0;
        return NULL;
    }

    // If `st_size` is non-zero, then it can safely be used as the size of
    // the link data. However, on Linux some valid links (particularly, those
    // in `/proc/`) will have `st_size` reported as `0`. In such cases, we
    // use an ample but fixed-size buffer, and hope for the best.

    bool assumeSize = (statBuf.st_size == 0);
    size_t linkSz = assumeSize ? 500 : statBuf.st_size;
    char *result = utilAlloc(linkSz + 1);
    ssize_t resultSz = readlink(path, result, linkSz);

    if (resultSz < 0) {
        die("Trouble with `readlink`: %s", strerror(errno));
    } else if (assumeSize ? (resultSz > linkSz) : (resultSz != linkSz)) {
        die("Strange `readlink` result: %d", (zint) resultSz);
    }

    result[resultSz] = '\0';
    return result;
}
