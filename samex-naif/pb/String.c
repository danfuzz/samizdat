/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

/** Array of single-character strings, for low character codes. */
static zvalue CACHED_CHARS[PB_MAX_CACHED_CHAR + 1];

/**
 * Shared `zchar` array, used to avoid memory allocation in common cases.
 * **Note:** It is only safe to use this via `allocArray`.
 */
static zchar SHARED_ARRAY[PB_SOFT_MAX_STRING];

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

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string. If not, this aborts the process
 * with a diagnostic message.
 */
static void assertString(zvalue value) {
    assertHasType(value, TYPE_String);
}

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string, and even furthermore that its size
 * is `1`. If not, this aborts the process with a diagnostic message.
 */
static void assertStringSize1(zvalue value) {
    assertString(value);
    if (getInfo(value)->size != 1) {
        die("Not a size 1 string.");
    }
}

/**
 * Allocates a `zchar[]` of the given size.
 *
 * **Note:** It is only safe to use this if external code *cannot* be called
 * while the allocation is active.
 */
static zchar *allocArray(zint size) {
    if (size < PB_SOFT_MAX_STRING) {
        return SHARED_ARRAY;
    } else {
        return utilAlloc(size * sizeof(zchar));
    }
}

/**
 * Frees a `zchar[]` previously allocated by `allocArray`.
 */
static void freeArray(zchar *array) {
    if (array != SHARED_ARRAY) {
        utilFree(array);
    }
}


/*
 * Exported Definitions
 */

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

    utilCpy(zchar, getInfo(result)->elems, chars, size);
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

    utilCpy(zchar, result, info->elems, info->size);
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

    zchar *chars = allocArray(size);

    for (zint i = 0, at = 0; i < argCount; i++) {
        zcharsFromString(&chars[at], args[i]);
        at += getInfo(args[i])->size;
    }

    zvalue result = stringFromZchars(size, chars);
    freeArray(chars);
    return result;
}

/* Documented in header. */
METH_IMPL(String, del) {
    zvalue string = args[0];
    zvalue n = args[1];

    StringInfo *info = getInfo(string);
    zchar *elems = info->elems;
    zint size = info->size;
    zint index = collNthIndexLenient(n);

    if ((index < 0) || (index >= size)) {
        return string;
    }

    zchar *chars = allocArray(size - 1);
    utilCpy(zchar, chars, elems, index);
    utilCpy(zchar, &chars[index], &elems[index + 1], (size - index - 1));
    zvalue result = stringFromZchars(size - 1, chars);
    freeArray(chars);
    return result;
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
METH_IMPL(String, put) {
    zvalue string = args[0];
    zvalue n = args[1];
    zvalue value = args[2];

    assertStringSize1(value);

    StringInfo *info = getInfo(string);
    zchar *elems = info->elems;
    zint size = info->size;
    zint index = collPutIndexStrict(size, n);

    if (index == size) {
        // This is an append operation.
        return GFN_CALL(cat, string, value);
    }

    zchar *chars = allocArray(size);
    zcharsFromString(chars, string);
    chars[index] = zcharFromString(value);
    zvalue result = stringFromZchars(size, chars);
    freeArray(chars);
    return result;
}

/* Documented in header. */
METH_IMPL(String, reverse) {
    zvalue string = args[0];

    StringInfo *info = getInfo(string);
    zint size = info->size;
    zchar *elems = info->elems;
    zchar *arr = allocArray(size);

    for (zint i = 0, j = size - 1; i < size; i++, j--) {
        arr[i] = elems[j];
    }

    zvalue result = stringFromZchars(size, arr);
    freeArray(arr);
    return result;
}

/* Documented in header. */
METH_IMPL(String, sizeOf) {
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
METH_IMPL(String, toInt) {
    zvalue string = args[0];
    return intFromZint(zcharFromString(string));
}

/* Documented in header. */
METH_IMPL(String, toNumber) {
    zvalue string = args[0];
    return intFromZint(zcharFromString(string));
}

/* Documented in header. */
METH_IMPL(String, toString) {
    zvalue string = args[0];
    return string;
}

/* Documented in header. */
METH_IMPL(String, totEq) {
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
METH_IMPL(String, totOrder) {
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
            return INT_NEG1;
        } else if (c1 > c2) {
            return INT_1;
        }
    }

    if (size1 == size2) {
        return INT_0;
    }

    return (size1 < size2) ? INT_NEG1 : INT_1;
}

/** Initializes the module. */
MOD_INIT(String) {
    MOD_USE(Collection);
    MOD_USE(OneOff);

    // Note: The `typeSystem` module initializes `TYPE_String`.

    METH_BIND(String, cat);
    METH_BIND(String, debugString);
    METH_BIND(String, del);
    METH_BIND(String, nth);
    METH_BIND(String, put);
    METH_BIND(String, reverse);
    METH_BIND(String, sizeOf);
    METH_BIND(String, slice);
    METH_BIND(String, toInt);
    METH_BIND(String, toNumber);
    METH_BIND(String, toString);
    METH_BIND(String, totEq);
    METH_BIND(String, totOrder);
    seqBind(TYPE_String);

    EMPTY_STRING = allocString(0);
    pbImmortalize(EMPTY_STRING);
}

/* Documented in header. */
zvalue TYPE_String = NULL;
