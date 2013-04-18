/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "util.h"

#include <stdlib.h>


/*
 * Helper functions
 */

/**
 * Does the basic decoding step, with syntactic but not semantic validation.
 */
static const char *justDecode(const char *string, zint stringBytes,
                              zchar *result) {
    if (stringBytes <= 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    unsigned char ch = *string;
    zint value; // Note: zint and not zchar for easier overflow detection.
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
            die("Invalid UTF-8 start byte: %#02x", (int) ch);
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
                    die("Invalid UTF-8 start byte: %#02x", (int) ch);
                    break;
                }
            }
        }
    }

    if (extraBytes > stringBytes) {
        die("Incomplete UTF-8 sequence.");
    }

    while (extraBytes > 0) {
        ch = *string;
        string++;
        extraBytes--;

        if ((ch & 0xc0) != 0x80) {
            die("Invalid UTF-8 continuation byte: %#02x", (int) ch);
        }

        value = (value << 6) | (ch & 0x3f);
    }

    if (value < minValue) {
        die("Overlong UTF-8 encoding of value: %#llx", value);
    }

    if (value >= 0x100000000LL) {
        die("Out-of-range UTF-8 encoded value: %#llx", value);
    }

    if (result != NULL) {
        *result = (zchar) value;
    }

    return string;
}


/*
 * Exported functions
 */

/* Documented in header. */
void uniAssertValid(zint value) {
    if ((value >= 0xd800) && (value <= 0xdfff)) {
        die("Invalid occurrence of surrogate code point: %#04x",
               (int) value);
    } else if (value == 0xfffe) {
        die("Invalid occurrence of reversed-BOM.");
    } else if (value == 0xffff) {
        die("Invalid occurrence of not-a-character.");
    } else if (value >= 0x110000) {
        die("Invalid occurrence of high code point: %#llx", value);
    }
}

/* Documented in header. */
const char *utf8DecodeOne(const char *string, zint stringBytes,
                          zchar *result) {
    string = justDecode(string, stringBytes, result);
    uniAssertValid(*result);

    return string;
}

/* Documented in header. */
zint utf8DecodeStringSize(const char *string, zint stringBytes) {
    const char *stringEnd = strGetEnd(string, stringBytes);
    zint result = 0;

    while (string < stringEnd) {
        string = justDecode(string, stringEnd - string, NULL);
        result++;
    }

    return result;
}

/* Documented in header. */
void utf8DecodeStringToChars(const char *string, zint stringBytes,
                             zchar *result) {
    const char *stringEnd = strGetEnd(string, stringBytes);

    while (string < stringEnd) {
        string = utf8DecodeOne(string, stringEnd - string, result);
        result++;
    }
}

/* Documented in header. */
char *utf8EncodeOne(char *string, zint ch) {
    if (ch < 0) {
        die("Out of range for UTF-8: %#llx", ch);
    } else if (ch < 0x80) {
        if (string != NULL) {
            string[0] = (char) ch;
        }
        return string + 1;
    } else if (ch < 0x800) {
        if (string != NULL) {
            string[0] = (char) (0xc0 | (ch >> 6));
            string[1] = (char) (0x80 | (ch & 0x3f));
        }
        return string + 2;
    } else if (ch < 0x10000) {
        if (string != NULL) {
            string[0] = (char) (0xe0 |  (ch >> 12)        );
            string[1] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            string[2] = (char) (0x80 | ( ch        & 0x3f));
        }
        return string + 3;
    } else if (ch < 0x200000) {
        if (string != NULL) {
            string[0] = (char) (0xf0 |  (ch >> 18)        );
            string[1] = (char) (0x80 | ((ch >> 12) & 0x3f));
            string[2] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            string[3] = (char) (0x80 | ( ch        & 0x3f));
        }
        return string + 4;
    } else if (ch < 0x4000000) {
        if (string != NULL) {
            string[0] = (char) (0xf8 |  (ch >> 24)        );
            string[1] = (char) (0x80 | ((ch >> 18) & 0x3f));
            string[2] = (char) (0x80 | ((ch >> 12) & 0x3f));
            string[3] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            string[4] = (char) (0x80 | ( ch        & 0x3f));
        }
        return string + 5;
    } else if (ch < 0x80000000) {
        if (string != NULL) {
            string[0] = (char) (0xfc |  (ch >> 30)        );
            string[1] = (char) (0x80 | ((ch >> 24) & 0x3f));
            string[2] = (char) (0x80 | ((ch >> 18) & 0x3f));
            string[3] = (char) (0x80 | ((ch >> 12) & 0x3f));
            string[4] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            string[5] = (char) (0x80 | ( ch        & 0x3f));
        }
        return string + 6;
    } else if (ch < 0x100000000) {
        if (string != NULL) {
            string[0] = (char)  0xfe;
            string[1] = (char) (0x80 | ((ch >> 30) & 0x3f));
            string[2] = (char) (0x80 | ((ch >> 24) & 0x3f));
            string[3] = (char) (0x80 | ((ch >> 18) & 0x3f));
            string[4] = (char) (0x80 | ((ch >> 12) & 0x3f));
            string[5] = (char) (0x80 | ((ch >> 6)  & 0x3f));
            string[6] = (char) (0x80 | ( ch        & 0x3f));
        }
        return string + 7;
    } else {
        die("Out of range for UTF-8: %#llx", ch);
    }
}
