/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * I/O
 */

#ifndef _IO_H_
#define _IO_H_

#include "dat.h"

#include <stdbool.h>


/**
 * Returns `getcwd()` as a string.
 */
zvalue ioCwd(void);

/**
 * Returns `true` if there is a "regular" file at the given path, or
 * `false` if not.
 */
bool ioFileExists(zvalue path);

/**
 * Returns the file type of the file at the given path if it exists, or
 * `NULL` if the file doesn't exist (including if one of the named directories
 * in the path doesn't exist). Non-`NULL` return values are always in-model
 * strings, one of:
 *
 * * `"file"` &mdash; regular file.
 * * `"directory"` &mdash; directory.
 * * `"other"` &mdash; anything else (e.g. named pipe).
 */
zvalue ioFileType(zvalue path);

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
