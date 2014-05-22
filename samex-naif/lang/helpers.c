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
 * Adds a `{(name): Value}` binding to the given map.
 */
static zvalue addTypeBinding(zvalue map, zvalue name) {
    return collPut(map, name, TYPE_Value);
}

/**
 * Adds a type binding (per above) to a source binding for a given map. This
 * is used to build up metainformation about imports.
 */
static zvalue addImportBinding(zvalue map, zvalue source, zvalue name) {
    zvalue names = get(map, source);

    names = addTypeBinding((names == NULL) ? EMPTY_MAP : names, name);
    return collPut(map, source, names);
}

/**
 * Adds a format to a list of same in a resource source map. This
 * is used to build up metainformation about resources.
 */
static zvalue addResourceBinding(zvalue map, zvalue source, zvalue format) {
    zvalue formats = get(map, source);

    if (formats == NULL) {
        formats = EMPTY_LIST;
    }

    // Unlike the `Lang0Node` version, this one doesn't de-duplicate formats.
    formats = listAppend(formats, format);
    return collPut(map, source, formats);
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
zvalue get_baseName(zvalue source) {
    if (hasType(source, TYPE_external)) {
        zvalue path = dataOf(source);
        zvalue components = splitAtChar(dataOf(source), STR_CH_DOT);
        return nth(components, get_size(components) - 1);
    } else if (hasType(source, TYPE_internal)) {
        zvalue components = splitAtChar(dataOf(source), STR_CH_SLASH);
        zvalue last = nth(components, get_size(components) - 1);
        zvalue parts = splitAtChar(last, STR_CH_DOT);
        return nth(parts, 0);
    } else {
        die("Bad type for `get_baseName`.");
    }
}

/* Documented in spec. */
zvalue get_definedNames(zvalue node) {
    if (hasType(node, TYPE_export)) {
        return get_definedNames(get(node, STR_value));
    } else if (   hasType(node, TYPE_importModule)
               || hasType(node, TYPE_importResource)
               || hasType(node, TYPE_varDef)
               || hasType(node, TYPE_varDefMutable)) {
        return listFrom1(get(node, STR_name));
    } else if (hasType(node, TYPE_importModuleSelection)) {
        zvalue prefix = get(node, STR_prefix);
        zvalue select = get(node, STR_select);
        if (select == NULL) {
            die("Cannot call `get_definedNames` on unresolved import.");
        }

        zint size = get_size(select);
        zvalue arr[size];
        arrayFromList(arr, select);

        for (zint i = 0; i < size; i++) {
            arr[i] = GFN_CALL(cat, prefix, arr[i]);
        }

        return listFromArray(size, arr);
    } else {
        return EMPTY_LIST;
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
    zvalue select = get(node, STR_select);
    zvalue source = get(node, STR_source);

    if (hasType(node, TYPE_importModule)) {
        zvalue stat = makeVarDef(name,
            makeCall(REFS(loadModule), listFrom1(makeLiteral(source))));

        return listFrom1(stat);
    } else if (hasType(node, TYPE_importModuleSelection)) {
        zvalue names = get_definedNames(node);
        zint size = get_size(names);
        zvalue loadCall = makeCall(REFS(loadModule),
            listFrom1(makeLiteral(source)));

        zvalue stats[size];
        for (zint i = 0; i < size; i++) {
            zvalue name = nth(names, i);
            zvalue sel = nth(select, i);
            stats[i] = makeVarDef(name,
                makeCall(REFS(get), listFrom2(loadCall, makeLiteral(sel))));
        }

        return listFromArray(size, stats);
    } else if (hasType(node, TYPE_importResource)) {
        zvalue stat = makeVarDef(
            name,
            makeCall(REFS(loadResource),
                listFrom2(makeLiteral(source), makeLiteral(format))));

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
zvalue makeInfoMap(zvalue node) {
    zvalue statements = get(node, STR_statements);
    zint size = get_size(statements);

    zvalue exports = EMPTY_MAP;
    zvalue imports = EMPTY_MAP;
    zvalue resources = EMPTY_MAP;

    for (zint i = 0; i < size; i++) {
        zvalue s = nth(statements, i);

        if (hasType(s, TYPE_exportSelection)) {
            zvalue select = get(s, STR_select);
            zint sz = get_size(select);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(select, j);
                exports = addTypeBinding(exports, name);
            }
        } else if (hasType(s, TYPE_export)) {
            zvalue names = get_definedNames(s);
            zint sz = get_size(names);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(names, j);
                exports = addTypeBinding(exports, name);
            }
            // And fall through to handle an `import*` payload, if any.
            s = get(s, STR_value);
        }

        // *Not* `else if` (see above).
        if (hasType(s, TYPE_importModule)) {
            imports =
                addImportBinding(imports, get(s, STR_source), TYPE_module);
        } else if (hasType(s, TYPE_importModuleSelection)) {
            zvalue source = get(s, STR_source);
            zvalue select = get(s, STR_select);
            zint sz = get_size(select);
            if (sz == 0) {
                die("Cannot call `makeInfoMap` on unresolved import.");
            }
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(select, j);
                imports = addImportBinding(imports, source, name);
            }
        } else if (hasType(s, TYPE_importResource)) {
            resources = addResourceBinding(resources,
                get(s, STR_source), get(s, STR_format));
        }
    }

    return mapFrom3(
        STR_exports,   exports,
        STR_imports,   imports,
        STR_resources, resources);
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
zvalue resolveImport(zvalue node, zvalue resolveFn) {
    zvalue source = get(node, STR_source);
    zvalue resolved =
        (resolveFn == NULL) ? NULL : FUN_CALL(resolveFn, source);

    if (hasType(node, TYPE_importModule)) {
        // No conversion, just validation.
        return node;
    } else if (hasType(node, TYPE_importModuleSelection)) {
        if (get(node, STR_select) != NULL) {
            // No conversion, just validation.
            return node;
        }
        // When given a `NULL` resolver, this acts as if all sources resolve
        // to an empty export list. So if this is a selection import, it
        // won't actually end up binding anything.
        zvalue select = (resolved == NULL)
            ? EMPTY_LIST
            : get(get(resolved, STR_info), STR_exports);
        return makeValue(
            TYPE_importModuleSelection,
            collPut(dataOf(node), STR_select, select),
            NULL);
    } else if (hasType(node, TYPE_importResource)) {
        // No conversion, just validation.
        return node;
    } else {
        die("Bad node type for `resolveImport`.");
    }
}

/* Documented in spec. */
zvalue withFormals(zvalue node, zvalue formals) {
    return makeValue(
        get_type(node),
        collPut(dataOf(node), STR_formals, formals),
        NULL);
}

/* Documented in spec. */
zvalue withModuleDefs(zvalue node) {
    if (get(node, STR_yield) != NULL) {
        die("Invalid node for `withModuleDefs` (has `yield`).");
    }

    zvalue info = makeInfoMap(node);

    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);
    zvalue statements = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);

        if (hasType(s, TYPE_exportSelection)) {
            continue;
        } else if (hasType(s, TYPE_export)) {
            s = get(s, STR_value);
        }

        statements = listAppend(statements, s);
    }

    zvalue exportValues = EMPTY_LIST;
    zvalue exportInfo = get(info, STR_exports);
    zint exSize = get_size(exportInfo);
    zmapping mappings[exSize];
    arrayFromMap(mappings, exportInfo);
    for (zint i = 0; i < exSize; i++) {
        zvalue name = mappings[i].key;
        exportValues = listAppend(exportValues,
            makeCall(REFS(makeValueMap),
                listFrom2(makeLiteral(name), makeVarRef(name))));
    }

    zvalue yieldExports = (get_size(exportValues) == 0)
        ? makeLiteral(EMPTY_MAP)
        : makeCall(REFS(cat), exportValues);
    zvalue yieldInfo = makeLiteral(info);
    zvalue yield = makeCall(REFS(makeValue),
        listFrom2(
            makeLiteral(TYPE_module),
            makeCall(REFS(cat),
                listFrom2(
                    makeCall(REFS(makeValueMap),
                        listFrom2(makeLiteral(STR_exports), yieldExports)),
                    makeCall(REFS(makeValueMap),
                        listFrom2(makeLiteral(STR_info), yieldInfo))))));

    return makeValue(
        get_type(node),
        GFN_CALL(cat,
            dataOf(node),
            mapFrom2(
                STR_statements, statements,
                STR_yield,      yield)),
        NULL);
}

/* Documented in spec. */
zvalue withResolvedImports(zvalue node, zvalue resolveFn) {
    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);
    zvalue arr[size];
    arrayFromList(arr, rawStatements);

    for (zint i = 0; i < size; i++) {
        zvalue s = arr[i];
        bool exported = false;
        zvalue defNode = s;

        if (hasType(s, TYPE_export)) {
            exported = true;
            defNode = get(s, STR_value);
        }

        if (!(   hasType(defNode, TYPE_importModule)
              || hasType(defNode, TYPE_importModuleSelection)
              || hasType(defNode, TYPE_importResource))) {
            continue;
        }

        zvalue resolved = resolveImport(defNode, resolveFn);

        if (exported) {
            resolved = makeExport(resolved);
        }

        arr[i] = resolved;
    }

    zvalue converted = listFromArray(size, arr);

    return makeValue(
        get_type(node),
        GFN_CALL(cat,
            dataOf(node),
            mapFrom1(STR_statements, converted)),
        NULL);
}

/* Documented in spec. */
zvalue withTop(zvalue node) {
    // Contrary to the spec, we bind to `EMPTY_LIST` and not `true`, because
    // (a) the actual value doesn't matter, and (b) `true` isn't available
    // here in a straightforward way.
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

/* Documented in spec. */
zvalue withoutTops(zvalue node) {
    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);

    zvalue tops = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);
        zvalue defNode = hasType(s, TYPE_export)
            ? get(s, STR_value)
            : s;

        if (get(defNode, STR_top) != NULL) {
            tops = listAppend(tops,
                makeVarDef(get(defNode, STR_name), NULL));
        }
    }

    zvalue mains = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);
        zvalue defNode = hasType(s, TYPE_export)
            ? get(s, STR_value)
            : s;

        if (get(defNode, STR_top) != NULL) {
            mains = listAppend(mains,
                makeVarBind(get(defNode, STR_name), get(defNode, STR_value)));
        } else {
            mains = listAppend(mains, s);
        }
    }

    zvalue exports = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);

        if (!hasType(s, TYPE_export)) {
            continue;
        }

        zvalue defNode = get(s, STR_value);
        if (get(defNode, STR_top) == NULL) {
            continue;
        }

        exports = listAppend(exports, get(defNode, STR_name));
    };

    zvalue optSelection = (get_size(exports) == 0)
        ? EMPTY_LIST
        : listFrom1(makeExportSelection(exports));

    return makeValue(
        get_type(node),
        GFN_CALL(cat,
            dataOf(node),
            mapFrom1(
                STR_statements, GFN_CALL(cat, tops, mains, optSelection))),
        NULL);
}
