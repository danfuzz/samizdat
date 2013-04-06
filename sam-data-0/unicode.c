/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "sam-data.h"
#include "unicode.h"

#include <stdlib.h>


/*
 * Helper functions
 */

/**
 * Gets a pointer just past the end of the given string, asserting
 * validity of same.
 */
static const zbyte *getStringEnd(const zbyte *string, zint stringBytes) {
    if (stringBytes < 0) {
        samDie("Invalid string size: %lld", stringBytes);
    }

    const zbyte *result = string + stringBytes;

    if (result < string) {
        samDie("Invalid string size (pointer wraparound): %p + %lld",
               string, stringBytes);
    }

    return result;
}

/**
 * Does the basic decoding step, with syntactic but not semantic validation.
 */
static const zbyte *justDecode(const zbyte *string, zint stringBytes,
                               zint *result) {
    if (stringBytes <= 0) {
        samDie("Invalid string size: %lld", stringBytes);
    }

    zbyte ch = *string;
    zint value;
    int extraBytes;
    zint minValue;

    string++;
    stringBytes--;

    switch (ch >> 4) {
        case 0x0: case 0x1: case 0x2: case 0x3:
        case 0x4: case 0x5: case 0x6: case 0x7: {
            // 00..7f: Single-byte form.
            extraBytes = 0;
            minValue = 0;
            value = ch;
            break;
        }
        case 0x8: case 0x9: case 0xa: case 0xb: {
            // 80..bf: Invalid start bytes.
            samDie("Invalid UTF-8 start byte: %#02x", (int) ch);
            break;
        }
        case 0xc: case 0xd: {
            // c0..df: Two-byte form.
            extraBytes = 1;
            minValue = 0x80;
            value = ch & 0x1f;
            break;
        }
        case 0xe: {
            // e0..ef: Three-byte form.
            extraBytes = 2;
            minValue = 0x800;
            value = ch & 0x0f;
            break;
        }
        case 0xf: {
            // f0..ff: Four- to six-byte forms.
            switch (ch & 0xf) {
                case 0x0: case 0x1: case 0x2: case 0x3:
                case 0x4: case 0x5: case 0x6: case 0x7: {
                    // f0..f7: Four-byte form.
                    extraBytes = 3;
                    minValue = 0x10000;
                    value = ch & 0x07;
                    break;
                }
                case 0x8: case 0x9: case 0xa: case 0xb: {
                    // f8..fb: Five-byte form.
                    extraBytes = 4;
                    minValue = 0x200000;
                    value = ch & 0x03;
                    break;
                }
                case 0xc: case 0xd: {
                    // fc..fd: Six-byte form.
                    extraBytes = 5;
                    minValue = 0x4000000;
                    value = ch & 0x01;
                    break;
                }
                case 0xe: {
                    // fe: Seven-byte forms. Strangely, not defined per
                    // spec, even though it's required if you want to be
                    // able to encode 32 bits. Strangeness is augmented
                    // by the fact that the six-byte forms *are* defined,
                    // even though there are no valid Unicode code points
                    // that would require that form.
                    extraBytes = 6;
                    minValue = 0x80000000;
                }
                case 0xf: {
                    // fe..ff: Invalid start bytes.
                    samDie("Invalid UTF-8 start byte: %#02x", (int) ch);
                    break;
                }
            }
        }
    }

    if (extraBytes > stringBytes) {
        samDie("Incomplete UTF-8 sequence.");
    }

    while (extraBytes > 0) {
        ch = *string;
        string++;
        extraBytes--;

        if ((ch & 0xc0) != 0x80) {
            samDie("Invalid UTF-8 continuation byte: %#02x", (int) ch);
        }

        value = (value << 6) | (ch & 0x3f);
    }

    if (value < minValue) {
        samDie("Overlong UTF-8 encoding of value: %#llx", value);
    }

    if (value >= 0x100000000LL) {
        samDie("Out-of-range UTF-8 encoded value: %#llx", value);
    }

    if (result != NULL) {
        *result = value;
    }

    return string;
}


/*
 * Intra-library API implementation
 */

/** Documented in `unicode.h`. */
void samAssertValidUnicode(zint value) {
    if ((value >= 0xd800) && (value <= 0xdfff)) {
        samDie("Invalid occurrence of surrogate code point: %#04x",
               (int) value);
    } else if (value == 0xfffe) {
        samDie("Invalid occurrence of reversed-BOM.");
    } else if (value == 0xffff) {
        samDie("Invalid occurrence of not-a-character.");
    } else if (value >= 0x110000) {
        samDie("Invalid occurrence of high code point: %#llx", value);
    }
}

/** Documented in `unicode.h`. */
const zbyte *samUtf8DecodeOne(const zbyte *string, zint stringBytes,
                              zint *result) {
    string = justDecode(string, stringBytes, result);
    samAssertValidUnicode(*result);

    return string;
}

/** Documented in `unicode.h`. */
zint samUtf8DecodeStringSize(const zbyte *string, zint stringBytes) {
    const zbyte *stringEnd = getStringEnd(string, stringBytes);
    zint result = 0;

    while (string < stringEnd) {
        string = justDecode(string, stringEnd - string, NULL);
        result++;
    }

    return result;
}

/** Documented in API header. */
void samUtf8DecodeStringToInts(const zbyte *string, zint stringBytes,
                               zint *result) {
    const zbyte *stringEnd = getStringEnd(string, stringBytes);

    while (string < stringEnd) {
        string = samUtf8DecodeOne(string, stringEnd - string, result);
        result++;
    }
}

/** Documented in API header. */
void samUtf8DecodeStringToValues(const zbyte *string, zint stringBytes,
                                 zvalue *result) {
    const zbyte *stringEnd = getStringEnd(string, stringBytes);
    zint one = 0;

    while (string < stringEnd) {
        string = samUtf8DecodeOne(string, stringEnd - string, &one);
        *result = samIntletFromInt(one);
        result++;
    }
}
