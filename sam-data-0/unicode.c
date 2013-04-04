/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "unicode.h"
#include "util.h"

/*
 * Helper functions
 */

/**
 * Does the basic decoding step, with syntactic but not semantic validation.
 */
static const zbyte *justDecode(const zbyte *string, zint *result) {
    zbyte ch = *string;
    zint value;
    int extraBytes;
    zint minValue;

    string++;

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
	    samDie("Invalid UTF-8 start byte: 0x%02x", (int) ch);
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
		    samDie("Invalid UTF-8 start byte: 0x%02x", (int) ch);
		    break;
		}
	    }
	}
    }

    while (extraBytes > 0) {
	ch = *string;
	string++;
	
	if ((ch & 0xc0) != 0x80) {
	    samDie("Invalid UTF-8 continuation byte: 0x%02x", (int) ch);
	}

	value = (value << 6) | (ch & 0x3f);
    }

    if (value < minValue) {
	samDie("Overly long UTF-8 encoding of value: 0x%llx", value);
    }

    if (value >= 0x100000000LL) {
	samDie("Out-of-range UTF-8 encoded value: 0x%llx", value);
    }

    *result = value;
    return string;
}


/*
 * API implementation
 */

/** Documented in API header. */
const zbyte *samDecodeUtf8(const zbyte *string, zint *result) {
    string = justDecode(string, result);

    if ((*result >= 0xd800) && (*result <= 0xdfff)) {
	samDie("Invalid occurrence of surrogate code point: 0x%04x",
	       (int) result);
    } else if (*result == 0xfffe) {
	samDie("Invalid occurrence of reversed-BOM.");
    } else if (*result == 0xffff) {
	samDie("Invalid occurrence of not-a-character.");
    }

    return string;
}
