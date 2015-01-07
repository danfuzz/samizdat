// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "util.h"


//
// Private Definitions
//

/**
 * Asserts that the given `zint` value is valid as a Unicode
 * code point.
 */
static void assertValidCodePoint(zint value) {
    if ((value >= 0xd800) && (value <= 0xdfff)) {
        die("Invalid occurrence of surrogate code point: %x", value);
    } else if (value == 0xfffe) {
        die("Invalid occurrence of reversed-BOM.");
    } else if (value == 0xffff) {
        die("Invalid occurrence of not-a-character.");
    } else if (value >= 0x110000) {
        die("Invalid occurrence of high code point: %x", value);
    }
}

/**
 * Gets a pointer just past the end of the given string, asserting
 * validity of same. This just validates this as an address range,
 * not as valid string contents.
 */
static const char *getUtfEnd(zint utfBytes, const char *utf) {
    if (utfBytes < 0) {
        if (utfBytes != -1) {
            die("Invalid UTF-8 size: %d", utfBytes);
        }

        utfBytes = strlen(utf);
    }

    const char *result = utf + utfBytes;

    if (result < utf) {
        die("Invalid UTF-8 size (pointer wraparound): %p + %d",
            utf, utfBytes);
    }

    return result;
}

/**
 * Does the basic decoding step, with syntactic but not semantic validation.
 */
static const char *justDecode(zchar *result, zint utfBytes, const char *utf) {
    if (utfBytes <= 0) {
        die("Invalid UTF-8 size: %d", utfBytes);
    }

    unsigned char ch = *utf;
    zint value;  // `zint` and not `zchar` is for saner overflow detection.
    int extraBytes;
    zint minValue;

    utf++;
    utfBytes--;

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
            die("Invalid UTF-8 start byte: %02x", (zint) ch);
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
                    value = 0;
                    break;
                }
                case 0xf: {
                    // fe..ff: Invalid start bytes.
                    die("Invalid UTF-8 start byte: %02x", (zint) ch);
                    break;
                }
            }
        }
    }

    if (extraBytes > utfBytes) {
        die("Incomplete UTF-8 sequence.");
    }

    while (extraBytes > 0) {
        ch = *utf;
        utf++;
        extraBytes--;

        if ((ch & 0xc0) != 0x80) {
            die("Invalid UTF-8 continuation byte: %02x", (zint) ch);
        }

        value = (value << 6) | (ch & 0x3f);
    }

    if (value < minValue) {
        die("Overlong UTF-8 encoding of value: %x", value);
    }

    if (value >= 0x100000000) {
        die("Out-of-range UTF-8 encoded value: %x", value);
    }

    if (result != NULL) {
        *result = (zchar) value;
    }

    return utf;
}

/**
 * Decodes a UTF-8 encoded code point from the given string of the
 * given size in bytes, storing via the given `zchar *`. Returns
 * a pointer to the position just after the bytes that were decoded.
 */
static const char *decodeValid(zchar *result, zint utfBytes, const char *utf) {
    utf = justDecode(result, utfBytes, utf);
    assertValidCodePoint(*result);
    return utf;
}


//
// Exported Definitions
//

// Documented in header.
zint utf8DecodeStringSize(zint utfBytes, const char *utf) {
    const char *utfEnd = getUtfEnd(utfBytes, utf);
    zint result = 0;

    while (utf < utfEnd) {
        utf = justDecode(NULL, utfEnd - utf, utf);
        result++;
    }

    return result;
}

// Documented in header.
void utf8DecodeCharsFromString(zchar *result, zint utfBytes, const char *utf) {
    const char *utfEnd = getUtfEnd(utfBytes, utf);

    while (utf < utfEnd) {
        utf = decodeValid(result, utfEnd - utf, utf);
        result++;
    }
}

// Documented in header.
char *utf8EncodeOne(char *result, zint ch) {
    if (ch < 0) {
        die("Out of range for UTF-8: %x", ch);
    } else if (ch < 0x80) {
        if (result != NULL) {
            result[0] = (char) ch;
        }
        return result + 1;
    } else if (ch < 0x800) {
        if (result != NULL) {
            result[0] = (char) (0xc0 | (ch >> 6));
            result[1] = (char) (0x80 | (ch & 0x3f));
        }
        return result + 2;
    } else if (ch < 0x10000) {
        if (result != NULL) {
            result[0] = (char) (0xe0 |  (ch >> 12)        );
            result[1] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            result[2] = (char) (0x80 | ( ch        & 0x3f));
        }
        return result + 3;
    } else if (ch < 0x200000) {
        if (result != NULL) {
            result[0] = (char) (0xf0 |  (ch >> 18)        );
            result[1] = (char) (0x80 | ((ch >> 12) & 0x3f));
            result[2] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            result[3] = (char) (0x80 | ( ch        & 0x3f));
        }
        return result + 4;
    } else if (ch < 0x4000000) {
        if (result != NULL) {
            result[0] = (char) (0xf8 |  (ch >> 24)        );
            result[1] = (char) (0x80 | ((ch >> 18) & 0x3f));
            result[2] = (char) (0x80 | ((ch >> 12) & 0x3f));
            result[3] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            result[4] = (char) (0x80 | ( ch        & 0x3f));
        }
        return result + 5;
    } else if (ch < 0x80000000) {
        if (result != NULL) {
            result[0] = (char) (0xfc |  (ch >> 30)        );
            result[1] = (char) (0x80 | ((ch >> 24) & 0x3f));
            result[2] = (char) (0x80 | ((ch >> 18) & 0x3f));
            result[3] = (char) (0x80 | ((ch >> 12) & 0x3f));
            result[4] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            result[5] = (char) (0x80 | ( ch        & 0x3f));
        }
        return result + 6;
    } else if (ch < 0x100000000) {
        if (result != NULL) {
            result[0] = (char)  0xfe;
            result[1] = (char) (0x80 | ((ch >> 30) & 0x3f));
            result[2] = (char) (0x80 | ((ch >> 24) & 0x3f));
            result[3] = (char) (0x80 | ((ch >> 18) & 0x3f));
            result[4] = (char) (0x80 | ((ch >> 12) & 0x3f));
            result[5] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            result[6] = (char) (0x80 | ( ch        & 0x3f));
        }
        return result + 7;
    } else {
        die("Out of range for UTF-8: %x", ch);
    }
}
