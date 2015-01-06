// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

/**
 * Given `argv[0]`, figures out the directory in which the executable
 * resides. This works reliably on systems that use a `/proc` filesystem,
 * but may fail on other systems for various reasons. If passed as non-`NULL`,
 * the `suffix` is appended to the result, after a `/`.
 *
 * This returns an allocated string, which can be freed with `utilFree()`.
 *
 * References:
 * * <http://stackoverflow.com/questions/933850>
 * * <http://stackoverflow.com/questions/1023306>
 */
char *getProgramDirectory(const char *argv0, const char *suffix);

#endif
