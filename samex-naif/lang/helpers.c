/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Node constructors and related helpers
 */

#include "const.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Type.h"
#include "util.h"

#include "helpers.h"


/*
 * Private functions
 */

/** Splits a string into a list, separating at the given character. */
static zvalue splitAtChar(zvalue string, zvalue chString) {
    zchar ch = zcharFromString(chString);
    zint size = get_size(string);
    zchar chars[size];
    zvalue result[size + 1];
    zint resultAt = 0;

    zcharsFromString(chars, string);

    for (zint at = 0; at <= size; /*at*/) {
        zint endAt = at;
        while ((endAt < size) && chars[endAt] != ch) {
            endAt++;
        }
        result[resultAt] = stringFromZchars(endAt - at, &chars[at]);
        resultAt++;
        at = endAt + 1;
    }

    return listFromArray(resultAt, result);
}

/**
 * Appends a name->variable binding map to the given list (of presumed
 * same).
 */
static zvalue appendNameBinding(zvalue list, zvalue name) {
    return listAppend(list,
        makeCall(makeVarRef(STR_makeValueMap),
            listFrom2(makeLiteral(name), makeVarRef(name))));
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue mapFrom1(zvalue k1, zvalue v1) {
    return mapFrom4(k1, v1, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* Documented in header. */
zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return mapFrom4(k1, v1, k2, v2, NULL, NULL, NULL, NULL);
}

/* Documented in header. */
zvalue mapFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3) {
    return mapFrom4(k1, v1, k2, v2, k3, v3, NULL, NULL);
}

/* Documented in header. */
zvalue mapFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4) {
    zmapping elems[4];
    zint at = 0;

    if (v1 != NULL) { elems[at].key = k1; elems[at].value = v1; at++; }
    if (v2 != NULL) { elems[at].key = k2; elems[at].value = v2; at++; }
    if (v3 != NULL) { elems[at].key = k3; elems[at].value = v3; at++; }
    if (v4 != NULL) { elems[at].key = k4; elems[at].value = v4; at++; }

    return (at == 0) ? EMPTY_MAP : mapFromArray(at, elems);
}

/* Documented in header. */
zvalue listFrom1(zvalue e1) {
    return listFromArray(1, &e1);
}

/* Documented in header. */
zvalue listFrom2(zvalue e1, zvalue e2) {
    zvalue elems[2] = { e1, e2 };
    return listFromArray(2, elems);
}

/* Documented in header. */
zvalue listFrom3(zvalue e1, zvalue e2, zvalue e3) {
    zvalue elems[3] = { e1, e2, e3 };
    return listFromArray(3, elems);
}

/* Documented in header. */
zvalue listAppend(zvalue list, zvalue elem) {
    return GFN_CALL(cat, list, listFrom1(elem));
}

/* Documented in spec. */
zvalue formalsMaxArgs(zvalue formals) {
    zint maxArgs = 0;
    zint sz = get_size(formals);

    for (zint i = 0; i < sz; i++) {
        zvalue one = nth(formals, i);
        zvalue repeat = get(one, STR_repeat);
        if (valEqNullOk(repeat, STR_CH_STAR)
            || valEqNullOk(repeat, STR_CH_PLUS)) {
            maxArgs = -1;
            break;
        }
        maxArgs++;
    }

    return intFromZint(maxArgs);
}

/* Documented in spec. */
zvalue formalsMinArgs(zvalue formals) {
    zint minArgs = 0;
    zint sz = get_size(formals);

    for (zint i = 0; i < sz; i++) {
        zvalue one = nth(formals, i);
        zvalue repeat = get(one, STR_repeat);
        if (!(valEqNullOk(repeat, STR_CH_QMARK)
              || valEqNullOk(repeat, STR_CH_STAR))) {
            minArgs++;
        }
    }

    return intFromZint(minArgs);
}

/* Documented in spec. */
zvalue get_baseName(zvalue thisPath) {
    if (hasType(thisPath, TYPE_external)) {
        zvalue path = dataOf(thisPath);
        zvalue components = splitAtChar(dataOf(thisPath), STR_CH_DOT);
        return nth(components, get_size(components) - 1);
    } else if (hasType(thisPath, TYPE_internal)) {
        zvalue components = splitAtChar(dataOf(thisPath), STR_CH_SLASH);
        zvalue last = nth(components, get_size(components) - 1);
        zvalue parts = splitAtChar(last, STR_CH_DOT);
        return nth(parts, 0);
    } else {
        die("Bad type for `get_baseName`.");
    }
}

/* Documented in spec. */
zvalue makeApply(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_function, function, STR_actuals, actuals);
    return makeValue(TYPE_apply, value, NULL);
}

/* Documented in spec. */
zvalue makeCall(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_function, function, STR_actuals, actuals);
    return makeValue(TYPE_call, value, NULL);
}

/* Documented in spec. */
zvalue makeCallOrApply(zvalue function, zvalue actuals) {
    // This is a fairly direct (but not exact) transliteration
    // of the corresponding code in `Lang0Node`.

    zint sz = (actuals == NULL) ? 0 : get_size(actuals);
    zvalue pending[sz];
    zvalue cookedActuals[sz];
    zint pendAt = 0;
    zint cookAt = 0;

    #define addToCooked(actual) do { \
        cookedActuals[cookAt] = (actual); \
        cookAt++; \
    } while (0)

    #define addPendingToCooked() do { \
        if (pendAt != 0) { \
            addToCooked(makeCall(REFS(makeList), \
                listFromArray(pendAt, pending))); \
            pendAt = 0; \
        } \
    } while (0)

    for (zint i = 0; i < sz; i++) {
        zvalue one = nth(actuals, i);
        zvalue node = get(one, STR_interpolate);
        if (node != NULL) {
            addPendingToCooked();
            addToCooked(makeCall(REFS(collect), listFrom1(node)));
        } else {
            pending[pendAt] = one;
            pendAt++;
        }
    }

    if (cookAt == 0) {
        // There were no interpolated arguments.
        return makeCall(function, actuals);
    }

    addPendingToCooked();

    if (cookAt > 1) {
        return makeApply(function,
            makeCall(REFS(cat), listFromArray(cookAt, cookedActuals)));
    } else {
        return makeApply(function, cookedActuals[0]);
    }

    #undef addToCooked
    #undef addPendingToCooked
}

/* Documented in spec. */
zvalue makeDynamicImport(zvalue node) {
    zvalue format = get(node, STR_format);
    zvalue name = get(node, STR_name);
    zvalue source = get(node, STR_source);

    if (hasType(node, TYPE_importModule)) {
        zvalue loadRef = hasType(source, TYPE_external)
            ? REFS(moduleLoad)
            : REFS(intraLoad);
        zvalue stat = makeVarDef(
            name,
            makeCall(loadRef, listFrom1(makeLiteral(dataOf(source)))));

        return listFrom1(stat);
    } else if (hasType(node, TYPE_importModuleSelection)) {
        zvalue selection = resolveSelection(node);
        zint size = get_size(selection);
        zmapping mappings[size];
        arrayFromMap(mappings, selection);

        zvalue loadRef = hasType(source, TYPE_external)
            ? REFS(moduleLoad)
            : REFS(intraLoad);
        zvalue loadCall = makeCall(loadRef,
            listFrom1(makeLiteral(dataOf(source))));

        zvalue stats[size];
        for (zint i = 0; i < size; i++) {
            zvalue targetName = mappings[i].key;
            zvalue sourceName = mappings[i].value;
            stats[i] = makeVarDef(
                targetName,
                makeCall(REFS(get),
                    listFrom2(loadCall, makeLiteral(sourceName))));
        }

        return listFromArray(size, stats);
    } else if (hasType(node, TYPE_importResource)) {
        if (hasType(source, TYPE_external)) {
            die("Cannot import external resource.");
        }

        zvalue stat = makeVarDef(
            name,
            makeCall(REFS(intraRead),
                listFrom2(
                    makeLiteral(dataOf(source)),
                    makeLiteral(format))));

        return listFrom1(stat);
    } else {
        die("Bad node type for makeDynamicImport");
    }
}

/* Documented in spec. */
zvalue makeExport(zvalue node) {
    return makeValue(TYPE_export,
        mapFrom1(STR_value, node),
        NULL);
}

/* Documented in spec. */
zvalue makeExportSelection(zvalue names) {
    return makeValue(TYPE_exportSelection,
        mapFrom1(STR_select, names),
        NULL);
}

/* Documented in spec. */
zvalue makeImport(zvalue baseData) {
    // Note: This is a near-transliteration of the equivalent code in
    // `Lang0Node`.
    zvalue data = baseData;  // Modified in some cases below.

    zvalue select = get(data, STR_select);
    if (select != NULL) {
        // It's a module binding selection.

        if (get(data, STR_name) != NULL) {
            die("Import selection name must be a prefix.");
        } else if (get(data, STR_format) != NULL) {
            die("Cannot import selection of resource.");
        }

        if (get(data, STR_prefix) == NULL) {
            // Default to empty string for `prefix`.
            data = collPut(data, STR_prefix, EMPTY_STRING);
        }

        if (hasType(select, TYPE_CH_STAR)) {
            // It's a wildcard import.
            data = collDel(data, STR_select);
        }

        return makeValue(TYPE_importModuleSelection, data, NULL);
    }

    if (get(data, STR_name) == NULL) {
        // No `name` provided, so figure out a default one.
        zvalue name = GFN_CALL(cat,
            STR_CH_DOLLAR,
            get_baseName(get(baseData, STR_source)));
        data = collPut(data, STR_name, name);
    }

    if (get(data, STR_format) != NULL) {
        // It's a resource.
        if (hasType(get(data, STR_source), TYPE_external)) {
            die("Cannot import external resource.");
        }
        return makeValue(TYPE_importResource, data, NULL);
    }

    // It's a whole-module import.
    return makeValue(TYPE_importModule, data, NULL);
}

/* Documented in spec. */
zvalue makeInterpolate(zvalue node) {
    return makeValue(TYPE_call,
        mapFrom3(
            STR_function,    REFS(interpolate),
            STR_actuals,     listFrom1(node),
            STR_interpolate, node),
        NULL);
}

/* Documented in spec. */
zvalue makeJumpNode(zvalue function, zvalue optValue) {
    return makeValue(TYPE_jump,
        mapFrom2(STR_function, function, STR_value, optValue),
        NULL);
}

/* Documented in spec. */
zvalue makeLiteral(zvalue value) {
    return makeValue(TYPE_literal, mapFrom1(STR_value, value), NULL);
}

/* Documented in spec. */
zvalue makeThunk(zvalue expression) {
    return makeValue(TYPE_closure,
        mapFrom3(
            STR_formals,    EMPTY_LIST,
            STR_statements, EMPTY_LIST,
            STR_yield,      expression),
        NULL);
}

/* Documented in spec. */
zvalue makeVarBind(zvalue name, zvalue value) {
    return makeValue(TYPE_varBind,
        mapFrom2(STR_name, name, STR_value, value),
        NULL);
}

/* Documented in spec. */
zvalue makeVarDef(zvalue name, zvalue value) {
    return makeValue(TYPE_varDef,
        mapFrom2(STR_name, name, STR_value, value),
        NULL);
}

/* Documented in spec. */
zvalue makeVarDefMutable(zvalue name, zvalue value) {
    return makeValue(TYPE_varDefMutable,
        mapFrom2(STR_name, name, STR_value, value),
        NULL);
}

/* Documented in spec. */
zvalue makeVarRef(zvalue name) {
    return makeValue(TYPE_varRef, mapFrom1(STR_name, name), NULL);
}

/* Documented in spec. */
zvalue makeOptValue(zvalue expression) {
    return makeCall(REFS(optValue), listFrom1(makeThunk(expression)));
}

/* Documented in spec. */
zvalue resolveSelection(zvalue node) {
    zvalue prefix = get(node, STR_prefix);
    zvalue select = get(node, STR_select);

    if (select == NULL) {
        // See TODO in Lang0Node implementation.
        die("TODO: wildcard selection import");
    }

    zint size = get_size(select);
    zmapping bindings[size];

    for (zint i = 0; i < size; i++) {
        zvalue name = nth(select, i);
        bindings[i].key = GFN_CALL(cat, prefix, name);
        bindings[i].value = name;
    }

    return mapFromArray(size, bindings);
}

/* Documented in spec. */
zvalue withFormals(zvalue node, zvalue formals) {
    return makeValue(
        get_type(node),
        collPut(dataOf(node), STR_formals, formals),
        NULL);
}

/* Documented in spec. */
zvalue withSimpleDefs(zvalue node) {
    // This implementation isn't as close a transliteration as other functions
    // in this file, but the end result should be the same.

    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);
    zvalue tops = EMPTY_LIST;
    zvalue mains = EMPTY_LIST;
    zvalue exports = EMPTY_LIST;

    for (zint i = 0; i < size; i++) {
        zvalue one = nth(rawStatements, i);

        if (hasType(one, TYPE_exportSelection)) {
            zvalue select = get(one, STR_select);
            zint selectSize = get_size(select);
            for (zint j = 0; j < selectSize; j++) {
                zvalue name = nth(select, j);
                exports = appendNameBinding(exports, name);
            }
            continue;
        }

        if (hasType(one, TYPE_export)) {
            one = get(one, STR_value);  // Re-set `one` to the inner statement.

            zvalue name = get(one, STR_name);

            if (name != NULL) {
                exports = appendNameBinding(exports, name);
            } else if (hasType(one, TYPE_importModuleSelection)) {
                zvalue selection = resolveSelection(one);
                zint size = get_size(selection);
                zmapping mappings[size];
                arrayFromMap(mappings, selection);

                for (zint i = 0; i < size; i++) {
                    zvalue name = mappings[i].key;
                    exports = appendNameBinding(exports, name);
                }
            } else {
                die("Bad `export` payload.");
            }

            // And fall through, to handle `top` and emit inner statement.
        }

        if (hasType(one, TYPE_varDef) && (get(one, STR_top) != NULL)) {
            zvalue name = get(one, STR_name);
            zvalue value = get(one, STR_value);
            tops = listAppend(tops, makeVarDef(name, NULL));
            mains = listAppend(mains, makeVarBind(name, value));
        } else {
            mains = listAppend(mains, one);
        }
    }

    zvalue yield = get(node, STR_yield);
    if (get_size(exports) != 0) {
        if (yield != NULL) {
            die("Cannot mix `export` and `yield`.");
        }
        yield = makeCall(makeVarRef(STR_cat), exports);
    }

    return makeValue(
        get_type(node),
        GFN_CALL(cat,
            dataOf(node),
            mapFrom2(
                STR_statements, GFN_CALL(cat, tops, mains),
                STR_yield,      yield)),
        NULL);
}

/* Documented in spec. */
zvalue withTop(zvalue node) {
    // Contrary to the spec, we bind to `EMPTY_LIST` and not `true`, because
    // (a) the actual value doesn't matter, and (b) `true` isn't available
    // in a straightforward way.
    return makeValue(
        get_type(node),
        collPut(dataOf(node), STR_top, EMPTY_LIST),
        NULL);
}

/* Documented in spec. */
zvalue withoutInterpolate(zvalue node) {
    return makeValue(
        get_type(node),
        collDel(dataOf(node), STR_interpolate),
        NULL);
}
