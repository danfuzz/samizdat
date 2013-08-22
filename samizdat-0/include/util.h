/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Lowest-level utilities
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include "ztype.h"

#include <stdbool.h>
#include <string.h>

// All the int-related declarations.
#include "util/zint.h"


/*
 * Message Declarations
 */

enum {
    /** Magic value used to identify stack giblets. */
    UTIL_GIBLET_MAGIC = 0x57ac61b1
};

/**
 * Function added to the "death context", which is expected to return
 * some sort of interesting string.
 */
typedef char *(*zcontextFunction)(void *state);

/**
 * Stack "giblet". This is a struct placed on the (C) stack, for use
 * when producing a stack trace upon process death.
 */
typedef struct UtilStackGiblet {
    /** Identifying magic value. */
    zint magic;

    /** Giblet one layer up on the stack. */
    struct UtilStackGiblet *pop;

    /** Function to call to produce a line of context. */
    zcontextFunction function;

    /** State to pass to `function`. */
    void *state;
} UtilStackGiblet;

/** The current top-of-stack of giblets. */
extern UtilStackGiblet *utilStackTop;

/**
 * Defines a giblet for the current function. Use this at the point a
 * stack trace for the call would be valid.
 */
#define UTIL_TRACE_START(function, state) \
    UtilStackGiblet stackGiblet = { \
        UTIL_GIBLET_MAGIC, utilStackTop, (function), (state) \
    }; \
    do { \
        utilStackTop = &stackGiblet; \
    } while(0)

/**
 * Ends the scope for a giblet. Use this as close to function return as
 * possible.
 */
#define UTIL_TRACE_END() \
    do { \
        utilStackTop = stackGiblet.pop; \
    } while(0)

/**
 * Emits a debugging message. Arguments are as with `printf()`.
 */
void note(const char *format, ...)
    __attribute__((format (printf, 1, 2)));

/**
 * Dies (aborts the process) with the given message. Arguments are as
 * with `printf()`. If there is any active stack context (more
 * `deathPush*()`es than `deathPop()`s), then that context is appended
 * to the death report.
 */
void die(const char *format, ...)
    __attribute__((noreturn))
    __attribute__((format (printf, 1, 2)));


/*
 * Allocation Declarations
 */

/**
 * Allocates zeroed-out memory of the indicated size (in bytes).
 */
void *utilAlloc(zint size);

/**
 * Frees memory previously allocated by `utilAlloc`.
 */
void utilFree(void *memory);

/**
 * Returns whether this appears to be a pointer to heap-allocated memory
 * (though not necessarily the start of an allocation).
 */
bool utilIsHeapAllocated(void *memory);


/*
 * UTF-8 Declarations
 */

/**
 * Gets the decoded size (the number of encoded Unicode code points)
 * of a UTF-8 encoded string of the given size in bytes.
 */
zint utf8DecodeStringSize(zint stringBytes, const char *string);

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zchar`s. The buffer must be sufficiently
 * large to hold the result of decoding.
 */
void utf8DecodeCharsFromString(zchar *result,
        zint stringBytes, const char *string);

/**
 * Encodes a single Unicode code point as UTF-8, writing it to the
 * given string, which must be large enough to hold it. Returns a
 * pointer to the position just after what was encoded. If `string` is
 * passed as `NULL`, this doesn't encode but still returns the
 * would-be encoded size in pointer form (i.e. `(char *) NULL +
 * size`).
 */
char *utf8EncodeOne(char *result, zint ch);


/*
 * Miscellaneous Declarations
 */

/**
 * Like `memcpy`, except that the last argument indicates an element
 * count (not a byte count), and a new first argument indicates the type
 * of element (from which per-element size is derived).
 */
#define utilCpy(type, dest, src, count) \
    memcpy((dest), (src), (count) * sizeof(type))

#endif
