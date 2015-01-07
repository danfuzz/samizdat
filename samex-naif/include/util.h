// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Lowest-level utilities
//

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdarg.h>
#include <string.h>

#include "ztype.h"

#include "util/zint.h"     // Int-related declarations.
#include "util/zstring.h"  // String-related declarations.


//
// Message Declarations
//

enum {
    /** Magic value used to identify stack giblets. */
    UTIL_GIBLET_MAGIC = 0x57ac61b1
};

/**
 * Function added to the "death context," which is expected to return
 * some sort of interesting string.
 */
typedef char *(*zcontextFunction)(void *state);

/**
 * Stack "giblet." This is a struct placed on the (C) stack, for use
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
 * Dies (aborts the process) with the given message. Arguments are as
 * with `utilFormat()`. If there is any active stack context (more
 * `UTIL_TRACE_START()`s than `UTIL_TRACE_END()`s), then that context is
 * appended to the death report.
 */
void die(const char *format, ...)
    __attribute__((noreturn));

/**
 * Emits a debugging message. Arguments are as with `utilFormat()`.
 */
void note(const char *format, ...);


//
// String Formatting Declarations
//

/**
 * Returns a freshly-allocated string constructed based on the given format
 * string and additional arguments. This is similar to `asprintf()`, except
 * that the return value is the allocated string (not a size-or-error) and
 * directives are bespoke.
 *
 * Directives:
 * * `%` -- Literal `%`.
 * * `c` -- The given `char` argument as a single-quoted character. Emits
 *   a hex escape if not in the printable ASCII range. This behavior is a bit
 *   different than the usual `printf` meaning.
 * * `d` -- The given `zint` argument as a decimal.
 * * `g` -- The given `double` argument as a decimal.
 * * `p` -- The given `void *` argument as a pointer address.
 * * `s` -- The given string argument (type `const char *`).
 * * `x` -- The given `zint` argument as an unsigned hexadecimal, prefixed
 *   with `0x`. The `0x` isn't considered part of the field width. This
 *   behavior is similar to `%#x` in `printf`.
 *
 * Modifier prefixes:
 * * `0`..`9` -- Indicate field width. An initial `0` means that padding
 *     should be with zeros instead of spaces.
 *
 * **Note:** The impetus for this function was that there is arguably no saner
 * way to allow for formatting of `zint`s. `zint` is defined to be 64 bits,
 * and there is no fixed directive string (per spec) which means "64-bit
 * integer." That is, `PRId64` is *way too ugly* to use in regular code.
 */
char *utilFormat(const char *format, ...);

/**
 * Like `utilFormat`, but takes a `va_list` instead of separate args. Use
 * similar to `vasprintf()`.
 */
char *utilVFormat(const char *format, va_list);


//
// Allocation Declarations
//

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

/**
 * Equivalent to `strdup` that uses the memory allocation functions defined
 * by this module.
 */
char *utilStrdup(const char *string);


//
// UTF-8 Declarations
//

/**
 * Gets the decoded size (the number of encoded Unicode code points)
 * of a UTF-8 encoded string of the given size in bytes. If `utfBytes`
 * is passed as `-1`, this relies on `utf` being `\0`-terminated.
 */
zint utf8DecodeStringSize(zint utfBytes, const char *utf);

/**
 * Decodes the given UTF-8 encoded string of the given size in bytes,
 * into the given buffer of `zchar`s. The buffer must be sufficiently
 * large to hold the result of decoding. If `stringBytes` is passed as `-1`,
 * this relies on `string` being `\0`-terminated.
 */
void utf8DecodeCharsFromString(zchar *result, zint utfBytes, const char *utf);

/**
 * Encodes a single Unicode code point as UTF-8, writing it to the
 * given `result` string, which must be large enough to hold it. Returns a
 * pointer to the position just after what was encoded. If `result` is
 * passed as `NULL`, this doesn't encode but still returns the
 * would-be encoded size in pointer form (i.e. `(char *) NULL +
 * size`).
 */
char *utf8EncodeOne(char *result, zint ch);


//
// I/O Declarations
//

/**
 * Gets the current directory into an allocated buffer.
 */
char *utilCwd(void);

/**
 * Reads and returns the contents of a symlink, if it in fact exists.
 * Otherwise returns `NULL`. Will die with an error if there is a problem
 * other than a non-existent file.
 *
 * Non-`NULL` return values are allocated and must be freed with `utilFree()`.
 */
char *utilReadLink(const char *path);


//
// Miscellaneous Declarations
//

/**
 * Like `memcmp`, except that the last argument indicates an element
 * count (not a byte count), and a new first argument indicates the type
 * of element (from which per-element size is derived).
 */
#define utilCmp(type, s1, s2, count) \
    memcmp((s1), (s2), (count) * sizeof(type))

/**
 * Like `memcpy`, except that the last argument indicates an element
 * count (not a byte count), and a new first argument indicates the type
 * of element (from which per-element size is derived).
 */
#define utilCpy(type, dest, src, count) \
    memcpy((dest), (src), (count) * sizeof(type))

/**
 * Like `memset`, except that it's always setting to zeros, and it bases the
 * amount to clear on the `sizeof` the given target.
 */
#define utilZero(dest) \
    memset((dest), 0, sizeof(dest))

/**
 * Guaranteed-stable sort, which is expected to perform particularly well on
 * partially-sorted data. The arguments are just like those to the standard
 * library function `qsort()`.
 */
void utilSortStable(void *base, size_t nel, size_t width,
        int (*compar)(const void *, const void *));

#endif
