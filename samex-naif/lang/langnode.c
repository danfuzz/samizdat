// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "const.h"
#include "type/Bool.h"
#include "type/Class.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/OneOff.h"
#include "type/Record.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
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
    zstring s = zstringFromString(string);
    zvalue result[s.size + 1];
    zint resultAt = 0;

    for (zint at = 0; at <= s.size; /*at*/) {
        zint endAt = at;
        while ((endAt < s.size) && s.chars[endAt] != ch) {
            endAt++;
        }

        zstring one = { endAt - at, &s.chars[at] };
        result[resultAt] = stringFromZstring(one);
        resultAt++;
        at = endAt + 1;
    }

    return listFromArray(resultAt, result);
}

/**
 * Adds a `{(name): Value}` binding to the given map or table.
 */
static zvalue addTypeBinding(zvalue map, zvalue name) {
    return collPut(map, name, CLS_Value);
}

/**
 * Adds a type binding (per above) to a source binding for a given map or
 * table. This is used to build up metainformation about imports.
 */
static zvalue addImportBinding(zvalue map, zvalue source, zvalue name) {
    zvalue names = get(map, source);

    names = addTypeBinding((names == NULL) ? EMPTY_MAP : names, name);
    return collPut(map, source, names);
}

/**
 * Adds a format to a list of same in a resource source map or table. This
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
static zvalue expandYield(zvalue table) {
    zvalue yieldNode = get(table, SYM_yield);

    if (     (yieldNode == NULL)
          || !recordEvalTypeIs(yieldNode, EVAL_nonlocalExit)) {
        return yieldNode;
    }

    zvalue function = get(yieldNode, SYM_function);
    zvalue value = get(yieldNode, SYM_value);
    zvalue yieldDef = get(table, SYM_yieldDef);
    zvalue functionTarget = get(function, SYM_target);

    if (     recordEvalTypeIs(function, EVAL_fetch)
          && recordEvalTypeIs(functionTarget, EVAL_varRef)
          && (yieldDef != NULL)
          && valEq(get(functionTarget, SYM_name), yieldDef)) {
        return value;
    }

    zvalue exitCall;

    switch (recordEvalType(value)) {
        case EVAL_void: {
            exitCall = makeCall(function, NULL);
            break;
        }
        case EVAL_maybe: {
            zvalue arg = makeInterpolate(
                makeMaybeValue(get(value, SYM_value)));
            exitCall = makeCallOrApply(function, listFrom1(arg));
            break;
        }
        default: {
            exitCall = makeCall(function, listFrom1(value));
            break;
        }
    }

    return makeNoYield(exitCall);
};

// Documented in `LangNode` source.
static zvalue makeMapLikeExpression(zvalue mappings, zvalue emptyLiteral,
        zvalue makeMultiValue, zvalue makeOneValue) {
    zint size = get_size(mappings);

    if (size == 0) {
        return emptyLiteral;
    }

    zvalue singleArgs[size * 2];
    zvalue catArgs[size];
    zint singleAt = 0;
    zint catAt = 0;

    #define addToCat(arg) do { \
        catArgs[catAt] = (arg); \
        catAt++; \
    } while (0)

    #define addSingleToCat() do { \
        if (singleAt != 0) { \
            addToCat(makeCall(makeMultiValue, \
                listFromArray(singleAt, singleArgs))); \
            singleAt = 0; \
        } \
    } while (0)

    for (zint i = 0; i < size; i++) {
        zvalue one = nth(mappings, i);
        if (recordEvalTypeIs(one, EVAL_mapping)) {
            zvalue keys = get(one, SYM_keys);
            zvalue value = get(one, SYM_value);
            bool handled = false;
            if (get_size(keys) == 1) {
                zvalue key = nth(keys, 0);
                if (get(key, SYM_interpolate) == NULL) {
                    singleArgs[singleAt] = key;
                    singleArgs[singleAt + 1] = value;
                    singleAt += 2;
                    handled = true;
                }
            }
            if (!handled) {
                addSingleToCat();
                addToCat(makeCallOrApply(makeOneValue,
                    listAppend(keys, value)));
            }
        } else {
            addSingleToCat();
            addToCat(one);
        }
    }

    if (catAt == 0) {
        addSingleToCat();
        return catArgs[0];
    }

    addSingleToCat();
    return makeCall(SYMS(cat),
        listPrepend(emptyLiteral, listFromArray(catAt, catArgs)));
};


//
// Module functions
//

// Documented in spec.
bool canYieldVoid(zvalue node) {
    switch (recordEvalType(node)) {
        case EVAL_apply: { return true;  }
        case EVAL_call:  { return true;  }
        case EVAL_fetch: { return true;  }
        case EVAL_maybe: { return true;  }
        case EVAL_store: { return true;  }
        case EVAL_void:  { return true;  }
        default:         { return false; }
    }
}

// Documented in spec.
zvalue formalsMaxArgs(zvalue formals) {
    zint maxArgs = 0;
    zint sz = get_size(formals);

    for (zint i = 0; i < sz; i++) {
        zvalue one = nth(formals, i);
        zvalue repeat = get(one, SYM_repeat);
        if (valEqNullOk(repeat, SYM_CH_STAR)
            || valEqNullOk(repeat, SYM_CH_PLUS)) {
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
        zvalue repeat = get(one, SYM_repeat);
        if (!(valEqNullOk(repeat, SYM_CH_QMARK)
              || valEqNullOk(repeat, SYM_CH_STAR))) {
            minArgs++;
        }
    }

    return intFromZint(minArgs);
}

// Documented in spec.
zvalue get_baseName(zvalue source) {
    switch (recordEvalType(source)) {
        case EVAL_external: {
            zvalue components = splitAtChar(get(source, SYM_name), STR_CH_DOT);
            return nth(components, get_size(components) - 1);
        }
        case EVAL_internal: {
            zvalue components =
                splitAtChar(get(source, SYM_name), STR_CH_SLASH);
            zvalue last = nth(components, get_size(components) - 1);
            zvalue parts = splitAtChar(last, STR_CH_DOT);
            return nth(parts, 0);
        }
        default: {
            die("Bad type for `get_baseName`.");
        }
    }
}

// Documented in spec.
zvalue get_definedNames(zvalue node) {
    switch (recordEvalType(node)) {
        case EVAL_export: {
            return get_definedNames(get(node, SYM_value));
        }
        case EVAL_importModule:
        case EVAL_importResource:
        case EVAL_varDef:
        case EVAL_varDefMutable: {
            return listFrom1(get(node, SYM_name));
        }
        case EVAL_importModuleSelection: {
            zvalue select = get(node, SYM_select);
            if (select == NULL) {
                die("Cannot call `get_definedNames` on unresolved import.");
            }

            zvalue prefix = get(node, SYM_prefix);
            if (prefix != NULL) {
                zvalue prefixStr = valToString(prefix);
                zint size = get_size(select);
                zvalue arr[size];
                arrayFromList(arr, select);

                for (zint i = 0; i < size; i++) {
                    arr[i] = symbolFromString(METH_CALL(cat, prefixStr, arr[i]));
                }

                return listFromArray(size, arr);
            } else {
                return select;
            }
        }
        default: {
            return EMPTY_LIST;
        }
    }
}

// Documented in spec.
bool isExpression(zvalue node) {
    switch (recordEvalType(node)) {
        case EVAL_apply:   { return true;  }
        case EVAL_call:    { return true;  }
        case EVAL_closure: { return true;  }
        case EVAL_fetch:   { return true;  }
        case EVAL_literal: { return true;  }
        case EVAL_noYield: { return true;  }
        case EVAL_store:   { return true;  }
        case EVAL_varRef:  { return true;  }
        default:           { return false; }
    }
}

// Documented in spec.
zvalue makeApply(zvalue function, zvalue values) {
    if (values == NULL) {
        values = TOK_void;
    }

    return recordFrom2(RECCLS_apply, SYM_function, function, SYM_values, values);
}

// Documented in spec.
zvalue makeAssignmentIfPossible(zvalue target, zvalue value) {
    // This code isn't parallel to the in-language code but has the same
    // effect. The difference stems from the fact that C isn't a great direct
    // host for closures, whereas in-language `lvalue` is very naturally a
    // closure. In this case, the mere presence of `lvalue` is taken as the
    // significant thing, and its value is ignored; instead, per-type
    // assignment conversion is implemented directly below.

    if (get(target, SYM_lvalue) == NULL) {
        return NULL;
    } else if (recordEvalTypeIs(target, EVAL_fetch)) {
        zvalue innerTarget = get(target, SYM_target);
        return recordFrom2(RECCLS_store,
            SYM_target, innerTarget, SYM_value, value);
    } else {
        die("Improper `lvalue` binding.");
    }
}

// Documented in spec.
zvalue makeBasicClosure(zvalue table) {
    return makeRecord(RECCLS_closure,
        METH_CALL(cat,
            tableFrom2(SYM_formals, EMPTY_LIST, SYM_statements, EMPTY_LIST),
            table));
}

// Documented in spec.
zvalue makeCall(zvalue function, zvalue values) {
    if (values == NULL) {
        values = EMPTY_LIST;
    }

    return recordFrom2(RECCLS_call, SYM_function, function, SYM_values, values);
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
        zvalue node = get(one, SYM_interpolate);
        if (node != NULL) {
            addPendingToCooked();
            addToCooked(makeCall(SYMS(collect), listFrom1(node)));
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
            makeCall(SYMS(cat), listFromArray(cookAt, cookedValues)));
    } else {
        return makeApply(function, cookedValues[0]);
    }

    #undef addToCooked
    #undef addPendingToCooked
}

// Documented in spec.
zvalue makeDynamicImport(zvalue node) {
    zvalue format = get(node, SYM_format);
    zvalue name = get(node, SYM_name);
    zvalue select = get(node, SYM_select);
    zvalue source = get(node, SYM_source);

    if (recordEvalTypeIs(node, EVAL_importModule)) {
        zvalue stat = makeVarDef(name,
            makeCall(REFS(loadModule), listFrom1(makeLiteral(source))));

        return listFrom1(stat);
    } else if (recordEvalTypeIs(node, EVAL_importModuleSelection)) {
        zvalue names = get_definedNames(node);
        zint size = get_size(names);
        zvalue loadCall = makeCall(REFS(loadModule),
            listFrom1(makeLiteral(source)));

        zvalue stats[size];
        for (zint i = 0; i < size; i++) {
            zvalue name = nth(names, i);
            zvalue sel = nth(select, i);
            stats[i] = makeVarDef(name,
                makeCall(SYMS(get), listFrom2(loadCall, makeLiteral(sel))));
        }

        return listFromArray(size, stats);
    } else if (recordEvalTypeIs(node, EVAL_importResource)) {
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
    return recordFrom1(RECCLS_export, SYM_value, node);
}

// Documented in spec.
zvalue makeExportSelection(zvalue names) {
    return recordFrom1(RECCLS_exportSelection, SYM_select, names);
}

// Documented in spec.
zvalue makeFullClosure(zvalue baseData) {
    zvalue table = hasClass(baseData, CLS_SymbolTable)
        ? baseData : dataOf(baseData);
    zvalue formals = get(table, SYM_formals);
    zvalue statements = get(table, SYM_statements);
    zint statSz = (statements == NULL) ? 0 : get_size(statements);
    zvalue yieldNode = expandYield(table);

    if (formals == NULL) {
        formals = EMPTY_LIST;
    }

    if (statements == NULL) {
        statements = EMPTY_LIST;
    }

    if (     (yieldNode == NULL)
          && (statSz != 0)
          && (get(table, SYM_yieldDef) == NULL)) {
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

    return makeRecord(RECCLS_closure,
        METH_CALL(cat,
            table,
            tableFrom3(
                SYM_formals,    formals,
                SYM_statements, statements,
                SYM_yield,      yieldNode)));
}

// Documented in spec.
zvalue makeImport(zvalue baseData) {
    // Note: This is a near-transliteration of the equivalent code in
    // `LangNode`.
    zvalue data = baseData;  // Modified in some cases below.

    zvalue select = get(data, SYM_select);
    if (select != NULL) {
        // It's a module binding selection.

        if (get(data, SYM_name) != NULL) {
            die("Import selection name must be a prefix.");
        } else if (get(data, SYM_format) != NULL) {
            die("Cannot import selection of resource.");
        }

        if (valEq(select, SYM_CH_STAR)) {
            // It's a wildcard import.
            data = collDel(data, SYM_select);
        }

        return makeRecord(RECCLS_importModuleSelection, data);
    }

    if (get(data, SYM_name) == NULL) {
        // No `name` provided, so figure out a default one.
        zvalue name = METH_CALL(cat,
            STR_CH_DOLLAR,
            get_baseName(get(baseData, SYM_source)));
        data = collPut(data, SYM_name, symbolFromString(name));
    }

    if (get(data, SYM_format) != NULL) {
        // It's a resource.
        if (hasClass(get(data, SYM_source), RECCLS_external)) {
            die("Cannot import external resource.");
        }
        return makeRecord(RECCLS_importResource, data);
    }

    // It's a whole-module import.
    return makeRecord(RECCLS_importModule, data);
}

// Documented in spec.
zvalue makeInfoTable(zvalue node) {
    zvalue info = get(node, SYM_info);
    if (info != NULL) {
        return info;
    }

    zvalue statements = get(node, SYM_statements);
    zint size = get_size(statements);

    zvalue exports = EMPTY_MAP;
    zvalue imports = EMPTY_MAP;
    zvalue resources = EMPTY_MAP;

    for (zint i = 0; i < size; i++) {
        zvalue s = nth(statements, i);

        if (recordEvalTypeIs(s, EVAL_exportSelection)) {
            zvalue select = get(s, SYM_select);
            zint sz = get_size(select);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(select, j);
                exports = addTypeBinding(exports, name);
            }
        } else if (recordEvalTypeIs(s, EVAL_export)) {
            zvalue names = get_definedNames(s);
            zint sz = get_size(names);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(names, j);
                exports = addTypeBinding(exports, name);
            }
            // And fall through to handle an `import*` payload, if any.
            s = get(s, SYM_value);
        }

        // *Not* `else if` (see above).
        if (recordEvalTypeIs(s, EVAL_importModule)) {
            imports =
                addImportBinding(imports, get(s, SYM_source), SYM_module);
        } else if (recordEvalTypeIs(s, EVAL_importModuleSelection)) {
            zvalue source = get(s, SYM_source);
            zvalue select = get(s, SYM_select);
            if (select == NULL) {
                die("Cannot call `makeInfoTable` on unresolved import.");
            }
            zint sz = get_size(select);
            for (zint j = 0; j < sz; j++) {
                zvalue name = nth(select, j);
                imports = addImportBinding(imports, source, name);
            }
        } else if (recordEvalTypeIs(s, EVAL_importResource)) {
            resources = addResourceBinding(resources,
                get(s, SYM_source), get(s, SYM_format));
        }
    }

    return symbolTableFromArgs(
        SYM_exports,   exports,
        SYM_imports,   imports,
        SYM_resources, resources,
        NULL);
}

// Documented in spec.
zvalue makeInterpolate(zvalue node) {
    return recordFrom3(RECCLS_fetch,
        SYM_target,      node,
        SYM_interpolate, node,
        SYM_lvalue,      EMPTY_LIST);
}

// Documented in spec.
zvalue makeLiteral(zvalue value) {
    return recordFrom1(RECCLS_literal, SYM_value, value);
}

// Documented in spec.
zvalue makeMapExpression(zvalue mappings) {
    return makeMapLikeExpression(
        mappings, makeLiteral(EMPTY_MAP), REFS(makeMap), REFS(makeValueMap));
};

// Documented in spec.
zvalue makeMaybe(zvalue value) {
    return recordFrom1(RECCLS_maybe, SYM_value, value);
}

// Documented in spec.
zvalue makeMaybeValue(zvalue expression) {
    return makeCall(REFS(maybeValue), listFrom1(makeThunk(expression)));
}

// Documented in spec.
zvalue makeNoYield(zvalue value) {
    return recordFrom1(RECCLS_noYield, SYM_value, value);
}

// Documented in spec.
zvalue makeNonlocalExit(zvalue function, zvalue optValue) {
    zvalue value = (optValue == NULL) ? TOK_void : optValue;
    return recordFrom2(RECCLS_nonlocalExit,
        SYM_function, function, SYM_value, value);
}

// Documented in spec.
zvalue makeSymbolLiteral(zvalue name) {
    return makeLiteral(symbolFromString(name));
}

// Documented in spec.
zvalue makeSymbolTableExpression(zvalue mappings) {
    return makeMapLikeExpression(
        mappings, makeLiteral(EMPTY_SYMBOL_TABLE),
        REFS(makeSymbolTable), REFS(makeValueSymbolTable));
};

// Documented in spec.
zvalue makeThunk(zvalue expression) {
    zvalue yieldNode = isExpression(expression)
        ? makeMaybe(expression)
        : expression;

    return makeFullClosure(tableFrom1(SYM_yield, yieldNode));
}

// Documented in spec.
zvalue makeVarRef(zvalue name) {
    return recordFrom1(RECCLS_varRef, SYM_name, name);
}

// Documented in spec.
zvalue makeVarDef(zvalue name, zvalue value) {
    return recordFrom2(RECCLS_varDef, SYM_name, name, SYM_value, value);
}

// Documented in spec.
zvalue makeVarDefMutable(zvalue name, zvalue value) {
    return recordFrom2(RECCLS_varDefMutable, SYM_name, name, SYM_value, value);
}

// Documented in spec.
zvalue makeVarFetch(zvalue name) {
    return recordFrom1(RECCLS_fetch, SYM_target, makeVarRef(name));
}

// Documented in spec.
zvalue makeVarFetchLvalue(zvalue name) {
    // See discussion in `makeAssignmentIfPossible` above, for details about
    // `lvalue`.
    return recordFrom2(RECCLS_fetch,
        SYM_target, makeVarRef(name), SYM_lvalue, EMPTY_LIST);
}

// Documented in spec.
zvalue makeVarStore(zvalue name, zvalue value) {
    return recordFrom2(RECCLS_store,
        SYM_target, makeVarRef(name), SYM_value, value);
}

// Documented in spec.
zvalue resolveImport(zvalue node, zvalue resolveFn) {
    if (recordEvalTypeIs(node, EVAL_importResource)) {
        // No conversion, just validation. TODO: Validate.
        //
        // **Note:** This clause is at the top so as to avoid the call to
        // `resolveFn()` below, which is inappropriate to do on resources.
        return node;
    }

    zvalue resolved = EMPTY_SYMBOL_TABLE;
    if (resolveFn != NULL) {
        zvalue source = get(node, SYM_source);
        resolved = FUN_CALL(resolveFn, source);
        if (resolved == NULL) {
            die("Could not resolve import.");
        } else if (!recordEvalTypeIs(resolved, EVAL_module)) {
            die("Invalid resolution result (not a `@module`)");
        }
    }

    if (recordEvalTypeIs(node, EVAL_importModule)) {
        // No conversion, just validation (done above).
        return node;
    } else if (recordEvalTypeIs(node, EVAL_importModuleSelection)) {
        // Get the exports. When given a `NULL` `resolveFn`, this acts as if
        // all sources resolve to an empty export map, and hence this node
        // won't bind anything.
        zvalue exports = EMPTY_LIST;
        zvalue info = get(resolved, SYM_info);
        if (info != NULL) {
            exports = get(info, SYM_exports);
        }

        zvalue select = get(node, SYM_select);
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
            return makeRecord(
                RECCLS_importModuleSelection,
                collPut(dataOf(node), SYM_select, select));
        }
    } else {
        die("Bad node type for `resolveImport`.");
    }
}

// Documented in spec.
zvalue withFormals(zvalue node, zvalue formals) {
    return makeRecord(
        get_class(node),
        collPut(dataOf(node), SYM_formals, formals));
}

// Documented in spec.
zvalue withModuleDefs(zvalue node) {
    if (!valEqNullOk(get(node, SYM_yield), TOK_void)) {
        die("Invalid node for `withModuleDefs` (has non-void `yield`).");
    }

    zvalue info = makeInfoTable(node);

    zvalue rawStatements = get(node, SYM_statements);
    zint size = get_size(rawStatements);
    zvalue statements = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);

        if (recordEvalTypeIs(s, EVAL_exportSelection)) {
            continue;
        } else if (recordEvalTypeIs(s, EVAL_export)) {
            s = get(s, SYM_value);
        }

        statements = listAppend(statements, s);
    }

    zvalue exportValues = EMPTY_LIST;
    zvalue exportInfo = get(info, SYM_exports);
    zint exSize = get_size(exportInfo);
    zmapping mappings[exSize];
    arrayFromMap(mappings, exportInfo);
    for (zint i = 0; i < exSize; i++) {
        zvalue name = mappings[i].key;
        exportValues = METH_CALL(cat, exportValues,
            listFrom2(makeLiteral(name), makeVarFetch(name)));
    }

    zvalue yieldExports = (exSize == 0)
        ? makeLiteral(EMPTY_SYMBOL_TABLE)
        : makeCall(REFS(makeSymbolTable), exportValues);
    zvalue yieldInfo = makeLiteral(info);
    zvalue yieldNode = makeCall(REFS(makeRecord),
        listFrom2(
            makeLiteral(RECCLS_module),
            makeCall(REFS(makeSymbolTable),
                listFrom4(
                    makeLiteral(SYM_exports), yieldExports,
                    makeLiteral(SYM_info),    yieldInfo))));

    return makeRecord(
        get_class(node),
        METH_CALL(cat,
            dataOf(node),
            tableFrom3(
                SYM_info,       info,
                SYM_statements, statements,
                SYM_yield,      yieldNode)));
}

// Documented in spec.
zvalue withName(zvalue node, zvalue name) {
    return makeRecord(
        get_class(node),
        collPut(dataOf(node), SYM_name, name));
}

// Documented in spec.
zvalue withResolvedImports(zvalue node, zvalue resolveFn) {
    zvalue rawStatements = get(node, SYM_statements);
    zint size = get_size(rawStatements);
    zvalue arr[size];
    arrayFromList(arr, rawStatements);

    for (zint i = 0; i < size; i++) {
        zvalue s = arr[i];
        bool exported = false;
        zvalue defNode = s;

        if (recordEvalTypeIs(s, EVAL_export)) {
            exported = true;
            defNode = get(s, SYM_value);
        }

        if (!(   recordEvalTypeIs(defNode, EVAL_importModule)
              || recordEvalTypeIs(defNode, EVAL_importModuleSelection)
              || recordEvalTypeIs(defNode, EVAL_importResource))) {
            continue;
        }

        zvalue resolved = resolveImport(defNode, resolveFn);

        if (exported) {
            resolved = makeExport(resolved);
        }

        arr[i] = resolved;
    }

    zvalue converted = listFromArray(size, arr);

    return makeRecord(
        get_class(node),
        METH_CALL(cat,
            dataOf(node),
            tableFrom1(SYM_statements, converted)));
}

// Documented in spec.
zvalue withTop(zvalue node) {
    return makeRecord(
        get_class(node),
        collPut(dataOf(node), SYM_top, BOOL_TRUE));
}


// Documented in spec.
zvalue withYieldDef(zvalue node, zvalue name) {
    zvalue table = dataOf(node);
    zvalue yieldDef = get(table, SYM_yieldDef);
    zvalue newBindings;

    if (yieldDef != NULL) {
        zvalue defStat = makeVarDef(name, makeVarFetch(yieldDef));
        newBindings = tableFrom1(
            SYM_statements, listPrepend(defStat, get(node, SYM_statements)));
    } else {
        newBindings = tableFrom1(SYM_yieldDef, name);
    }

    return makeRecord(
        get_class(node),
        METH_CALL(cat, table, newBindings));
};

// Documented in spec.
zvalue withoutInterpolate(zvalue node) {
    return makeRecord(
        get_class(node),
        collDel(dataOf(node), SYM_interpolate));
}

// Documented in spec.
zvalue withoutTops(zvalue node) {
    zvalue rawStatements = get(node, SYM_statements);
    zint size = get_size(rawStatements);

    zvalue tops = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);
        zvalue defNode = recordEvalTypeIs(s, EVAL_export)
            ? get(s, SYM_value)
            : s;

        if (get(defNode, SYM_top) != NULL) {
            tops = listAppend(tops,
                makeVarDef(get(defNode, SYM_name), NULL));
        }
    }

    zvalue mains = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);
        zvalue defNode = recordEvalTypeIs(s, EVAL_export)
            ? get(s, SYM_value)
            : s;

        if (get(defNode, SYM_top) != NULL) {
            mains = listAppend(mains,
                makeVarStore(get(defNode, SYM_name), get(defNode, SYM_value)));
        } else {
            mains = listAppend(mains, s);
        }
    }

    zvalue exports = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = nth(rawStatements, i);

        if (!recordEvalTypeIs(s, EVAL_export)) {
            continue;
        }

        zvalue defNode = get(s, SYM_value);
        if (get(defNode, SYM_top) == NULL) {
            continue;
        }

        exports = listAppend(exports, get(defNode, SYM_name));
    };

    zvalue optSelection = (get_size(exports) == 0)
        ? EMPTY_LIST
        : listFrom1(makeExportSelection(exports));

    return makeRecord(
        get_class(node),
        METH_CALL(cat,
            dataOf(node),
            tableFrom1(
                SYM_statements, METH_CALL(cat, tops, mains, optSelection))));
}
