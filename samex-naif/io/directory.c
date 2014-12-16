// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "io.h"
#include "type/Cmp.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/SymbolTable.h"
#include "util.h"


//
// Exported Definitions
//

// Documented in header.
zvalue ioReadDirectory(zvalue path) {
    ioCheckPath(path);
    zint sz = utf8SizeFromString(path);
    char str[sz + 1];
    utf8FromString(sz + 1, str, path);

    zvalue type = ioFileType(path, true);
    if (!cmpEq(type, SYM(directory))) {
        return NULL;
    }

    DIR *dir = opendir(str);
    if (dir == NULL) {
        die("Trouble opening directory \"%s\": %s", str, strerror(errno));
    }

    zvalue pathPrefix = cm_cat(path, stringFromZchar('/'));
    zvalue result = EMPTY_MAP;
    struct dirent entry;
    struct dirent *entryPtr;

    for (;;) {
        if (readdir_r(dir, &entry, &entryPtr) != 0) {
            die("Trouble reading directory \"%s\": %s", str, strerror(errno));
        } else if (entryPtr == NULL) {
            // End-of-directory is indicated by setting `entryPtr` to `NULL`.
            break;
        }

        if ((entry.d_namlen < 3)
            && (   (strcmp(entry.d_name, ".") == 0)
                || (strcmp(entry.d_name, "..") == 0))) {
            // Skip the entries for "this directory" and "parent directory."
            continue;
        }

        zvalue name = stringFromUtf8(entry.d_namlen, entry.d_name);

        // Note: `dirent.d_type` is very conveniently defined in BSD, but it
        // is unfortunately *not* particularly standardized. Instead, we
        // use `lstat()` (via `ioFileType()`).
        zvalue type = ioFileType(cm_cat(pathPrefix, name), false);

        result = cm_cat(result, symtabFromMapping((zmapping) {name, type}));
    }

    if (closedir(dir) != 0) {
        die("Trouble closing directory \"%s\": %s", str, strerror(errno));
    }

    return result;
}
