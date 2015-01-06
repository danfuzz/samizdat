// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Figures out the directory where the executable resides.
//

// Required for `realpath` when using glibc.
#define _XOPEN_SOURCE 700

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

#include "impl.h"


//
// Private Defintions
//

/**
 * Names of proc-type files to try, when determining the executable path.
 */
static const char *PROC_SELF_FILES[] = {
    "/proc/self/exe",         // Linux
    "/proc/curproc/file",     // FreeBSD and some other BSD variants
    "/proc/curproc/exe",      // NetBSD
    "/proc/self/path/a.out",  // Solaris and variants
    NULL
};

/**
 * Resolves links in the given path, returning an absolute path.
 */
static char *resolveLinks(const char *path) {
    char resultBuf[PATH_MAX + 1];
    char *result = realpath(path, resultBuf);

    if (result == NULL) {
        die("Trouble with `realpath`: %s", strerror(errno));
    }

    return utilStrdup(resultBuf);
}

/**
 * Combines two partial paths, returning an allocated result.
 */
static char *catTwoPaths(const char *path1, const char *path2) {
    char *result = utilAlloc(strlen(path1) + strlen(path2) + 2);
    strcpy(result, path1);
    strcat(result, "/");
    strcat(result, path2);
    return result;
}

/**
 * Resolves an `argv[0]` argument to a heuristically-likely path, if possible.
 * Non-`NULL` results are always allocated.
 */
static char *resolveArgv0(const char *argv0) {
    if ((argv0 == NULL) || (argv0[0] == '\0')) {
        return NULL;
    }

    if (argv0[0] == '/') {
        // Absolute path.
        return utilStrdup(argv0);
    }

    if (strchr(argv0, '/') != NULL) {
        // Relative path from CWD.
        char *curDir = utilCwd();
        char *result = catTwoPaths(curDir, argv0);
        utilFree(curDir);
        return result;
    }

    // Have to search through PATH.
    char *pathEnv = getenv("PATH");

    if (pathEnv == NULL) {
        return NULL;
    }

    for (bool last = false; !last; /*last*/) {
        char *endPtr = strchr(pathEnv, ':');
        if (endPtr == NULL) {
            endPtr = strchr(pathEnv, '\0');
            last = true;
        }

        size_t size = endPtr - pathEnv;
        char *onePath = utilAlloc(size + 1);
        utilCpy(char, onePath, pathEnv, size);
        onePath[size] = '\0';

        char *oneFile = catTwoPaths(onePath, argv0);
        utilFree(onePath);

        struct stat statBuf;
        if (stat(oneFile, &statBuf) != 0) {
            // File not found or invalid component aren't actually fatal here.
            if ((errno != ENOENT) && (errno != ENOTDIR)) {
                die("Trouble with `stat`: %s", strerror(errno));
            }
        } else {
            // Found it!
            return oneFile;
        }

        utilFree(oneFile);
        pathEnv = endPtr + 1;
    }

    return NULL;
}


//
// Module Defintions
//

// Documented in header.
char *getProgramDirectory(const char *argv0, const char *suffix) {
    char *execPath = NULL;

    for (int i = 0; PROC_SELF_FILES[i] != NULL; i++) {
        execPath = utilReadLink(PROC_SELF_FILES[i]);
        if (execPath != NULL) {
            break;
        }
    }

    if (execPath == NULL) {
        // Need to use `argv[0]`.
        execPath = resolveArgv0(argv0);
    }

    if (execPath == NULL) {
        die("Unable to determine executable directory.");
    }

    char *result = resolveLinks(execPath);
    utilFree(execPath);

    // Chop off everything at and after the final `/`. Treat the root dir
    // as a special case.
    char *slashAt = strrchr(result, '/');
    if (slashAt != result) {
        *slashAt = '\0';
    }

    if (suffix != NULL) {
        char *newResult = catTwoPaths(result, suffix);
        utilFree(result);
        result = newResult;
    }

    return result;
}
