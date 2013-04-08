/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "unicode.h"

#include <stdlib.h>


/*
 * Helper functions
 */

/**
 * Gets a pointer just past the end of the given string, asserting
 * validity of same.
 */
static const char *getStringEnd(const char *string, zint stringBytes) {
    if (stringBytes < 0) {
        samDie("Invalid string size: %lld", stringBytes);
    }

    const char *result = string + stringBytes;

    if (result < string) {
        samDie("Invalid string size (pointer wraparound): %p + %lld",
               string, stringBytes);
    }

    return result;
}

/**
 * Does the basic decoding step, with syntactic but not semantic validation.
 */
static const char *justDecode(const char *string, zint stringBytes,
                              zint *result) {
    if (stringBytes <= 0) {
        samDie("Invalid string size: %lld", stringBytes);
    }

    char ch = *string;
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
 * Module functions
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
const char *samUtf8DecodeOne(const char *string, zint stringBytes,
                             zint *result) {
    string = justDecode(string, stringBytes, result);
    samAssertValidUnicode(*result);

    return string;
}

/** Documented in `unicode.h`. */
zint samUtf8DecodeStringSize(const char *string, zint stringBytes) {
    const char *stringEnd = getStringEnd(string, stringBytes);
    zint result = 0;

    while (string < stringEnd) {
        string = justDecode(string, stringEnd - string, NULL);
        result++;
    }

    return result;
}

/** Documented in `unicode.h`. */
void samUtf8DecodeStringToInts(const char *string, zint stringBytes,
                               zint *result) {
    const char *stringEnd = getStringEnd(string, stringBytes);

    while (string < stringEnd) {
        string = samUtf8DecodeOne(string, stringEnd - string, result);
        result++;
    }
}

/** Documented in `unicode.h`. */
void samUtf8DecodeStringToValues(const char *string, zint stringBytes,
                                 zvalue *result) {
    const char *stringEnd = getStringEnd(string, stringBytes);
    zint one = 0;

    while (string < stringEnd) {
        string = samUtf8DecodeOne(string, stringEnd - string, &one);
        *result = samIntletFromInt(one);
        result++;
    }
}

/** Documented in `unicode.h`. */
char *samUtf8EncodeOne(char *string, zint ch) {
    if (ch < 0x80) {
        if (string != NULL) {
            string[0] = (char) ch;
        }
        return string + 1;
    } else if (ch < 0x800) {
        if (string != NULL) {
            string[0] = (char) ((ch & 0x1f) | 0xc0);
            string[1] = (char) ((ch >> 5) | 0x80);
        }
        return string + 2;
    } else if (ch < 0x10000) {
        if (string != NULL) {
            string[0] = (char) ((ch & 0x0f) | 0xe0);
            string[1] = (char) (((ch >> 4) & 0x3f) | 0x80);
            string[2] = (char) ((ch >> 10) | 0x80);
        }
        return string + 3;
    } else if (ch < 0x200000) {
        if (string != NULL) {
            string[0] = (char) ((ch & 0x07) | 0xf0);
            string[1] = (char) (((ch >> 3) & 0x3f) | 0x80);
            string[2] = (char) (((ch >> 9) & 0x3f) | 0x80);
            string[3] = (char) ((ch >> 15) | 0x80);
        }
        return string + 4;
    } else if (ch < 0x4000000) {
        if (string != NULL) {
            string[0] = (char) ((ch & 0x03) | 0xf8);
            string[1] = (char) (((ch >> 2) & 0x3f) | 0x80);
            string[2] = (char) (((ch >> 8) & 0x3f) | 0x80);
            string[3] = (char) (((ch >> 14) & 0x3f) | 0x80);
            string[4] = (char) ((ch >> 20) | 0x80);
        }
        return string + 5;
    } else if (ch < 0x80000000) {
        if (string != NULL) {
            string[0] = (char) ((ch & 0x01) | 0xfc);
            string[1] = (char) (((ch >> 1) & 0x3f) | 0x80);
            string[2] = (char) (((ch >> 7) & 0x3f) | 0x80);
            string[3] = (char) (((ch >> 13) & 0x3f) | 0x80);
            string[4] = (char) (((ch >> 19) & 0x3f) | 0x80);
            string[5] = (char) ((ch >> 25) | 0x80);
        }
        return string + 6;
    } else if (ch < 0x100000000) {
        if (string != NULL) {
            string[0] = (char) 0xfe;
            string[1] = (char) ((ch & 0x3f) | 0x80);
            string[2] = (char) (((ch >> 6) & 0x3f) | 0x80);
            string[3] = (char) (((ch >> 12) & 0x3f) | 0x80);
            string[4] = (char) (((ch >> 18) & 0x3f) | 0x80);
            string[5] = (char) (((ch >> 24) & 0x3f) | 0x80);
            string[6] = (char) (((ch >> 30) & 0x3f) | 0x80);
        }
        return string + 7;
    } else {
        samDie("Out of range for UTF-8: %#llx", ch);
    }
}
