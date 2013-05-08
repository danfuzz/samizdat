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
 * Converts a path string to an absolute form, as a listlet of path components,
 * where each component is a string. If the original is a relative path,
 * the result will have the system's current working directory prepended.
 */
zvalue ioPathListletFromUtf8(const char *path);

/**
 * Gets symbolic link information about the file with the given name.
 * This returns the linked path if the file names a symbolic link, or
 * returns void if not.
 */
zvalue ioReadLink(zvalue pathListlet);

/**
 * Reads the file with the given name in its entirety, interpreting
 * it as UTF-8. Returns a string (listlet of Unicode-representing
 * integers) of the contents.
 *
 * The `pathListlet` represents an absolute filesystem path as individual
 * strings.
 */
zvalue ioReadFileUtf8(zvalue pathListlet);

/**
 * Writes the given string to the file with the given name, encoding
 * it as UTF-8. `pathListlet` is as with `ioReadFileUtf8`.
 */
void ioWriteFileUtf8(zvalue pathListlet, zvalue text);

#endif
