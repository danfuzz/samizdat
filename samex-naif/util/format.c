// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>


// Needed for `asprintf` when using glibc.
#define _GNU_SOURCE

#include <inttypes.h>
#include <stdio.h>

#include "impl.h"


//
// Private Definitions
//

/** Format result-in-progress. */
typedef struct {
    /** Total buffer size. */
    zint size;

    /** Output cursor position. */
    zint at;

    /** Pointer to actual characters. */
    char *chars;
}
zformatBuf;

/**
 * Ensures the given buffer has room for the given number of additional
 * characters.
 */
static void bufEnsure(zformatBuf *buf, zint size) {
    if ((buf->size - buf->at) < size) {
        zint newSize = (buf->at + size) * 2 + UTIL_INITIAL_FORMAT_SIZE;
        char *newChars = utilAlloc(newSize);

        if (buf->chars != NULL) {
            utilCpy(char, newChars, buf->chars, buf->at);
            utilFree(buf->chars);
        }

        buf->chars = newChars;
    }
}

/**
 * Append the given string to the given buffer. Grows the buffer if needed.
 */
static void bufCat(zformatBuf *buf, const char *str) {
    zint sz = strlen(str);

    bufEnsure(buf, sz);
    utilCpy(char, &buf->chars[buf->at], str, sz);
    buf->at += sz;
}

/**
 * Append the given char to the given buffer. Grows the buffer if needed.
 */
static void bufCatChar(zformatBuf *buf, char ch) {
    bufEnsure(buf, 1);
    buf->chars[buf->at] = ch;
    buf->at++;
}

/**
 * Append the given quantity of the given character to the given buffer. Grows
 * the buffer if needed.
 */
static void bufCatChars(zformatBuf *buf, char ch, zint size) {
    bufEnsure(buf, size);

    for (zint i = 0; i < size; i++) {
        buf->chars[buf->at + i] = ch;
    }

    buf->at += size;
}


//
// Exported Definitions
//

// Documented in header.
char *utilFormat(const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    char *result = utilVFormat(format, rest);
    va_end(rest);

    return result;
}

// Documented in header.
char *utilVFormat(const char *format, va_list rest) {
    zformatBuf buf = { 0, 0, NULL };

    while (*format != '\0') {
        char ch = *format;
        format++;
        if (ch != '%') {
            bufCatChar(&buf, ch);
            continue;
        }

        zint fieldWidth = -1;
        char padChar = ' ';
        bool done = false;
        const char *intermed = NULL;
        char *freeMe = NULL;

        while ((intermed == NULL) && (*format != '\0')) {
            ch = *format;
            format++;

            switch (ch) {
                case '0': {
                    if (fieldWidth == -1) {
                        padChar = '0';
                    } else {
                        fieldWidth *= 10;
                    }
                    break;
                }
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9': {
                    if (fieldWidth == -1) {
                        fieldWidth = 0;
                    }
                    fieldWidth = (fieldWidth * 10) + (ch - '0');
                    break;
                }
                case '%': {
                    intermed = "%";
                    break;
                }
                case 'c': {
                    zint cval = (zint) (unsigned char) va_arg(rest, int);
                    int err = ((cval >= ' ') && (cval < 0x7f))
                        ? asprintf(&freeMe, "'%c'", (char) cval)
                        : asprintf(&freeMe, "'\\x%02x'", (int) cval);

                    if (err < 0) {
                        die("Failure in `asprintf`.");
                    }

                    intermed = freeMe;
                    break;
                }
                case 'd': {
                    zint i = va_arg(rest, zint);
                    int err = ((fieldWidth > 0) && (padChar == '0'))
                        ? asprintf(&freeMe, "%0*" PRId64, (int) fieldWidth, i)
                        : asprintf(&freeMe, "%" PRId64, i);

                    if (err < 0) {
                        die("Failure in `asprintf`.");
                    }

                    intermed = freeMe;
                    break;
                }
                case 'g': {
                    double d = va_arg(rest, double);
                    int err = ((fieldWidth > 0) && (padChar == '0'))
                        ? asprintf(&freeMe, "%0*g", (int) fieldWidth, d)
                        : asprintf(&freeMe, "%g", d);

                    if (err < 0) {
                        die("Failure in `asprintf`.");
                    }

                    intermed = freeMe;
                    break;
                }
                case 'p': {
                    void *ptr = va_arg(rest, void *);
                    if (asprintf(&freeMe, "%p", ptr) < 0) {
                        die("Failure in `asprintf`.");
                    }
                    intermed = freeMe;
                    break;
                }
                case 's': {
                    intermed = va_arg(rest, const char *);
                    break;
                }
                case 'x': {
                    zint i = va_arg(rest, zint);
                    fieldWidth += 2;  // For the `0x`.
                    int err = ((fieldWidth > 0) && (padChar == '0'))
                        ? asprintf(&freeMe, "%#0*" PRIx64, (int) fieldWidth, i)
                        : asprintf(&freeMe, "%#" PRIx64, i);

                    if (err < 0) {
                        die("Failure in `asprintf`.");
                    }

                    intermed = freeMe;
                    break;
                }
                default: {
                    die("Unknown directive character: %c", ch);
                }
            }
        }

        if (intermed == NULL) {
            die("Unterminated format directive.");
        }

        if (fieldWidth > 0) {
            zint size = strlen(intermed);
            if (size < fieldWidth) {
                bufCatChars(&buf, padChar, fieldWidth - size);
            }
        }

        bufCat(&buf, intermed);

        if (freeMe != NULL) {
            utilFree(freeMe);
        }
    }

    bufCatChar(&buf, '\0');
    return buf.chars;
}
