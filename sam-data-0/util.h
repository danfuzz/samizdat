/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

/*
 * Private utilities
 */

#ifndef _UTIL_H_
#define _UTIL_H_

/**
 * Emits a debugging note. Arguments are as with `printf()`.
 */
void samNote(const char *format, ...)
    __attribute__((format (printf, 1, 2)));

/**
 * Dies (aborts the process) with the given message. Arguments are as
 * with `printf()`.
 */
void samDie(const char *format, ...)
    __attribute__((noreturn))
    __attribute__((format (printf, 1, 2)));

#endif
