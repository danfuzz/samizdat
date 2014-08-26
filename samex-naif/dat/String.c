// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Box.h"
#include "type/Int.h"
#include "type/List.h"
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

    /**
     * Another string which contains the actual content, or `NULL` if the
     * content is in `elems` (below).
     */
    zvalue contentString;

    /** Offset into `contentString` where this string's content is. */
    zint contentOffset;

    /**
     * Characters of the string, in index order, if `contentString` is
     * `NULL`.
     */
    zchar elems[/*size*/];
} StringInfo;

/**
 * Gets a pointer to the value's info.
 */
static StringInfo *getInfo(zvalue string) {
    return datPayload(string);
}

/**
 * Gets a pointer to the actual characters, given an info pointer.
 */
static zchar *getElems(StringInfo *info) {
    if (info->contentString != NULL) {
        return &getInfo(info->contentString)->elems[info->contentOffset];
    }

    return info->elems;
}

/**
 * Allocates a string with the given size allocated with the value.
 */
static zvalue allocString(zint size) {
    zvalue result =
        datAllocValue(CLS_String, sizeof(StringInfo) + size * sizeof(zchar));

    getInfo(result)->size = size;
    return result;
}

/**
 * Makes a string that refers to a content string. Does not do any type or
 * bounds checking. It *does* shunt from an already-indirect string to the
 * ultimate bearer of content.
 */
static zvalue makeIndirectString(zvalue string, zint offset, zint size) {
    StringInfo *info = getInfo(string);

    if (info->contentString != NULL) {
        string = info->contentString;
        offset += info->contentOffset;
    }

    zvalue result = datAllocValue(CLS_String, sizeof(StringInfo));
    info = getInfo(result);

    info->size = size;
    info->contentString = string;
    info->contentOffset = offset;

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
    if (string1 == string2) {
        // Easy out.
        return true;
    }

    StringInfo *info1 = getInfo(string1);
    StringInfo *info2 = getInfo(string2);
    zint size1 = info1->size;
    zint size2 = info2->size;

    if (size1 != size2) {
        return false;
    }

    zchar *e1 = getElems(info1);
    zchar *e2 = getElems(info2);

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
    if (string1 == string2) {
        // Easy out.
        return ZSAME;
    }

    StringInfo *info1 = getInfo(string1);
    StringInfo *info2 = getInfo(string2);
    zchar *e1 = getElems(info1);
    zchar *e2 = getElems(info2);
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
    zchar *elems = getElems(info);
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
    zchar *elems = getElems(info);
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
    return getElems(info)[0];
}

// Documented in header.
void zcharsFromString(zchar *result, zvalue string) {
    assertString(string);

    StringInfo *info = getInfo(string);

    utilCpy(zchar, result, getElems(info), info->size);
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
    zchar *elems = getElems(info);
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
    zchar *elems = getElems(info);
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
            return stringFromZchar(getElems(info)[0]);
        }
        default: {
            die("Invalid to call `fetch` on string with size > 1.");
        }
    }
}

// Documented in header.
METH_IMPL(String, gcMark) {
    zvalue string = args[0];
    StringInfo *info = getInfo(string);

    datMark(info->contentString);
    return NULL;
}

// Documented in header.
METH_IMPL(String, get_size) {
    zvalue string = args[0];
    return intFromZint(getInfo(string)->size);
}

// Documented in header.
METH_IMPL(String, nextValue) {
    zvalue string = args[0];
    zvalue box = args[1];
    StringInfo *info = getInfo(string);
    zint size = info->size;

    switch (size) {
        case 0: {
            // `string` is empty.
            return NULL;
        }
        case 1: {
            // `string` is a single character, so it can be yielded directly.
            METH_CALL(store, box, string);
            return EMPTY_STRING;
        }
        default: {
            // The hard case. Make a single-character string for the yield.
            // Make an indirect string for the return value, to avoid the
            // churn of copying and re-re-...-copying the content.
            zchar *elems = getElems(info);
            METH_CALL(store, box, stringFromZchar(elems[0]));
            return makeIndirectString(string, 1, size - 1);
        }
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

    return stringFromZchar(getElems(info)[index]);
}

// Documented in header.
METH_IMPL(String, put) {
    zvalue string = args[0];
    zvalue n = args[1];
    zvalue value = args[2];

    assertStringSize1(value);

    StringInfo *info = getInfo(string);
    zchar *elems = getElems(info);
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
    zchar *elems = getElems(info);
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
    }

    zint size = end - start;

    if (size > 16) {
        // Share storage for large results.
        return makeIndirectString(string, start, size);
    } else {
        return stringFromZchars(size, &getElems(info)[start]);
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
    zchar *elems = getElems(info);
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
    classBindMethods(CLS_String,
        NULL,
        symbolTableFromArgs(
            SYM_METH(String, cat),
            SYM_METH(String, collect),
            SYM_METH(String, debugString),
            SYM_METH(String, del),
            SYM_METH(String, fetch),
            SYM_METH(String, gcMark),
            SYM_METH(String, get_size),
            SYM_METH(String, nextValue),
            SYM_METH(String, nth),
            SYM_METH(String, put),
            SYM_METH(String, reverse),
            SYM_METH(String, sliceExclusive),
            SYM_METH(String, sliceInclusive),
            SYM_METH(String, toInt),
            SYM_METH(String, toNumber),
            SYM_METH(String, toString),
            SYM_METH(String, totalEq),
            SYM_METH(String, totalOrder),
            SYM_METH(String, valueList),
            SYM_NAME(get),        FUN_Sequence_get,
            SYM_NAME(keyList),    FUN_Sequence_keyList,
            SYM_NAME(nthMapping), FUN_Sequence_nthMapping,
            NULL));

    EMPTY_STRING = allocString(0);
    datImmortalize(EMPTY_STRING);
}

// Documented in header.
zvalue CLS_String = NULL;
