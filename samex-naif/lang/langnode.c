// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "const.h"
#include "type/Bool.h"
#include "type/Class.h"
#include "type/DerivedData.h"
#include "type/Function.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/OneOff.h"
#include "type/Selector.h"
#include "type/String.h"
#include "util.h"

#include "helpers.h"
#include "langnode.h"
#include "impl.h"


//
// Private functions
//

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
    return collPut(map, name, CLS_Value);
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

    // Unlike the `LangNode` version, this one doesn't de-duplicate formats.
    formats = listAppend(formats, format);
    return collPut(map, source, formats);
}

// Documented in `LangNode` source.
static zvalue expandYield(zvalue map) {
    zvalue yieldNode = get(map, STR_yield);

    if (     (yieldNode == NULL)
          || !hasClass(yieldNode, CLS_nonlocalExit)) {
        return yieldNode;
    }

    zvalue function = get(yieldNode, STR_function);
    zvalue value = get(yieldNode, STR_value);
    zvalue yieldDef = get(map, STR_yieldDef);
    zvalue functionTarget = get(function, STR_target);

    if (     hasClass(function, CLS_fetch)
          && hasClass(functionTarget, CLS_varRef)
          && (yieldDef != NULL)
          && valEq(get(functionTarget, STR_name), yieldDef)) {
        return value;
    }

    zvalue exitCall;

    if (hasClass(value, CLS_void)) {
        exitCall = makeCall(function, NULL);
    } else if (hasClass(value, CLS_maybe)) {
        zvalue arg = makeInterpolate(makeMaybeValue(get(value, STR_value)));
        exitCall = makeCallOrApply(function, listFrom1(arg));
    } else {
        exitCall = makeCall(function, listFrom1(value));
    }

    return makeNoYield(exitCall);
};


//
// Module functions
//

// Documented in spec.
bool canYieldVoid(zvalue node) {
    switch (get_evalType(node)) {
        case EVAL_apply:    return true;
        case EVAL_call:     return true;
        case EVAL_fetch:    return true;
        case EVAL_maybe:    return true;
        case EVAL_store:    return true;
        case EVAL_void:     return true;
        default: {
            return false;
        }
    }
}

// Documented in spec.
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

// Documented in spec.
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

// Documented in spec.
zvalue get_baseName(zvalue source) {
    if (hasClass(source, CLS_external)) {
        zvalue components = splitAtChar(get(source, STR_name), STR_CH_DOT);
        return nth(components, get_size(components) - 1);
    } else if (hasClass(source, CLS_internal)) {
        zvalue components = splitAtChar(get(source, STR_name), STR_CH_SLASH);
        zvalue last = nth(components, get_size(components) - 1);
        zvalue parts = splitAtChar(last, STR_CH_DOT);
        return nth(parts, 0);
    } else {
        die("Bad type for `get_baseName`.");
    }
}

// Documented in spec.
zvalue get_definedNames(zvalue node) {
    if (hasClass(node, CLS_export)) {
        return get_definedNames(get(node, STR_value));
    } else if (   hasClass(node, CLS_importModule)
               || hasClass(node, CLS_importResource)
               || hasClass(node, CLS_varDef)
               || hasClass(node, CLS_varDefMutable)) {
        return listFrom1(get(node, STR_name));
    } else if (hasClass(node, CLS_importModuleSelection)) {
        zvalue prefix = get(node, STR_prefix);
        zvalue select = get(node, STR_select);
        if (select == NULL) {
            die("Cannot call `get_definedNames` on unresolved import.");
        }

        zint size = get_size(select);
        zvalue arr[size];
        arrayFromList(arr, select);

        for (zint i = 0; i < size; i++) {
            arr[i] = METH_CALL(cat, prefix, arr[i]);
        }

        return listFromArray(size, arr);
    } else {
        return EMPTY_LIST;
    }
}

// Documented in spec.
bool isExpression(zvalue node) {
    switch (get_evalType(node)) {
        case EVAL_apply:    return true;
        case EVAL_call:     return true;
        case EVAL_closure:  return true;
        case EVAL_fetch:    return true;
        case EVAL_literal:  return true;
        case EVAL_noYield:  return true;
        case EVAL_store:    return true;
        case EVAL_varRef:   return true;
        default: {
            return false;
        }
    }
}

// Documented in spec.
zvalue makeApply(zvalue function, zvalue values) {
    if (values == NULL) {
        values = TOK_void;
    }

    zvalue value = mapFrom2(STR_function, function, STR_values, values);
    return makeData(CLS_apply, value);
}

// Documented in spec.
zvalue makeAssignmentIfPossible(zvalue target, zvalue value) {
    // This code isn't parallel to the in-language code but has the same
    // effect. The difference stems from the fact that C isn't a great direct
    // host for closures, whereas in-language `lvalue` is very naturally a
    // closure. In this case, the mere presence of `lvalue` is taken as the
    // significant thing, and its value is ignored; instead, per-type
    // assignment conversion is implemented directly below.

    if (get(target, STR_lvalue) == NULL) {
        return NULL;
    } else if (hasClass(target, CLS_fetch)) {
        zvalue innerTarget = get(target, STR_target);
        return makeData(CLS_store,
            mapFrom2(STR_target, innerTarget, STR_value, value));
    } else {
        die("Improper `lvalue` binding.");
    }
}

// Documented in spec.
zvalue makeBasicClosure(zvalue map) {
    return makeData(CLS_closure,
        METH_CALL(cat,
            mapFrom2(STR_formals, EMPTY_LIST, STR_statements, EMPTY_LIST),
            map));
}

// Documented in spec.
zvalue makeCall(zvalue function, zvalue values) {
    if (values == NULL) {
        values = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_function, function, STR_values, values);
    return makeData(CLS_call, value);
}

// Documented in spec.
zvalue makeCallOrApply(zvalue function, zvalue values) {
    // This is a fairly direct (but not exact) transliteration
    // of the corresponding code in `LangNode`.

    zint sz = (values == NULL) ? 0 : get_size(values);
    zvalue pending[sz];
    zvalue cookedValues[sz];
    zint pendAt = 0;
    zint cookAt = 0;

    if (sz == 0) {
        return makeApply(function, NULL);
    }

    #define addToCooked(actual) do { \
        cookedValues[cookAt] = (actual); \
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
        zvalue one = nth(values, i);
        zvalue node = get(one, STR_interpolate);
        if (node != NULL) {
            addPendingToCooked();
            addToCooked(makeCall(makeSelector(STR_collect), listFrom1(node)));
        } else {
            pending[pendAt] = one;
            pendAt++;
        }
    }

    if (cookAt == 0) {
        // There were no interpolated arguments.
        return makeCall(function, values);
    }

    addPendingToCooked();

    if (cookAt > 1) {
        return makeApply(function,
            makeCall(REFS(cat), listFromArray(cookAt, cookedValues)));
    } else {
        return makeApply(function, cookedValues[0]);
    }

    #undef addToCooked
    #undef addPendingToCooked
}

// Documented in spec.
zvalue makeDynamicImport(zvalue node) {
    zvalue format = get(node, STR_format);
    zvalue name = get(node, STR_name);
    zvalue select = get(node, STR_select);
    zvalue source = get(node, STR_source);

    if (hasClass(node, CLS_importModule)) {
        zvalue stat = makeVarDef(name,
            makeCall(REFS(loadModule), listFrom1(makeLiteral(source))));

        return listFrom1(stat);
    } else if (hasClass(node, CLS_importModuleSelection)) {
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
    } else if (hasClass(node, CLS_importResource)) {
        zvalue stat = makeVarDef(
            name,
            makeCall(REFS(loadResource),
                listFrom2(makeLiteral(source), makeLiteral(format))));

        return listFrom1(stat);
    } else {
        die("Bad node type for makeDynamicImport");
    }
}

// Documented in spec.
zvalue makeExport(zvalue node) {
    return makeData(CLS_export, mapFrom1(STR_value, node));
}

// Documented in spec.
zvalue makeExportSelection(zvalue names) {
    return makeData(CLS_exportSelection, mapFrom1(STR_select, names));
}

// Documented in spec.
zvalue makeFullClosure(zvalue nodeOrMap) {
    zvalue map = hasClass(nodeOrMap, CLS_Map) ? nodeOrMap : dataOf(nodeOrMap);
    zvalue formals = get(map, STR_formals);
    zvalue statements = get(map, STR_statements);
    zint statSz = (statements == NULL) ? 0 : get_size(statements);
    zvalue yieldNode = expandYield(map);

    if (formals == NULL) {
        formals = EMPTY_LIST;
    }

    if (statements == NULL) {
        statements = EMPTY_LIST;
    }

    if (     (yieldNode == NULL)
          && (statSz != 0)
          && (get(map, STR_yieldDef) == NULL)) {
        zvalue lastStat = nth(statements, statSz - 1);
        if (isExpression(lastStat)) {
            statements = METH_CALL(sliceExclusive, statements, intFromZint(0));
            yieldNode = canYieldVoid(lastStat)
                ? makeMaybe(lastStat)
                : lastStat;
        }
    }

    if (yieldNode == NULL) {
        yieldNode = TOK_void;
    }

    return makeData(CLS_closure,
        METH_CALL(cat,
            map,
            mapFrom3(
                STR_formals,    formals,
                STR_statements, statements,
                STR_yield,      yieldNode)));
}

// Documented in spec.
zvalue makeImport(zvalue baseData) {
    // Note: This is a near-transliteration of the equivalent code in
    // `LangNode`.
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

        if (hasClass(select, CLS_CH_STAR)) {
            // It's a wildcard import.
            data = collDel(data, STR_select);
        }

        return makeData(CLS_importModuleSelection, data);
    }

    if (get(data, STR_name) == NULL) {
        // No `name` provided, so figure out a default one.
        zvalue name = METH_CALL(cat,
            STR_CH_DOLLAR,
            get_baseName(get(baseData, STR_source)));
        data = collPut(data, STR_name, name);
    }

    if (get(data, STR_format) != NULL) {
        // It's a resource.
        if (hasClass(get(data, STR_source), CLS_external)) {
            die("Cannot import external resource.");
        }
        return makeData(CLS_importResource, data);
    }

    // It's a whole-module import.
    return makeData(CLS_importModule, data);
}

// Documented in spec.
zvalue makeInfoMap(zvalue node) {
    zvalue info = get(node, STR_info);
    if (info != NULL) {
        return info;
    }

    zvalue statements = get(node, STR_statements);
    zint size = get_size(statements);

    zvalue exports = EMPTY_MAP;
    zvalue imports = EMPTY_MAP;
    zvalue resources = EMPTY_MAP;

    for (zint i = 0; i < size; i++) {
        zvalue s = nth(statements, i);

        if (hasClass(s, CLS_exportSelection)) {
            zvalue select = get(s, STR_select);
            zint sz = get_size(select);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(select, j);
                exports = addTypeBinding(exports, name);
            }
        } else if (hasClass(s, CLS_export)) {
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
        if (hasClass(s, CLS_importModule)) {
            imports =
                addImportBinding(imports, get(s, STR_source), CLS_module);
        } else if (hasClass(s, CLS_importModuleSelection)) {
            zvalue source = get(s, STR_source);
            zvalue select = get(s, STR_select);
            if (select == NULL) {
                die("Cannot call `makeInfoMap` on unresolved import.");
            }
            zint sz = get_size(select);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(select, j);
                imports = addImportBinding(imports, source, name);
            }
        } else if (hasClass(s, CLS_importResource)) {
            resources = addResourceBinding(resources,
                get(s, STR_source), get(s, STR_format));
        }
    }

    return mapFrom3(
        STR_exports,   exports,
        STR_imports,   imports,
        STR_resources, resources);
}

// Documented in spec.
zvalue makeInterpolate(zvalue node) {
    return makeData(CLS_fetch,
        mapFrom3(
            STR_target,      node,
            STR_interpolate, node,
            STR_lvalue,      EMPTY_LIST));
}

// Documented in spec.
zvalue makeLiteral(zvalue value) {
    return makeData(CLS_literal, mapFrom1(STR_value, value));
}

// Documented in spec.
zvalue makeMaybe(zvalue value) {
    return makeData(CLS_maybe, mapFrom1(STR_value, value));
}

// Documented in spec.
zvalue makeMaybeValue(zvalue expression) {
    return makeCall(REFS(maybeValue), listFrom1(makeThunk(expression)));
}

// Documented in spec.
zvalue makeNoYield(zvalue value) {
    return makeData(CLS_noYield, mapFrom1(STR_value, value));
}

// Documented in spec.
zvalue makeNonlocalExit(zvalue function, zvalue optValue) {
    zvalue value = (optValue == NULL) ? TOK_void : optValue;
    return makeData(CLS_nonlocalExit,
        mapFrom2(STR_function, function, STR_value, value));
}

// Documented in spec.
zvalue makeSelector(zvalue name) {
    return makeLiteral(selectorFromName(name));
}

// Documented in spec.
zvalue makeThunk(zvalue expression) {
    zvalue yieldNode = isExpression(expression)
        ? makeMaybe(expression)
        : expression;

    return makeFullClosure(mapFrom1(STR_yield, yieldNode));
}

// Documented in spec.
zvalue makeVarRef(zvalue name) {
    return makeData(CLS_varRef, mapFrom1(STR_name, name));
}

// Documented in spec.
zvalue makeVarDef(zvalue name, zvalue value) {
    return makeData(CLS_varDef,
        mapFrom2(STR_name, name, STR_value, value));
}

// Documented in spec.
zvalue makeVarDefMutable(zvalue name, zvalue value) {
    return makeData(CLS_varDefMutable,
        mapFrom2(STR_name, name, STR_value, value));
}

// Documented in spec.
zvalue makeVarFetch(zvalue name) {
    return makeData(CLS_fetch, mapFrom1(STR_target, makeVarRef(name)));
}

// Documented in spec.
zvalue makeVarFetchLvalue(zvalue name) {
    // See discussion in `makeAssignmentIfPossible` above, for details about
    // `lvalue`.
    return makeData(CLS_fetch,
        mapFrom2(STR_target, makeVarRef(name), STR_lvalue, EMPTY_LIST));
}

// Documented in spec.
zvalue makeVarStore(zvalue name, zvalue value) {
    return makeData(CLS_store,
        mapFrom2(STR_target, makeVarRef(name), STR_value, value));
}

// Documented in spec.
zvalue resolveImport(zvalue node, zvalue resolveFn) {
    if (hasClass(node, CLS_importResource)) {
        // No conversion, just validation. TODO: Validate.
        //
        // **Note:** This clause is at the top so as to avoid the call to
        // `resolveFn()` below, which is inappropriate to do on resources.
        return node;
    }

    zvalue resolved = EMPTY_MAP;
    if (resolveFn != NULL) {
        zvalue source = get(node, STR_source);
        resolved = FUN_CALL(resolveFn, source);
        if (resolved == NULL) {
            die("Could not resolve import.");
        } else if (!hasClass(resolved, CLS_module)) {
            die("Invalid resolution result (not a `@module`)");
        }
    }

    if (hasClass(node, CLS_importModule)) {
        // No conversion, just validation (done above).
        return node;
    } else if (hasClass(node, CLS_importModuleSelection)) {
        // Get the exports. When given a `NULL` `resolveFn`, this acts as if
        // all sources resolve to an empty export map, and hence this node
        // won't bind anything.
        zvalue exports = EMPTY_LIST;
        zvalue info = get(resolved, STR_info);
        if (info != NULL) {
            exports = get(info, STR_exports);
        }

        zvalue select = get(node, STR_select);
        if (select != NULL) {
            // The selection is specified. So no modification needs to be
            // done to the node, just validation, including of the import in
            // general (above) and the particular selection (here).
            zint size = get_size(select);
            for (zint i = 0; i < size; i++) {
                zvalue one = nth(select, i);
                if (get(exports, one) == NULL) {
                    die("Could not resolve import selection.");
                }
            }
            return node;
        } else {
            // It's a wildcard select.
            select = METH_CALL(keyList, exports);
            return makeData(
                CLS_importModuleSelection,
                collPut(dataOf(node), STR_select, select));
        }
    } else {
        die("Bad node type for `resolveImport`.");
    }
}

// Documented in spec.
zvalue withFormals(zvalue node, zvalue formals) {
    return makeData(
        get_class(node),
        collPut(dataOf(node), STR_formals, formals));
}

// Documented in spec.
zvalue withModuleDefs(zvalue node) {
    if (!valEqNullOk(get(node, STR_yield), TOK_void)) {
        die("Invalid node for `withModuleDefs` (has non-void `yield`).");
    }

    zvalue info = makeInfoMap(node);

    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);
    zvalue statements = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);

        if (hasClass(s, CLS_exportSelection)) {
            continue;
        } else if (hasClass(s, CLS_export)) {
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
                listFrom2(makeLiteral(name), makeVarFetch(name))));
    }

    zvalue yieldExports = (exSize == 0)
        ? makeLiteral(EMPTY_MAP)
        : makeCall(REFS(cat), exportValues);
    zvalue yieldInfo = makeLiteral(info);
    zvalue yieldNode = makeCall(REFS(makeData),
        listFrom2(
            makeLiteral(CLS_module),
            makeCall(REFS(cat),
                listFrom2(
                    makeCall(REFS(makeValueMap),
                        listFrom2(makeLiteral(STR_exports), yieldExports)),
                    makeCall(REFS(makeValueMap),
                        listFrom2(makeLiteral(STR_info), yieldInfo))))));

    return makeData(
        get_class(node),
        METH_CALL(cat,
            dataOf(node),
            mapFrom3(
                STR_info,       info,
                STR_statements, statements,
                STR_yield,      yieldNode)));
}

// Documented in spec.
zvalue withName(zvalue node, zvalue name) {
    return makeData(
        get_class(node),
        collPut(dataOf(node), STR_name, name));
}

// Documented in spec.
zvalue withResolvedImports(zvalue node, zvalue resolveFn) {
    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);
    zvalue arr[size];
    arrayFromList(arr, rawStatements);

    for (zint i = 0; i < size; i++) {
        zvalue s = arr[i];
        bool exported = false;
        zvalue defNode = s;

        if (hasClass(s, CLS_export)) {
            exported = true;
            defNode = get(s, STR_value);
        }

        if (!(   hasClass(defNode, CLS_importModule)
              || hasClass(defNode, CLS_importModuleSelection)
              || hasClass(defNode, CLS_importResource))) {
            continue;
        }

        zvalue resolved = resolveImport(defNode, resolveFn);

        if (exported) {
            resolved = makeExport(resolved);
        }

        arr[i] = resolved;
    }

    zvalue converted = listFromArray(size, arr);

    return makeData(
        get_class(node),
        METH_CALL(cat,
            dataOf(node),
            mapFrom1(STR_statements, converted)));
}

// Documented in spec.
zvalue withTop(zvalue node) {
    return makeData(
        get_class(node),
        collPut(dataOf(node), STR_top, BOOL_TRUE));
}


// Documented in spec.
zvalue withYieldDef(zvalue node, zvalue name) {
    zvalue map = dataOf(node);
    zvalue yieldDef = get(map, STR_yieldDef);
    zvalue newBindings;

    if (yieldDef != NULL) {
        zvalue defStat = makeVarDef(name, makeVarFetch(yieldDef));
        newBindings = mapFrom1(
            STR_statements, listPrepend(defStat, get(node, STR_statements)));
    } else {
        newBindings = mapFrom1(STR_yieldDef, name);
    }

    return makeData(
        get_class(node),
        METH_CALL(cat, map, newBindings));
};

// Documented in spec.
zvalue withoutInterpolate(zvalue node) {
    return makeData(
        get_class(node),
        collDel(dataOf(node), STR_interpolate));
}

// Documented in spec.
zvalue withoutTops(zvalue node) {
    zvalue rawStatements = get(node, STR_statements);
    zint size = get_size(rawStatements);

    zvalue tops = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);
        zvalue defNode = hasClass(s, CLS_export)
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
        zvalue defNode = hasClass(s, CLS_export)
            ? get(s, STR_value)
            : s;

        if (get(defNode, STR_top) != NULL) {
            mains = listAppend(mains,
                makeVarStore(get(defNode, STR_name), get(defNode, STR_value)));
        } else {
            mains = listAppend(mains, s);
        }
    }

    zvalue exports = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);

        if (!hasClass(s, CLS_export)) {
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

    return makeData(
        get_class(node),
        METH_CALL(cat,
            dataOf(node),
            mapFrom1(
                STR_statements, METH_CALL(cat, tops, mains, optSelection))));
}
