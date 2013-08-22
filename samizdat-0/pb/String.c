/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"

#include <string.h>


/*
 * Private Definitions
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
 * Gets a pointer to the value's info.
 */
static StringInfo *getInfo(zvalue list) {
    return pbPayload(list);
}

/**
 * Allocates a string of the given size.
 */
static zvalue allocString(zint size) {
    zvalue result =
        pbAllocValue(TYPE_String, sizeof(StringInfo) + size * sizeof(zchar));

    getInfo(result)->size = size;
    return result;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
void assertString(zvalue value) {
    assertHasType(value, TYPE_String);
}

/* Documented in header. */
void assertStringSize1(zvalue value) {
    assertString(value);
    if (getInfo(value)->size != 1) {
        die("Not a size 1 string.");
    }
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

    utf8DecodeCharsFromString(getInfo(result)->elems, stringBytes, string);
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
    getInfo(result)->elems[0] = value;

    if (value <= PB_MAX_CACHED_CHAR) {
        CACHED_CHARS[value] = result;
        pbImmortalize(result);
    }

    return result;
}

/* Documented in header. */
zvalue stringFromZchars(zint size, const zchar *chars) {
    // Deal with special cases. This calls into `stringFromZchar` since that's
    // what handles caching of single-character strings.
    switch (size) {
        case 0: return EMPTY_STRING;
        case 1: return stringFromZchar(chars[0]);
    }

    zvalue result = allocString(size);

    memcpy(getInfo(result)->elems, chars, size * sizeof(zchar));
    return result;
}

/* Documented in header. */
void utf8FromString(zint resultSize, char *result, zvalue string) {
    assertString(string);

    StringInfo *info = getInfo(string);
    zint size = info->size;
    zchar *elems = info->elems;
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
    assertString(string);

    StringInfo *info = getInfo(string);
    zint size = info->size;
    zchar *elems = info->elems;
    zint result = 0;

    for (zint i = 0; i < size; i++) {
        result += (utf8EncodeOne(NULL, elems[i]) - (char *) NULL);
    }

    return result;
}

/* Documented in header. */
zchar zcharFromString(zvalue string) {
    assertStringSize1(string);

    StringInfo *info = getInfo(string);
    return info->elems[0];
}

/* Documented in header. */
void zcharsFromString(zchar *result, zvalue string) {
    assertString(string);

    StringInfo *info = getInfo(string);

    memcpy(result, info->elems, info->size * sizeof(zchar));
}


/*
 * Type Definition
 */

/* Documented in header. */
zvalue EMPTY_STRING = NULL;

/* Documented in header. */
METH_IMPL(String, cat) {
    if (argCount == 1) {
        return args[0];
    }

    zint size = 0;

    for (zint i = 0; i < argCount; i++) {
        size += getInfo(args[i])->size;
    }

    zchar chars[size];

    for (zint i = 0, at = 0; i < argCount; i++) {
        zcharsFromString(&chars[at], args[i]);
        at += getInfo(args[i])->size;
    }

    return stringFromZchars(size, chars);
}

/* Documented in header. */
METH_IMPL(String, debugString) {
    zvalue string = args[0];
    zvalue quote = stringFromUtf8(1, "\"");

    return GFN_CALL(cat, quote, string, quote);
}

/* Documented in header. */
METH_IMPL(String, nth) {
    zvalue string = args[0];
    zvalue n = args[1];

    StringInfo *info = getInfo(string);
    zint index = collNthIndexStrict(info->size, n);

    if (index < 0) {
        return NULL;
    }

    return stringFromZchar(info->elems[index]);
}

/* Documented in header. */
METH_IMPL(String, perEq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    StringInfo *info1 = getInfo(v1);
    StringInfo *info2 = getInfo(v2);
    zint size1 = info1->size;
    zint size2 = info2->size;

    if (size1 != size2) {
        return NULL;
    }

    zchar *e1 = info1->elems;
    zchar *e2 = info2->elems;

    for (zint i = 0; i < size1; i++) {
        if (e1[i] != e2[i]) {
            return NULL;
        }
    }

    return v2;
}

/* Documented in header. */
METH_IMPL(String, perOrder) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    StringInfo *info1 = getInfo(v1);
    StringInfo *info2 = getInfo(v2);
    zchar *e1 = info1->elems;
    zchar *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zchar c1 = e1[i];
        zchar c2 = e2[i];

        if (c1 < c2) {
            return PB_NEG1;
        } else if (c1 > c2) {
            return PB_1;
        }
    }

    if (size1 == size2) {
        return PB_0;
    }

    return (size1 < size2) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
METH_IMPL(String, size) {
    zvalue string = args[0];
    return intFromZint(getInfo(string)->size);
}

/* Documented in header. */
METH_IMPL(String, slice) {
    zvalue string = args[0];
    StringInfo *info = getInfo(string);
    zint start;
    zint end;

    collConvertSliceArgs(&start, &end, info->size, argCount, args);
    return stringFromZchars(end - start, &info->elems[start]);
}

/* Documented in header. */
void pbBindString(void) {
    // Note: The type `Type` is responsible for initializing `TYPE_String`.

    METH_BIND(String, cat);
    METH_BIND(String, debugString);
    METH_BIND(String, nth);
    METH_BIND(String, perEq);
    METH_BIND(String, perOrder);
    METH_BIND(String, size);
    METH_BIND(String, slice);
    seqBind(TYPE_String);

    EMPTY_STRING = allocString(0);
    pbImmortalize(EMPTY_STRING);
}

/* Documented in header. */
zvalue TYPE_String = NULL;
