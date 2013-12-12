/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * I/O Functions
 */

#include "util.h"

#include <errno.h>
#include <unistd.h>

/*
 * Exported Definitions
 */

/* Documented in header. */
char *utilCwd(void) {
    // The maximum buffer size is determined per the recommendation
    // in the Posix docs for `getcwd`.

    long maxSize = pathconf(".", _PC_PATH_MAX);
    char buf[maxSize + 1];

    if (getcwd(buf, maxSize) == NULL) {
        die("Trouble with getcwd(): %s", strerror(errno));
    }

    return strdup(buf);
}
