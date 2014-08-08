// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "const.h"
#include "type/Box.h"
#include "type/DerivedData.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/define.h"
#include "zlimits.h"

#include "impl.h"


//
// Private Definitions
//

/** Array of single-character strings, for low character codes. */
static zvalue CACHED_CHARS[DAT_MAX_CACHED_CHAR + 1];

/**
 * Shared `zchar` array, used to avoid memory allocation in common cases.
 * **Note:** It is only safe to use this via `allocArray`.
 */
static zchar SHARED_ARRAY[DAT_MAX_STRING_SOFT];

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
    return datPayload(list);
}

/**
 * Allocates a string of the given size.
 */
static zvalue allocString(zint size) {
    zvalue result =
        datAllocValue(CLS_String, sizeof(StringInfo) + size * sizeof(zchar));

    getInfo(result)->size = size;
    return result;
}

/**
 * Asserts that the given value is a valid `zvalue`, and
 * furthermore that it is a string. If not, this aborts the process
 * with a diagnostic message.
 */
static void assertString(zvalue value) {
    assertHasClass(value, CLS_String);
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
    if (size < DAT_MAX_STRING_SOFT) {
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

/**
 * Shared implementation of `eq`, given valid string values.
 */
static bool uncheckedEq(zvalue string1, zvalue string2) {
    StringInfo *info1 = getInfo(string1);
    StringInfo *info2 = getInfo(string2);
    zint size1 = info1->size;
    zint size2 = info2->size;

    if (size1 != size2) {
        return false;
    }

    zchar *e1 = info1->elems;
    zchar *e2 = info2->elems;

    for (zint i = 0; i < size1; i++) {
        if (e1[i] != e2[i]) {
            return false;
        }
    }

    return true;
}

/**
 * Shared implementation of `order`, given valid string values.
 */
static zorder uncheckedZorder(zvalue string1, zvalue string2) {
    StringInfo *info1 = getInfo(string1);
    StringInfo *info2 = getInfo(string2);
    zchar *e1 = info1->elems;
    zchar *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zchar c1 = e1[i];
        zchar c2 = e2[i];

        if (c1 < c2) {
            return ZLESS;
        } else if (c1 > c2) {
            return ZMORE;
        }
    }

    if (size1 == size2) {
        return ZSAME;
    }

    return (size1 < size2) ? ZLESS : ZMORE;
}


//
// Exported Definitions
//

// Documented in header.
bool stringEq(zvalue string1, zvalue string2) {
    assertString(string1);
    assertString(string2);
    return uncheckedEq(string1, string2);
}

// Documented in header.
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

// Documented in header.
zvalue stringFromZchar(zchar value) {
    if (value <= DAT_MAX_CACHED_CHAR) {
        zvalue result = CACHED_CHARS[value];
        if (result != NULL) {
            return result;
        }
    }

    zvalue result = allocString(1);
    getInfo(result)->elems[0] = value;

    if (value <= DAT_MAX_CACHED_CHAR) {
        CACHED_CHARS[value] = result;
        datImmortalize(result);
    }

    return result;
}

// Documented in header.
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

// Documented in header.
zorder stringZorder(zvalue string1, zvalue string2) {
    assertString(string1);
    assertString(string2);
    return uncheckedZorder(string1, string2);
}

// Documented in header.
char *utf8DupFromString(zvalue string) {
    zint size = utf8SizeFromString(string) + 1;  // `+1` for the final `\0`.
    char *result = utilAlloc(size);

    utf8FromString(size, result, string);
    return result;
}

// Documented in header.
zint utf8FromString(zint resultSize, char *result, zvalue string) {
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

    zint finalSize = out - result;

    if (finalSize > resultSize) {
        die("Buffer too small for utf8-encoded string.");
    }

    return finalSize;
}

// Documented in header.
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

// Documented in header.
zchar zcharFromString(zvalue string) {
    assertStringSize1(string);

    StringInfo *info = getInfo(string);
    return info->elems[0];
}

// Documented in header.
void zcharsFromString(zchar *result, zvalue string) {
    assertString(string);

    StringInfo *info = getInfo(string);

    utilCpy(zchar, result, info->elems, info->size);
}


//
// Class Definition
//

// Documented in header.
zvalue EMPTY_STRING = NULL;

// Documented in header.
METH_IMPL(String, cat) {
    if (argCount == 1) {
        return args[0];
    }

    zint size = 0;

    for (zint i = 0; i < argCount; i++) {
        zvalue one = args[i];
        assertString(one);
        size += getInfo(one)->size;
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

// Documented in header.
METH_IMPL(String, collect) {
    zvalue string = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;

    StringInfo *info = getInfo(string);
    zchar *elems = info->elems;
    zint size = info->size;
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = stringFromZchar(elems[i]);
        zvalue one = (function == NULL) ? elem : FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

// Documented in header.
METH_IMPL(String, del) {
    zvalue string = args[0];
    zvalue n = args[1];

    StringInfo *info = getInfo(string);
    zchar *elems = info->elems;
    zint size = info->size;
    zint index = seqNthIndexLenient(n);

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

// Documented in header.
METH_IMPL(String, debugString) {
    zvalue string = args[0];
    zvalue quote = stringFromUtf8(1, "\"");

    return METH_CALL(cat, quote, string, quote);
}

// Documented in header.
METH_IMPL(String, fetch) {
    zvalue string = args[0];
    StringInfo *info = getInfo(string);

    switch (info->size) {
        case 0: {
            return NULL;
        }
        case 1: {
            return stringFromZchar(info->elems[0]);
        }
        default: {
            die("Invalid to call `fetch` on string with size > 1.");
        }
    }
}

// Documented in header.
METH_IMPL(String, get_size) {
    zvalue string = args[0];
    return intFromZint(getInfo(string)->size);
}

// Documented in header.
METH_IMPL(String, nextValue) {
    // This yields the first element directly (if any), and returns a
    // `SequenceGenerator` value to represent the rest.
    zvalue seq = args[0];
    zvalue box = args[1];
    zvalue first = nth(seq, 0);

    if (first == NULL) {
        // `seq` is empty.
        return NULL;
    } else {
        METH_CALL(store, box, first);
        return makeData(
            CLS_SequenceGenerator,
            mapFromArgs(
                STR_seq,   seq,
                STR_index, intFromZint(1),
                NULL));
    }
}

// Documented in header.
METH_IMPL(String, nth) {
    zvalue string = args[0];
    zvalue n = args[1];

    StringInfo *info = getInfo(string);
    zint index = seqNthIndexStrict(info->size, n);

    if (index < 0) {
        return NULL;
    }

    return stringFromZchar(info->elems[index]);
}

// Documented in header.
METH_IMPL(String, put) {
    zvalue string = args[0];
    zvalue n = args[1];
    zvalue value = args[2];

    assertStringSize1(value);

    StringInfo *info = getInfo(string);
    zchar *elems = info->elems;
    zint size = info->size;
    zint index = seqPutIndexStrict(size, n);

    if (index == size) {
        // This is an append operation.
        return METH_CALL(cat, string, value);
    }

    zchar *chars = allocArray(size);
    zcharsFromString(chars, string);
    chars[index] = zcharFromString(value);
    zvalue result = stringFromZchars(size, chars);
    freeArray(chars);
    return result;
}

// Documented in header.
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

// Documented in header.
static zvalue doSlice(bool inclusive, zint argCount, const zvalue *args) {
    zvalue string = args[0];
    StringInfo *info = getInfo(string);
    zint start;
    zint end;

    seqConvertSliceArgs(&start, &end, inclusive, info->size, argCount, args);

    if (start == -1) {
        return NULL;
    } else {
        return stringFromZchars(end - start, &info->elems[start]);
    }
}

// Documented in header.
METH_IMPL(String, sliceExclusive) {
    return doSlice(false, argCount, args);
}

// Documented in header.
METH_IMPL(String, sliceInclusive) {
    return doSlice(true, argCount, args);
}

// Documented in header.
METH_IMPL(String, toInt) {
    zvalue string = args[0];
    return intFromZint(zcharFromString(string));
}

// Documented in header.
METH_IMPL(String, toNumber) {
    zvalue string = args[0];
    return intFromZint(zcharFromString(string));
}

// Documented in header.
METH_IMPL(String, toString) {
    zvalue string = args[0];
    return string;
}

// Documented in header.
METH_IMPL(String, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `String`.

    assertString(other);
    return uncheckedEq(value, other) ? value : NULL;
}

// Documented in header.
METH_IMPL(String, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];  // Note: Not guaranteed to be a `String`.

    assertString(other);
    switch (uncheckedZorder(value, other)) {
        case ZLESS: return INT_NEG1;
        case ZSAME: return INT_0;
        case ZMORE: return INT_1;
    }
}

// Documented in header.
METH_IMPL(String, valueList) {
    zvalue string = args[0];

    StringInfo *info = getInfo(string);
    zint size = info->size;
    zchar *elems = info->elems;
    zvalue result[size];

    for (zint i = 0; i < size; i++) {
        result[i] = stringFromZchar(elems[i]);
    }

    return listFromArray(size, result);
}

/** Initializes the module. */
MOD_INIT(String) {
    MOD_USE(Sequence);
    MOD_USE(OneOff);

    // Note: The `objectModel` module initializes `CLS_String`.

    METH_BIND(String, cat);
    METH_BIND(String, collect);
    METH_BIND(String, debugString);
    METH_BIND(String, del);
    METH_BIND(String, fetch);
    METH_BIND(String, get_size);
    METH_BIND(String, nextValue);
    METH_BIND(String, nth);
    METH_BIND(String, put);
    METH_BIND(String, reverse);
    METH_BIND(String, sliceExclusive);
    METH_BIND(String, sliceInclusive);
    METH_BIND(String, toInt);
    METH_BIND(String, toNumber);
    METH_BIND(String, toString);
    METH_BIND(String, totalEq);
    METH_BIND(String, totalOrder);
    METH_BIND(String, valueList);
    seqBind(CLS_String);

    EMPTY_STRING = allocString(0);
    datImmortalize(EMPTY_STRING);
}

// Documented in header.
zvalue CLS_String = NULL;
