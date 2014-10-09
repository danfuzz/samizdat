// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Box.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/String.h"
#include "type/define.h"

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
    /** Size and pointer to characters. */
    zstring s;

    /**
     * Another string which contains the actual content, or `NULL` if the
     * content is in `content` (below). This is just used to keep the
     * characters from getting gc'ed out from under this instance.
     */
    zvalue contentString;

    /** Characters of the string, if `contentString` is `NULL`. */
    zchar content[/*s.size*/];
} StringInfo;

/**
 * Gets a pointer to the value's info.
 */
static StringInfo *getInfo(zvalue string) {
    return datPayload(string);
}

/**
 * Allocates a string with the given size allocated with the value.
 */
static zvalue allocString(zint size) {
    zvalue result =
        datAllocValue(CLS_String, sizeof(StringInfo) + size * sizeof(zchar));
    StringInfo *info = getInfo(result);

    info->s.size = size;
    info->s.chars = info->content;
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
    }

    zvalue result = datAllocValue(CLS_String, sizeof(StringInfo));
    StringInfo *resultInfo = getInfo(result);

    resultInfo->s.size = size;
    resultInfo->s.chars = &info->s.chars[offset];
    resultInfo->contentString = string;

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
    if (getInfo(value)->s.size != 1) {
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
        return true;
    }

    return zstringEq(getInfo(string1)->s, getInfo(string2)->s);
}

/**
 * Shared implementation of `order`, given valid string values.
 */
static zorder uncheckedZorder(zvalue string1, zvalue string2) {
    if (string1 == string2) {
        return ZSAME;
    }

    return zstringOrder(getInfo(string1)->s, getInfo(string2)->s);
}

/**
 * Helper that does most of the work of the `slice*` methods.
 */
static zvalue doSlice(zvalue ths, bool inclusive,
        zvalue startArg, zvalue endArg) {
    StringInfo *info = getInfo(ths);
    zint start;
    zint end;

    seqConvertSliceArgs(&start, &end, inclusive, info->s.size,
        startArg, endArg);

    if (start == -1) {
        return NULL;
    }

    zint size = end - start;

    if (size > 16) {
        // Share storage for large results.
        return makeIndirectString(ths, start, size);
    } else {
        zstring s = { size, &info->s.chars[start] };
        return stringFromZstring(s);
    }
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
zvalue stringFromUtf8(zint utfBytes, const char *utf) {
    zint decodedSize = utf8DecodeStringSize(utfBytes, utf);

    switch (decodedSize) {
        case 0: {
            return EMPTY_STRING;
        }
        case 1: {
            // Call into `stringFromChar` since that's what handles caching
            // of single-character strings.
            zchar ch;
            utf8DecodeCharsFromString(&ch, utfBytes, utf);
            return stringFromZchar(ch);
        }
    }

    zvalue result = allocString(decodedSize);

    utf8DecodeCharsFromString(getInfo(result)->content, utfBytes, utf);
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
    getInfo(result)->content[0] = value;

    if (value <= DAT_MAX_CACHED_CHAR) {
        CACHED_CHARS[value] = result;
        datImmortalize(result);
    }

    return result;
}

// Documented in header.
zvalue stringFromZstring(zstring string) {
    // Deal with special cases. This calls into `stringFromZchar` since that's
    // what handles caching of single-character strings.
    switch (string.size) {
        case 0: { return EMPTY_STRING;                     }
        case 1: { return stringFromZchar(string.chars[0]); }
    }

    zvalue result = allocString(string.size);

    utilCpy(zchar, getInfo(result)->content, string.chars, string.size);
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
    assertString(string);
    return utf8DupFromZstring(getInfo(string)->s);
}

// Documented in header.
zint utf8FromString(zint resultSize, char *result, zvalue string) {
    assertString(string);
    return utf8FromZstring(resultSize, result, getInfo(string)->s);
}

// Documented in header.
zint utf8SizeFromString(zvalue string) {
    assertString(string);
    return utf8SizeFromZstring(getInfo(string)->s);
}

// Documented in header.
zchar zcharFromString(zvalue string) {
    assertStringSize1(string);
    return getInfo(string)->s.chars[0];
}

// Documented in header.
zstring zstringFromString(zvalue string) {
    assertString(string);
    return getInfo(string)->s;
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_1(String, castFrom, value) {
    zvalue cls = get_class(value);

    if (valEq(cls, CLS_Int)) {
        zint n = zintFromInt(value);
        zchar result;

        if (!zcharFromZint(&result, n)) {
            die("Invalid int value for char: %lld", n);
        } else {
            return stringFromZchar(result);
        }
    } else if (valEq(cls, CLS_Symbol)) {
        return stringFromZstring(zstringFromSymbol(value));
    } else if (classAccepts(CLS_String, value)) {
        return value;
    }

    return NULL;
}

// Documented in header.
METH_IMPL_1(String, castToward, cls) {
    StringInfo *info = getInfo(ths);

    if (valEq(cls, CLS_Int)) {
        if (info->s.size == 1) {
            return intFromZint(zcharFromString(ths));
        }
    } else if (valEq(cls, CLS_Symbol)) {
        return symbolFromZstring(info->s);
    } else if (classAccepts(cls, ths)) {
        return ths;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_rest(String, cat, args) {
    if (argsSize == 0) {
        return ths;
    }

    zint thsSize = getInfo(ths)->s.size;
    zvalue strings[argsSize];
    StringInfo *infos[argsSize];

    zint size = thsSize;
    for (zint i = 0; i < argsSize; i++) {
        zvalue one = args[i];
        if (classAccepts(CLS_Symbol, one)) {
            one = cm_toString(one);
        } else {
            assertString(one);
        }
        strings[i] = one;
        infos[i] = getInfo(one);
        size += infos[i]->s.size;
    }

    zchar *chars = allocArray(size);
    zint at = thsSize;
    arrayFromZstring(chars, getInfo(ths)->s);
    for (zint i = 0; i < argsSize; i++) {
        zstring one = infos[i]->s;
        arrayFromZstring(&chars[at], one);
        at += one.size;
    }

    zstring s = { size, chars };
    zvalue result = stringFromZstring(s);
    freeArray(chars);
    return result;
}

// Documented in spec.
METH_IMPL_0_1(String, collect, function) {
    StringInfo *info = getInfo(ths);
    const zchar *chars = info->s.chars;
    zint size = info->s.size;
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = stringFromZchar(chars[i]);
        zvalue one = (function == NULL) ? elem : FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

// Documented in spec.
METH_IMPL_1(String, del, key) {
    StringInfo *info = getInfo(ths);
    const zchar *chars = info->s.chars;
    zint size = info->s.size;
    zint index = seqNthIndexLenient(key);

    if ((index < 0) || (index >= size)) {
        return ths;
    }

    zchar *resultChars = allocArray(size - 1);
    utilCpy(zchar, resultChars, chars, index);
    utilCpy(zchar, &resultChars[index], &chars[index + 1], (size - index - 1));

    zstring s = { size - 1, resultChars };
    zvalue result = stringFromZstring(s);
    freeArray(resultChars);
    return result;
}

// Documented in spec.
METH_IMPL_0(String, debugString) {
    zvalue quote = stringFromUtf8(1, "\"");
    return cm_cat(quote, ths, quote);
}

// Documented in spec.
METH_IMPL_0(String, fetch) {
    StringInfo *info = getInfo(ths);

    switch (info->s.size) {
        case 0: { return NULL; }
        case 1: { return ths;  }
        default: {
            die("Invalid to call `fetch` on string with size > 1.");
        }
    }
}

// Documented in header.
METH_IMPL_0(String, gcMark) {
    StringInfo *info = getInfo(ths);

    datMark(info->contentString);
    return NULL;
}

// Documented in spec.
METH_IMPL_0(String, get_size) {
    return intFromZint(getInfo(ths)->s.size);
}

// Documented in spec.
METH_IMPL_1(String, nextValue, box) {
    StringInfo *info = getInfo(ths);
    zint size = info->s.size;

    switch (size) {
        case 0: {
            // `string` is empty.
            return NULL;
        }
        case 1: {
            // `string` is a single character, so it can be yielded directly.
            cm_store(box, ths);
            return EMPTY_STRING;
        }
        default: {
            // The hard case. Make a single-character string for the yield.
            // Make an indirect string for the return value, to avoid the
            // churn of copying and re-re-...-copying the content.
            const zchar *chars = info->s.chars;
            cm_store(box, stringFromZchar(chars[0]));
            return makeIndirectString(ths, 1, size - 1);
        }
    }
}

// Documented in spec.
METH_IMPL_1(String, nth, n) {
    StringInfo *info = getInfo(ths);
    zint index = seqNthIndexStrict(info->s.size, n);

    if (index < 0) {
        return NULL;
    }

    return stringFromZchar(info->s.chars[index]);
}

// Documented in spec.
METH_IMPL_2(String, put, key, value) {
    assertStringSize1(value);

    StringInfo *info = getInfo(ths);
    zint size = info->s.size;
    zint index = seqPutIndexStrict(size, key);

    if (index == size) {
        // This is an append operation.
        return cm_cat(ths, value);
    }

    zchar *resultChars = allocArray(size);
    arrayFromZstring(resultChars, getInfo(ths)->s);
    resultChars[index] = zcharFromString(value);

    zstring s = { size, resultChars };
    zvalue result = stringFromZstring(s);
    freeArray(resultChars);
    return result;
}

// Documented in spec.
METH_IMPL_0(String, reverse) {
    StringInfo *info = getInfo(ths);
    zint size = info->s.size;
    const zchar *chars = info->s.chars;
    zchar *arr = allocArray(size);

    for (zint i = 0, j = size - 1; i < size; i++, j--) {
        arr[i] = chars[j];
    }

    zstring s = { size, arr };
    zvalue result = stringFromZstring(s);
    freeArray(arr);
    return result;
}

// Documented in spec.
METH_IMPL_1_2(String, sliceExclusive, start, end) {
    return doSlice(ths, false, start, end);
}

// Documented in spec.
METH_IMPL_1_2(String, sliceInclusive, start, end) {
    return doSlice(ths, true, start, end);
}

// Documented in spec.
METH_IMPL_0(String, toInt) {
    return intFromZint(zcharFromString(ths));
}

// Documented in spec.
METH_IMPL_0(String, toString) {
    return ths;
}

// Documented in spec.
METH_IMPL_1(String, totalEq, other) {
    assertString(other);  // Note: Not guaranteed to be a `String`.
    return uncheckedEq(ths, other) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(String, totalOrder, other) {
    assertString(other);  // Note: Not guaranteed to be a `String`.
    switch (uncheckedZorder(ths, other)) {
        case ZLESS: { return INT_NEG1; }
        case ZSAME: { return INT_0;    }
        case ZMORE: { return INT_1;    }
    }
}

// Documented in spec.
METH_IMPL_0(String, valueList) {
    StringInfo *info = getInfo(ths);
    zint size = info->s.size;
    const zchar *chars = info->s.chars;
    zvalue result[size];

    for (zint i = 0; i < size; i++) {
        result[i] = stringFromZchar(chars[i]);
    }

    return listFromArray(size, result);
}

/** Initializes the module. */
MOD_INIT(String) {
    MOD_USE(Sequence);

    CLS_String = makeCoreClass(SYM(String), CLS_Core,
        symbolTableFromArgs(
            CMETH_BIND(String, castFrom),
            NULL),
        symbolTableFromArgs(
            METH_BIND(String, cat),
            METH_BIND(String, castToward),
            METH_BIND(String, collect),
            METH_BIND(String, debugString),
            METH_BIND(String, del),
            METH_BIND(String, fetch),
            METH_BIND(String, gcMark),
            METH_BIND(String, get_size),
            METH_BIND(String, nextValue),
            METH_BIND(String, nth),
            METH_BIND(String, put),
            METH_BIND(String, reverse),
            METH_BIND(String, sliceExclusive),
            METH_BIND(String, sliceInclusive),
            METH_BIND(String, toInt),
            METH_BIND(String, toString),
            METH_BIND(String, totalEq),
            METH_BIND(String, totalOrder),
            METH_BIND(String, valueList),
            SYM(get),        FUN_Sequence_get,
            SYM(keyList),    FUN_Sequence_keyList,
            SYM(nthMapping), FUN_Sequence_nthMapping,
            NULL));

    EMPTY_STRING = allocString(0);
    datImmortalize(EMPTY_STRING);
}

// Documented in header.
zvalue CLS_String = NULL;

// Documented in header.
zvalue EMPTY_STRING = NULL;
