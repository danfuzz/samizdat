// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// I/O
//

#ifndef _IO_H_
#define _IO_H_

#include <stdbool.h>

#include "type/declare.h"

/** File type constant. Documented in the spec. */
SYM_DECL(absent);

/** File type constant. Documented in the spec. */
SYM_DECL(directory);

/** File type constant. Documented in the spec. */
SYM_DECL(file);

/** File type constant. Documented in the spec. */
SYM_DECL(other);

/** File type constant. Documented in the spec. */
SYM_DECL(symlink);


/**
 * Returns `getcwd()` as a string.
 */
zvalue ioCwd(void);

/**
 * Returns the file type of the file at the given path if it exists, or
 * `@absent` if the file doesn't exist (including if one of the named
 * directories in the path doesn't exist). Types are the same as for
 * `core.Io0::fileType` (see which).
 *
 * If `followSymlinks` is passed as `true`, then symlinks are followed, and
 * the result is the file type of the linked file. If `false`, then the
 * result given a symlink is `@symlink`.
 */
zvalue ioFileType(zvalue path, bool followSymlinks);

/**
 * Gets the contents of the directory at the given path. If the path does not
 * name a directory, this returns `NULL`. A successful result is a map from
 * string names to file types (as strings). Types are as with `ioFileType`,
 * with the addition of `@symlink` as a possibility.
 */
zvalue ioReadDirectory(zvalue path);

/**
 * Gets symbolic link information about the file with the given name.
 * It the file names a symbolic link, then this returns the linked path as
 * a simple string. If the file does not name a symbolic link, this returns
 * `NULL`.
 */
zvalue ioReadLink(zvalue path);

/**
 * Reads the file with the given name in its entirety, interpreting
 * it as UTF-8. Returns a string (list of Unicode-representing
 * ints) of the contents.
 */
zvalue ioReadFileUtf8(zvalue path);

/**
 * Writes the given string to the file with the given name, encoding
 * it as UTF-8.
 */
void ioWriteFileUtf8(zvalue path, zvalue text);

/**
 * Checks an absolute filesystem path for validity. This fails (fatally)
 * if `path` isn't a string, if it is empty, if it doesn't start with a
 * slash, or if it contains any `\0` characters.
 */
void ioCheckAbsolutePath(zvalue path);

/**
 * Checks a filesystem path for validity. This fails (fatally) if `path`
 * isn't a string, if it is empty, or if it contains any `\0` characters.
 */
void ioCheckPath(zvalue path);

#endif
