/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "zlimits.h"

#include <string.h>


/*
 * Helper definitions
 */

/** Array of single-character strings, for low character codes. */
static zvalue CACHED_CHARS[PB_MAX_CACHED_CHAR + 1];

/**
 * String structure.
 */
typedef struct {
    /** Number of characters. */
    zint size;

    /** Characters of the string, in index order. */
    zchar elems[/*size*/];
} StringInfo;

/**
 * Allocates a string of the given size.
 */
static zvalue allocString(zint size) {
    zvalue result =
        pbAllocValue(TYPE_String, sizeof(StringInfo) + size * sizeof(zchar));

    ((StringInfo *) pbPayload(result))->size = size;
    return result;
}

/**
 * Gets the size of a string.
 */
static zint stringSizeOf(zvalue string) {
    return ((StringInfo *) pbPayload(string))->size;
}

/**
 * Gets the array of `zvalue` elements from a list.
 */
static zchar *stringElems(zvalue string) {
    return ((StringInfo *) pbPayload(string))->elems;
}


/*
 * Exported functions
 */

/* Documented in header. */
void pbAssertString(zvalue value) {
    assertTypeIs(value, TYPE_String);
}

/* Documented in header. */
void pbAssertStringSize1(zvalue value) {
    pbAssertString(value);
    if (pbSize(value) != 1) {
        die("Not a size 1 string.");
    }
}

/* Documented in header. */
zvalue stringAdd(zvalue str1, zvalue str2) {
    pbAssertString(str1);
    pbAssertString(str2);

    zint size1 = stringSizeOf(str1);
    zint size2 = stringSizeOf(str2);

    if (size1 == 0) {
        return str2;
    } else if (size2 == 0) {
        return str1;
    }

    zvalue result = allocString(size1 + size2);

    memcpy(stringElems(result), stringElems(str1),
           size1 * sizeof(zchar));
    memcpy(stringElems(result) + size1, stringElems(str2),
           size2 * sizeof(zchar));
    return result;
}

/* Documented in header. */
zvalue stringFromUtf8(zint stringBytes, const char *string) {
    if (stringBytes == -1) {
        stringBytes = strlen(string);
    } else if (stringBytes < 0) {
        die("Invalid string size: %lld", stringBytes);
    }

    zint decodedSize = utf8DecodeStringSize(stringBytes, string);

    switch (decodedSize) {
        case 0: return EMPTY_STRING;
        case 1: {
            // Call into `stringFromChar` since that's what handles caching
            // of single-character strings.
            zchar ch;
            utf8DecodeCharsFromString(&ch, stringBytes, string);
            return stringFromZchar(ch);
        }
    }

    zvalue result = allocString(decodedSize);

    utf8DecodeCharsFromString(stringElems(result), stringBytes, string);
    return result;
}

/* Documented in header. */
zvalue stringFromZchar(zchar value) {
    if (value <= PB_MAX_CACHED_CHAR) {
        zvalue result = CACHED_CHARS[value];
        if (result != NULL) {
            return result;
        }
    }

    zvalue result = allocString(1);
    stringElems(result)[0] = value;

    if (value <= PB_MAX_CACHED_CHAR) {
        CACHED_CHARS[value] = result;
        pbImmortalize(result);
    }

    return result;
}

/* Documented in header. */
zvalue stringFromZchars(zint size, const zchar *chars) {
    // Deal with special cases. This calls into `stringFromChar` since that's
    // what handles caching of single-character strings.
    switch (size) {
        case 0: return EMPTY_STRING;
        case 1: return stringFromZchar(chars[0]);
    }

    zvalue result = allocString(size);

    memcpy(stringElems(result), chars, size * sizeof(zchar));
    return result;
}

/* Documented in header. */
zint stringNth(zvalue string, zint n) {
    pbAssertString(string);

    if ((n < 0) || (n >= stringSizeOf(string))) {
        return -1;
    }

    return stringElems(string)[n];
}

/* Documented in header. */
zvalue stringSlice(zvalue string, zint start, zint end) {
    pbAssertString(string);
    pbAssertSliceRange(stringSizeOf(string), start, end);

    return stringFromZchars(end - start, &stringElems(string)[start]);
}

/* Documented in header. */
void utf8FromString(zint resultSize, char *result, zvalue string) {
    pbAssertString(string);

    zint size = stringSizeOf(string);
    zchar *elems = stringElems(string);
    char *out = result;

    for (zint i = 0; i < size; i++) {
        out = utf8EncodeOne(out, elems[i]);
    }

    *out = '\0';
    out++;

    if ((out - result) > resultSize) {
        die("Buffer too small for utf8-encoded string.");
    }
}

/* Documented in header. */
zint utf8SizeFromString(zvalue string) {
    pbAssertString(string);

    zint size = stringSizeOf(string);
    zchar *elems = stringElems(string);
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        result += (utf8EncodeOne(NULL, elems[i]) - (char *) NULL);
    }

    return result;
}

/* Documented in header. */
void zcharsFromString(zchar *result, zvalue string) {
    pbAssertString(string);

    memcpy(result, stringElems(string), stringSizeOf(string) * sizeof(zchar));
}


/*
 * Type binding
 */

/* Documented in header. */
zvalue EMPTY_STRING = NULL;

/* Documented in header. */
static zvalue String_debugString(zvalue state,
        zint argCount, const zvalue *args) {
    zvalue string = args[0];
    zvalue quote = stringFromUtf8(1, "\"");

    zvalue result = stringAdd(quote, string);
    result = stringAdd(result, quote);

    return result;
}

/* Documented in header. */
static zvalue String_eq(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zint sz1 = stringSizeOf(v1);
    zint sz2 = stringSizeOf(v2);

    if (sz1 != sz2) {
        return NULL;
    }

    zchar *e1 = stringElems(v1);
    zchar *e2 = stringElems(v2);

    for (zint i = 0; i < sz1; i++) {
        if (e1[i] != e2[i]) {
            return NULL;
        }
    }

    return v2;
}

/* Documented in header. */
static zvalue String_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zchar *e1 = stringElems(v1);
    zchar *e2 = stringElems(v2);
    zint sz1 = stringSizeOf(v1);
    zint sz2 = stringSizeOf(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zchar c1 = e1[i];
        zchar c2 = e2[i];

        if (c1 < c2) {
            return PB_NEG1;
        } else if (c1 > c2) {
            return PB_1;
        }
    }

    if (sz1 == sz2) {
        return PB_0;
    }

    return (sz1 < sz2) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
static zvalue String_size(zvalue state, zint argCount, const zvalue *args) {
    zvalue string = args[0];
    return intFromZint(stringSizeOf(string));
}

/* Documented in header. */
void pbBindString(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_String`.

    gfnBindCore(GFN_debugString, TYPE_String, String_debugString);
    gfnBindCore(GFN_eq,          TYPE_String, String_eq);
    gfnBindCore(GFN_order,       TYPE_String, String_order);
    gfnBindCore(GFN_size,        TYPE_String, String_size);

    EMPTY_STRING = allocString(0);
    pbImmortalize(EMPTY_STRING);
}

/* Documented in header. */
zvalue TYPE_String = NULL;
