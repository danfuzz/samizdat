/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * I/O
 */

#ifndef _IO_H_
#define _IO_H_

#include "dat.h"

/**
 * Returns `getcwd()` as a string.
 */
zvalue ioCwdString(void);

/**
 * Converts a path string to an absolute form, as a list of path components,
 * where each component is a string. If the original is a relative path,
 * the result will have the system's current working directory prepended.
 */
zvalue ioPathListFromUtf8(const char *path);

/**
 * Gets symbolic link information about the file with the given name.
 * This returns the linked path if the file names a symbolic link, or
 * returns void if not.
 */
zvalue ioReadLink(zvalue pathList);

/**
 * Reads the file with the given name in its entirety, interpreting
 * it as UTF-8. Returns a string (list of Unicode-representing
 * ints) of the contents.
 *
 * The `pathList` represents an absolute filesystem path as individual
 * strings.
 */
zvalue ioReadFileUtf8(zvalue pathList);

/**
 * Writes the given string to the file with the given name, encoding
 * it as UTF-8. `pathList` is as with `ioReadFileUtf8`.
 */
void ioWriteFileUtf8(zvalue pathList, zvalue text);

#endif
