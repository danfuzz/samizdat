// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdlib.h>

#include "type/Box.h"
#include "type/Cmp.h"
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

    info->s = (zstring) {size, info->content};
    info->contentString = NULL;

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

    resultInfo->s = (zstring) {size, &info->s.chars[offset]};
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
        return stringFromZstring((zstring) {size, &info->s.chars[start]});
    }
}

/**
 * Comparison function to order `zint`s, passed to standard library sorting
 * functions.
 */
static int zintOrder(const void *ptr1, const void *ptr2) {
    zint v1 = *((const zint *) ptr1);
    zint v2 = *((const zint *) ptr2);
    return (v1 == v2) ? 0 : ((v1 < v2) ? -1 : 1);
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
    zvalue cls = classOf(value);

    if (cmpEq(cls, CLS_Int)) {
        zint n = zintFromInt(value);
        zchar result;

        if (!zcharFromZint(&result, n)) {
            return NULL;
        } else {
            return stringFromZchar(result);
        }
    } else if (cmpEq(cls, CLS_Symbol)) {
        return stringFromZstring(zstringFromSymbol(value));
    } else if (typeAccepts(thsClass, value)) {
        return value;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_1(String, castToward, cls) {
    StringInfo *info = getInfo(ths);

    if (cmpEq(cls, CLS_Int)) {
        if (info->s.size == 1) {
            return intFromZint(zcharFromString(ths));
        }
    } else if (cmpEq(cls, CLS_Symbol)) {
        return symbolFromZstring(info->s);
    } else if (typeAccepts(cls, ths)) {
        return ths;
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_rest(String, cat, args) {
    if (args.size == 0) {
        return ths;
    }

    zint thsSize = getInfo(ths)->s.size;
    zvalue strings[args.size];
    StringInfo *infos[args.size];

    zint size = thsSize;
    for (zint i = 0; i < args.size; i++) {
        zvalue one = args.elems[i];
        if (typeAccepts(CLS_Symbol, one)) {
            one = cm_castFrom(CLS_String, one);
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
    for (zint i = 0; i < args.size; i++) {
        zstring one = infos[i]->s;
        arrayFromZstring(&chars[at], one);
        at += one.size;
    }

    zvalue result = stringFromZstring((zstring) {size, chars});
    freeArray(chars);
    return result;
}

// Documented in spec.
METH_IMPL_0_opt(String, collect, function) {
    StringInfo *info = getInfo(ths);
    const zchar *chars = info->s.chars;
    zint size = info->s.size;
    zvalue *elems = utilAlloc(size * sizeof(zvalue));
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = stringFromZchar(chars[i]);
        zvalue one = (function == NULL) ? elem : FUN_CALL(function, elem);

        if (one != NULL) {
            elems[at] = one;
            at++;
        }
    }

    zvalue result = listFromZarray((zarray) {at, elems});
    utilFree(elems);
    return result;
}

// Documented in spec.
METH_IMPL_1(String, crossEq, other) {
    assertString(other);  // Note: Not guaranteed to be a `String`.
    return uncheckedEq(ths, other) ? ths : NULL;
}

// Documented in spec.
METH_IMPL_1(String, crossOrder, other) {
    assertString(other);  // Note: Not guaranteed to be a `String`.
    return symbolFromZorder(uncheckedZorder(ths, other));
}

// Documented in spec.
METH_IMPL_rest(String, del, ns) {
    StringInfo *info = getInfo(ths);
    zint size = info->s.size;

    if ((ns.size == 0) || (size == 0)) {
        // Easy outs: Not actually deleting anything, and/or starting out
        // with the empty string.
        return ths;
    }

    // Convert all the given `ns` to ints, leniently. Leave `-1` for any
    // argument that is invalid. Sort them.
    zint indexes[ns.size];
    for (zint i = 0; i < ns.size; i++) {
        indexes[i] = seqNthIndexLenient(ns.elems[i]);
        if (indexes[i] >= size) {
            indexes[i] = -1;
        }
    }
    qsort(indexes, ns.size, sizeof(zint), zintOrder);

    // Make a local copy of the characters, and compact out the selected
    // indexes.

    // Start `indexAt` at the first valid index.
    zint indexAt = 0;
    while ((indexAt < ns.size) && (indexes[indexAt] < 0)) {
        indexAt++;
    }

    if (indexAt == ns.size) {
        // None of `ns` were in `ths`.
        return ths;
    }

    zchar *chars = allocArray(size);
    arrayFromZstring(chars, info->s);

    zint at = 0;
    for (zint i = 0; i < size; i++) {
        if ((indexAt < ns.size) && (indexes[indexAt] == i)) {
            // The loop skips duplicates.
            do {
                indexAt++;
            } while ((indexAt < ns.size) && (indexes[indexAt] == i));
        } else {
            if (i != at) {
                chars[at] = chars[i];
            }
            at++;
        }
    }

    if (at == 0) {
        // All of the characters were removed.
        return EMPTY_STRING;
    }

    // Construct a new instance with the remaining characters.
    zvalue result = stringFromZstring((zstring) {at, chars});
    freeArray(chars);
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

// Documented in spec.
METH_IMPL_0_opt(String, forEach, function) {
    StringInfo *info = getInfo(ths);
    zstring s = info->s;
    zvalue result = NULL;

    if (function == NULL) {
        // Without a function, this method just returns the last element.
        return (s.size == 0) ? NULL : stringFromZchar(s.chars[s.size - 1]);
    }

    for (zint i = 0; i < s.size; i++) {
        zvalue v = FUN_CALL(function, stringFromZchar(s.chars[i]));
        if (v != NULL) {
            result = v;
        }
    }

    return result;
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
METH_IMPL_1(String, repeat, count) {
    StringInfo *thsInfo = getInfo(ths);
    zint n = zintFromInt(count);

    if (n < 0) {
        die("Invalid negative count for `repeat`.");
    } else if (n == 0) {
        return EMPTY_STRING;
    }

    zint thsSize = thsInfo->s.size;
    zint size = n * thsSize;
    zvalue result = allocString(size);
    StringInfo *info = getInfo(result);

    for (zint i = 0; i < n; i++) {
        utilCpy(zchar, &info->content[i * thsSize], thsInfo->s.chars, thsSize);
    }

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

    zvalue result = stringFromZstring((zstring) {size, arr});
    freeArray(arr);
    return result;
}

// Documented in spec.
METH_IMPL_1_opt(String, sliceExclusive, start, end) {
    return doSlice(ths, false, start, end);
}

// Documented in spec.
METH_IMPL_1_opt(String, sliceInclusive, start, end) {
    return doSlice(ths, true, start, end);
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

    return listFromZarray((zarray) {size, result});
}

/** Initializes the module. */
MOD_INIT(String) {
    MOD_USE(Sequence);

    CLS_String = makeCoreClass(SYM(String), CLS_Core,
        METH_TABLE(
            CMETH_BIND(String, castFrom)),
        METH_TABLE(
            METH_BIND(String, cat),
            METH_BIND(String, castToward),
            METH_BIND(String, collect),
            METH_BIND(String, crossEq),
            METH_BIND(String, crossOrder),
            METH_BIND(String, debugString),
            METH_BIND(String, del),
            METH_BIND(String, fetch),
            METH_BIND(String, forEach),
            METH_BIND(String, gcMark),
            METH_BIND(String, get_size),
            METH_BIND(String, nextValue),
            METH_BIND(String, nth),
            METH_BIND(String, repeat),
            METH_BIND(String, reverse),
            METH_BIND(String, sliceExclusive),
            METH_BIND(String, sliceInclusive),
            METH_BIND(String, valueList),
            SYM(get),          FUN_Sequence_get,
            SYM(keyList),      FUN_Sequence_keyList,
            SYM(reverseNth),   FUN_Sequence_reverseNth,
            SYM(sliceGeneral), FUN_Sequence_sliceGeneral));

    EMPTY_STRING = datImmortalize(allocString(0));
}

// Documented in header.
zvalue CLS_String = NULL;

// Documented in header.
zvalue EMPTY_STRING = NULL;
