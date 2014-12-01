// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "const.h"
#include "type/Bool.h"
#include "type/Class.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
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

        result[resultAt] =
            stringFromZstring((zstring) {endAt - at, &s.chars[at]});
        resultAt++;
        at = endAt + 1;
    }

    return listFromZarray((zarray) {resultAt, result});
}

/**
 * Adds a `{(name): Value}` binding to the given map or table.
 */
static zvalue addTypeBinding(zvalue map, zvalue name) {
    return cm_cat(map, cm_new_SymbolTable(name, CLS_Value));
}

/**
 * Adds a type binding (per above) to a source binding for a given map or
 * table. This is used to build up metainformation about imports.
 */
static zvalue addImportBinding(zvalue map, zvalue source, zvalue name) {
    zvalue names = cm_get(map, source);

    names = addTypeBinding((names == NULL) ? EMPTY_MAP : names, name);
    return cm_cat(map, mapFromMapping((zmapping) {source, names}));
}

/**
 * Adds a format to a list of same in a resource source map or table. This
 * is used to build up metainformation about resources.
 */
static zvalue addResourceBinding(zvalue map, zvalue source, zvalue format) {
    zvalue formats = cm_get(map, source);

    if (formats == NULL) {
        formats = EMPTY_LIST;
    }

    // Unlike the `LangNode` version, this one doesn't de-duplicate formats.
    formats = listAppend(formats, format);
    return cm_cat(map, mapFromMapping((zmapping) {source, formats}));
}

// Documented in `LangNode` source.
static zvalue expandYield(zvalue node) {
    zvalue yieldNode = cm_get(node, SYM(yield));

    if (     (yieldNode == NULL)
          || !recordEvalTypeIs(yieldNode, EVAL_nonlocalExit)) {
        return yieldNode;
    }

    zvalue function = cm_get(yieldNode, SYM(function));
    zvalue value = cm_get(yieldNode, SYM(value));
    zvalue yieldDef = cm_get(node, SYM(yieldDef));
    zvalue functionTarget = cm_get(function, SYM(target));

    if (     recordEvalTypeIs(function, EVAL_fetch)
          && recordEvalTypeIs(functionTarget, EVAL_varRef)
          && (yieldDef != NULL)
          && valEq(cm_get(functionTarget, SYM(name)), yieldDef)) {
        return value;
    }

    zvalue exitCall;

    switch (recordEvalType(value)) {
        case EVAL_void: {
            exitCall = makeFunCall(function, NULL);
            break;
        }
        case EVAL_maybe: {
            zvalue arg = makeInterpolate(
                makeMaybeValue(cm_get(value, SYM(value))));
            exitCall = makeFunCallGeneral(function, cm_new_List(arg));
            break;
        }
        default: {
            exitCall = makeFunCall(function, cm_new_List(value));
            break;
        }
    }

    return makeNoYield(exitCall);
};

// Documented in `LangNode` source.
static zvalue extractMethods(zvalue allMethods, zvalue scope) {
    zarray methArr = zarrayFromList(allMethods);
    zvalue names = EMPTY_SYMBOL_TABLE;
    zvalue pairs = EMPTY_LIST;

    for (zint i = 0; i < methArr.size; i++) {
        zvalue one = methArr.elems[i];
        zvalue name = cm_get(one, SYM(name));

        if (!valEq(scope, get_name(one))) {
            continue;
        } else if (cm_get(names, name)) {
            die("Duplicate method: %s", cm_debugString(name));
        }

        names = symtabCatMapping(names, (zmapping) {name, BOOL_TRUE});
        pairs = listAppend(pairs,
            cm_new_List(
                makeLiteral(name),
                cm_new(Record, SYM(closure), one)));
    }

    return makeCall(LITS(SymbolTable), SYMS(new),
        METH_APPLY(EMPTY_LIST, cat, pairs));
}

// Documented in `LangNode` source.
static zvalue makeMapLikeExpression(zvalue mappings, zvalue clsLit,
        zvalue emptyLit) {
    zint size = get_size(mappings);

    if (size == 0) {
        return emptyLit;
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
            addToCat(makeCall(clsLit, SYMS(new), \
                listFromZarray((zarray) {singleAt, singleArgs}))); \
            singleAt = 0; \
        } \
    } while (0)

    for (zint i = 0; i < size; i++) {
        zvalue one = cm_nth(mappings, i);
        if (recordEvalTypeIs(one, EVAL_mapping)) {
            zvalue keys = cm_get(one, SYM(keys));
            zvalue value = cm_get(one, SYM(value));
            bool handled = false;
            if (get_size(keys) == 1) {
                zvalue key = cm_nth(keys, 0);
                if (cm_get(key, SYM(interpolate)) == NULL) {
                    singleArgs[singleAt] = key;
                    singleArgs[singleAt + 1] = value;
                    singleAt += 2;
                    handled = true;
                }
            }
            if (!handled) {
                addSingleToCat();
                addToCat(makeCallGeneral(clsLit, SYMS(singleValue),
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
    return makeCall(emptyLit, SYMS(cat),
        listFromZarray((zarray) {catAt, catArgs}));
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
zvalue extractLiteral(zvalue node) {
    return recordEvalTypeIs(node, EVAL_literal)
        ? cm_get(node, SYM(value))
        : NULL;
}

// Documented in spec.
zvalue formalsMaxArgs(zvalue formals) {
    zint maxArgs = 0;
    zint sz = get_size(formals);

    for (zint i = 0; i < sz; i++) {
        zvalue one = cm_nth(formals, i);
        zvalue repeat = cm_get(one, SYM(repeat));
        if (valEqNullOk(repeat, SYM(CH_STAR))
            || valEqNullOk(repeat, SYM(CH_PLUS))) {
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
        zvalue one = cm_nth(formals, i);
        zvalue repeat = cm_get(one, SYM(repeat));
        if (!(valEqNullOk(repeat, SYM(CH_QMARK))
              || valEqNullOk(repeat, SYM(CH_STAR)))) {
            minArgs++;
        }
    }

    return intFromZint(minArgs);
}

// Documented in spec.
zvalue get_baseName(zvalue source) {
    switch (recordEvalType(source)) {
        case EVAL_external: {
            zvalue components =
                splitAtChar(cm_get(source, SYM(name)), STR_CH_DOT);
            return cm_nth(components, get_size(components) - 1);
        }
        case EVAL_internal: {
            zvalue components =
                splitAtChar(cm_get(source, SYM(name)), STR_CH_SLASH);
            zvalue last = cm_nth(components, get_size(components) - 1);
            zvalue parts = splitAtChar(last, STR_CH_DOT);
            return cm_nth(parts, 0);
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
            return get_definedNames(cm_get(node, SYM(value)));
        }
        case EVAL_importModule:
        case EVAL_importResource:
        case EVAL_varDef: {
            return cm_new_List(cm_get(node, SYM(name)));
        }
        case EVAL_importModuleSelection: {
            zvalue select = cm_get(node, SYM(select));
            if (select == NULL) {
                die("Cannot call `get_definedNames` on unresolved import.");
            }

            zvalue prefix = cm_get(node, SYM(prefix));
            if (prefix != NULL) {
                zarray arr = zarrayFromList(select);
                zvalue elems[arr.size];

                for (zint i = 0; i < arr.size; i++) {
                    elems[i] = cm_cat(prefix, arr.elems[i]);
                }

                return listFromZarray((zarray) {arr.size, elems});
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
zvalue makeAssignmentIfPossible(zvalue target, zvalue value) {
    // This code isn't parallel to the in-language code but has the same
    // effect. The difference stems from the fact that C isn't a great direct
    // host for closures, whereas in-language `lvalue` is very naturally a
    // closure. In this case, the mere presence of `lvalue` is taken as the
    // significant thing, and its value is ignored; instead, per-type
    // assignment conversion is implemented directly below.

    if (cm_get(target, SYM(lvalue)) == NULL) {
        return NULL;
    } else if (recordEvalTypeIs(target, EVAL_fetch)) {
        zvalue innerTarget = cm_get(target, SYM(target));
        return recFrom2(SYM(store),
            SYM(target), innerTarget, SYM(value), value);
    } else {
        die("Improper `lvalue` binding.");
    }
}

// Documented in spec.
zvalue makeApply(zvalue target, zvalue name, zvalue values) {
    if (values == NULL) {
        values = TOK_void;
    }

    return recFrom3(SYM(apply),
        SYM(name),   name,
        SYM(target), target,
        SYM(values), values);
}

// Documented in spec.
zvalue makeBasicClosure(zvalue table) {
    return cm_new(Record, SYM(closure),
        cm_cat(
            symtabFrom2(SYM(formals), EMPTY_LIST, SYM(statements), EMPTY_LIST),
            table));
}

// Documented in spec.
zvalue makeCall(zvalue target, zvalue name, zvalue values) {
    if (values == NULL) {
        values = EMPTY_LIST;
    }

    return recFrom3(SYM(call),
        SYM(name),   name,
        SYM(target), target,
        SYM(values), values);
}

// Documented in spec.
zvalue makeCallGeneral(zvalue target, zvalue name, zvalue values) {
    // This is a fairly direct (but not exact) transliteration
    // of the corresponding code in `LangNode`.

    zint sz = (values == NULL) ? 0 : get_size(values);
    zvalue pending[sz];
    zvalue cookedValues[sz];
    zint pendAt = 0;
    zint cookAt = 0;

    if (sz == 0) {
        return makeApply(target, name, NULL);
    }

    #define addToCooked(actual) do { \
        cookedValues[cookAt] = (actual); \
        cookAt++; \
    } while (0)

    #define addPendingToCooked() do { \
        if (pendAt != 0) { \
            addToCooked(makeCall(LITS(List), SYMS(new), \
                listFromZarray((zarray) {pendAt, pending}))); \
            pendAt = 0; \
        } \
    } while (0)

    for (zint i = 0; i < sz; i++) {
        zvalue one = cm_nth(values, i);
        zvalue node = cm_get(one, SYM(interpolate));
        if (node != NULL) {
            addPendingToCooked();
            addToCooked(makeCall(node, SYMS(collect), EMPTY_LIST));
        } else {
            pending[pendAt] = one;
            pendAt++;
        }
    }

    if (cookAt == 0) {
        // There were no interpolated arguments.
        return makeCall(target, name, values);
    }

    addPendingToCooked();

    if (cookAt > 1) {
        zvalue cookedList = listFromZarray((zarray) {cookAt, cookedValues});
        return makeApply(target, name,
            makeCall(LITS(EMPTY_LIST), SYMS(cat), cookedList));
    } else {
        return makeApply(target, name, cookedValues[0]);
    }

    #undef addToCooked
    #undef addPendingToCooked
}

// Documented in spec.
zvalue makeClassDef(zvalue name, zvalue attributes, zvalue methods) {
    zvalue attribMap = METH_APPLY(EMPTY_MAP, cat, attributes);
    zint attribSize = get_size(attribMap);

    if (get_size(attributes) != attribSize) {
        die("Duplicate attribute.");
    }

    zvalue keys = METH_CALL(attribMap, keyList);
    for (zint i = 0; i < attribSize; i++) {
        zvalue one = cm_nth(keys, i);
        if (!(valEq(one, SYM(access)) || valEq(one, SYM(new)))) {
            die("Invalid attribute: %s", cm_debugString(one));
        }
    }

    zvalue accessSecret = cm_get(attribMap, SYM(access));
    if (accessSecret != NULL) {
        accessSecret = recFrom2(SYM(mapping),
            SYM(keys),  cm_new_List(SYMS(access)),
            SYM(value), accessSecret);
    }

    zvalue newSecret = cm_get(attribMap, SYM(new));
    if (newSecret != NULL) {
        newSecret = recFrom2(SYM(mapping),
            SYM(keys),  cm_new_List(SYMS(new)),
            SYM(value), newSecret);
    }

    zvalue config = makeSymbolTableExpression(
        cm_new_List(accessSecret, newSecret));

    zvalue call = makeCall(LITS(Object), SYMS(subclass),
        cm_new_List(
            makeLiteral(name),
            config,
            extractMethods(methods, SYM(classMethod)),
            extractMethods(methods, SYM(instanceMethod))));

    return withTop(makeVarDef(name, SYM(result), call));
}

// Documented in spec.
zvalue makeDynamicImport(zvalue node) {
    zvalue format = cm_get(node, SYM(format));
    zvalue name = cm_get(node, SYM(name));
    zvalue select = cm_get(node, SYM(select));
    zvalue source = cm_get(node, SYM(source));

    switch (recordEvalType(node)) {
        case EVAL_importModule: {
            zvalue stat = makeVarDef(name, SYM(result),
                makeFunCall(REFS(loadModule),
                    cm_new_List(makeLiteral(source))));
            return cm_new_List(stat);
        }
        case EVAL_importModuleSelection: {
            zvalue names = get_definedNames(node);
            zint size = get_size(names);
            zvalue loadCall = makeFunCall(REFS(loadModule),
                cm_new_List(makeLiteral(source)));

            zvalue stats[size];
            for (zint i = 0; i < size; i++) {
                zvalue name = cm_nth(names, i);
                zvalue sel = cm_nth(select, i);
                stats[i] = makeVarDef(name, SYM(result),
                    makeCall(loadCall, SYMS(get),
                        cm_new_List(makeLiteral(sel))));
            }

            return listFromZarray((zarray) {size, stats});
        }
        case EVAL_importResource: {
            zvalue stat = makeVarDef(name, SYM(result),
                makeFunCall(REFS(loadResource),
                    cm_new_List(makeLiteral(source), makeLiteral(format))));
            return cm_new_List(stat);
        }
        default: {
            die("Bad node type for makeDynamicImport");
        }
    }
}

// Documented in spec.
zvalue makeExport(zvalue node) {
    return recFrom1(SYM(export), SYM(value), node);
}

// Documented in spec.
zvalue makeExportSelection(zvalue names) {
    return recFrom1(SYM(exportSelection), SYM(select), names);
}

// Documented in spec.
zvalue makeFullClosure(zvalue base) {
    zvalue formals = cm_get(base, SYM(formals));
    zvalue statements = cm_get(base, SYM(statements));
    zint statSz = (statements == NULL) ? 0 : get_size(statements);
    zvalue yieldNode = expandYield(base);

    if (formals == NULL) {
        formals = EMPTY_LIST;
    }

    if (statements == NULL) {
        statements = EMPTY_LIST;
    }

    if (     (yieldNode == NULL)
          && (statSz != 0)
          && (cm_get(base, SYM(yieldDef)) == NULL)) {
        zvalue lastStat = cm_nth(statements, statSz - 1);
        if (isExpression(lastStat)) {
            statements = METH_CALL(statements, sliceExclusive, INT_0);
            yieldNode = canYieldVoid(lastStat)
                ? makeMaybe(lastStat)
                : lastStat;
        }
    }

    if (yieldNode == NULL) {
        yieldNode = TOK_void;
    }

    return cm_new(Record, SYM(closure),
        cm_cat(
            base,
            symtabFrom3(
                SYM(formals),    formals,
                SYM(statements), statements,
                SYM(yield),      yieldNode)));
}

// Documented in spec.
zvalue makeFunCall(zvalue function, zvalue values) {
    if (symbolEq(get_name(function), SYM(literal))) {
        zvalue first = cm_nth(values, 0);
        zvalue rest = METH_CALL(values, sliceInclusive, INT_1);
        return makeCall(first, function, rest);
    } else {
        return makeCall(function, SYMS(call), values);
    }
}

// Documented in spec.
zvalue makeFunCallGeneral(zvalue function, zvalue values) {
    return makeCallGeneral(function, SYMS(call), values);
}

// Documented in spec.
zvalue makeImport(zvalue baseData) {
    // Note: This is a near-transliteration of the equivalent code in
    // `LangNode`.
    zvalue data = baseData;  // Modified in some cases below.

    zvalue select = cm_get(data, SYM(select));
    if (select != NULL) {
        // It's a module binding selection.

        if (cm_get(data, SYM(name)) != NULL) {
            die("Import selection name must be a prefix.");
        } else if (cm_get(data, SYM(format)) != NULL) {
            die("Cannot import selection of resource.");
        }

        if (valEq(select, SYM(CH_STAR))) {
            // It's a wildcard import.
            data = METH_CALL(data, del, SYM(select));
        }

        return cm_new(Record, SYM(importModuleSelection), data);
    }

    if (cm_get(data, SYM(name)) == NULL) {
        // No `name` provided, so figure out a default one.
        zvalue name = cm_cat(
            STR_CH_DOLLAR,
            get_baseName(cm_get(baseData, SYM(source))));
        data = cm_cat(data,
            cm_new_SymbolTable(SYM(name), symbolFromString(name)));
    }

    if (cm_get(data, SYM(format)) != NULL) {
        // It's a resource.
        if (recordEvalTypeIs(cm_get(data, SYM(source)), EVAL_external)) {
            die("Cannot import external resource.");
        }
        return cm_new(Record, SYM(importResource), data);
    }

    // It's a whole-module import.
    return cm_new(Record, SYM(importModule), data);
}

// Documented in spec.
zvalue makeInfoTable(zvalue node) {
    zvalue info = cm_get(node, SYM(info));
    if (info != NULL) {
        return info;
    }

    zvalue statements = cm_get(node, SYM(statements));
    zint size = get_size(statements);

    zvalue exports = EMPTY_MAP;
    zvalue imports = EMPTY_MAP;
    zvalue resources = EMPTY_MAP;

    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(statements, i);

        switch (recordEvalType(s)) {
            case EVAL_exportSelection: {
                zvalue select = cm_get(s, SYM(select));
                zint sz = get_size(select);
                for (zint j = 0; j < sz; j++) {
                    zvalue name = cm_nth(select, j);
                    exports = addTypeBinding(exports, name);
                }
                break;
            }
            case EVAL_export: {
                zvalue names = get_definedNames(s);
                zint sz = get_size(names);
                for (zint j = 0; j < sz; j++) {
                    zvalue name = cm_nth(names, j);
                    exports = addTypeBinding(exports, name);
                }
                // And fall through to the next `switch` statement, to handle
                // an `import*` payload, if any.
                s = cm_get(s, SYM(value));
                break;
            }
            default: {
                // The rest of the types are intentionally left un-handled.
                break;
            }
        }

        // Intentionally *not* part of the above `switch` (see comment above).
        switch (recordEvalType(s)) {
            case EVAL_importModule: {
                imports = addImportBinding(imports,
                    cm_get(s, SYM(source)), SYM(module));
                break;
            }
            case EVAL_importModuleSelection: {
                zvalue source = cm_get(s, SYM(source));
                zvalue select = cm_get(s, SYM(select));
                if (select == NULL) {
                    die("Cannot call `makeInfoTable` on unresolved import.");
                }
                zint sz = get_size(select);
                for (zint j = 0; j < sz; j++) {
                    zvalue name = cm_nth(select, j);
                    imports = addImportBinding(imports, source, name);
                }
                break;
            }
            case EVAL_importResource: {
                resources = addResourceBinding(resources,
                    cm_get(s, SYM(source)), cm_get(s, SYM(format)));
                break;
            }
            default: {
                // The rest of the types are intentionally left un-handled.
                break;
            }
        }
    }

    return symtabFrom3(
        SYM(exports),   exports,
        SYM(imports),   imports,
        SYM(resources), resources);
}

// Documented in spec.
zvalue makeInterpolate(zvalue node) {
    return recFrom3(SYM(fetch),
        SYM(target),      node,
        SYM(interpolate), node,
        SYM(lvalue),      EMPTY_LIST);
}

// Documented in spec.
zvalue makeLiteral(zvalue value) {
    return recFrom1(SYM(literal), SYM(value), value);
}

// Documented in spec.
zvalue makeMapExpression(zvalue mappings) {
    return makeMapLikeExpression(mappings, LITS(Map), LITS(EMPTY_MAP));
};

// Documented in spec.
zvalue makeMaybe(zvalue value) {
    return recFrom1(SYM(maybe), SYM(value), value);
}

// Documented in spec.
zvalue makeMaybeValue(zvalue expression) {
    zvalue box = cm_get(expression, SYM(box));

    if (box != NULL) {
        return box;
    } else {
        return makeFunCall(REFS(maybeValue),
            cm_new_List(makeThunk(expression)));
    }
}

// Documented in spec.
zvalue makeNoYield(zvalue value) {
    return recFrom1(SYM(noYield), SYM(value), value);
}

// Documented in spec.
zvalue makeNonlocalExit(zvalue function, zvalue optValue) {
    zvalue value = (optValue == NULL) ? TOK_void : optValue;
    return recFrom2(SYM(nonlocalExit),
        SYM(function), function, SYM(value), value);
}

// Documented in spec.
zvalue makeRecordExpression(zvalue name, zvalue data) {
    zvalue nameValue = extractLiteral(name);
    zvalue dataValue = extractLiteral(data);

    if ((nameValue != NULL) && (dataValue != NULL)) {
        return makeLiteral(cm_new(Record, nameValue, dataValue));
    } else {
        return makeCall(LITS(Record), SYMS(new), cm_new_List(name, data));
    }
}

// Documented in spec.
zvalue makeSymbolTableExpression(zvalue mappings) {
    return makeMapLikeExpression(
        mappings, LITS(SymbolTable), LITS(EMPTY_SYMBOL_TABLE));
};

// Documented in spec.
zvalue makeThunk(zvalue expression) {
    zvalue yieldNode = isExpression(expression)
        ? makeMaybe(expression)
        : expression;

    return makeFullClosure(cm_new_SymbolTable(SYM(yield), yieldNode));
}

// Documented in spec.
zvalue makeVarRef(zvalue name) {
    return recFrom1(SYM(varRef), SYM(name), name);
}

// Documented in spec.
zvalue makeVarDef(zvalue name, zvalue box, zvalue optValue) {
    zvalue value = (optValue == NULL) ? TOK_void : optValue;
    return recFrom3(SYM(varDef),
        SYM(name),  name,
        SYM(box),   box,
        SYM(value), value);
}

// Documented in spec.
zvalue makeVarFetch(zvalue name) {
    return recFrom1(SYM(fetch), SYM(target), makeVarRef(name));
}

// Documented in spec.
zvalue makeVarFetchGeneral(zvalue name) {
    // See discussion in `makeAssignmentIfPossible` above, for details about
    // `lvalue`.
    zvalue ref = makeVarRef(name);
    return recFrom3(SYM(fetch),
        SYM(box),    ref,
        SYM(lvalue), EMPTY_LIST,
        SYM(target), ref);
}

// Documented in spec.
zvalue makeVarStore(zvalue name, zvalue value) {
    return recFrom2(SYM(store),
        SYM(target), makeVarRef(name), SYM(value), value);
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
        zvalue source = cm_get(node, SYM(source));
        resolved = FUN_CALL(resolveFn, source);
        if (resolved == NULL) {
            die("Could not resolve import.");
        } else if (!recordEvalTypeIs(resolved, EVAL_module)) {
            die("Invalid resolution result (not a `@module`)");
        }
    }

    switch (recordEvalType(node)) {
        case EVAL_importModule: {
            // No conversion, just validation (done above).
            return node;
        }
        case EVAL_importModuleSelection: {
            // Get the exports. When given a `NULL` `resolveFn`, this acts as
            // if all sources resolve to an empty export map, and hence this
            // node won't bind anything.
            zvalue exports = EMPTY_LIST;
            zvalue info = cm_get(resolved, SYM(info));
            if (info != NULL) {
                exports = cm_get(info, SYM(exports));
            }

            zvalue select = cm_get(node, SYM(select));
            if (select != NULL) {
                // The selection is specified. So no modification needs to be
                // done to the node, just validation, including of the import
                // in general (above) and the particular selection (here).
                zint size = get_size(select);
                for (zint i = 0; i < size; i++) {
                    zvalue one = cm_nth(select, i);
                    if (cm_get(exports, one) == NULL) {
                        die("Could not resolve import selection.");
                    }
                }
                return node;
            } else {
                // It's a wildcard select.
                select = METH_CALL(exports, keyList);
                return cm_cat(node, cm_new_SymbolTable(SYM(select), select));
            }
        }
        default: {
            die("Bad node type for `resolveImport`.");
        }
    }
}

// Documented in spec.
zvalue withFormals(zvalue node, zvalue formals) {
    return cm_cat(node, cm_new_SymbolTable(SYM(formals), formals));
}

// Documented in spec.
zvalue withModuleDefs(zvalue node) {
    if (!valEqNullOk(cm_get(node, SYM(yield)), TOK_void)) {
        die("Invalid node for `withModuleDefs` (has non-void `yield`).");
    }

    zvalue info = makeInfoTable(node);

    zvalue rawStatements = cm_get(node, SYM(statements));
    zint size = get_size(rawStatements);
    zvalue statements = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(rawStatements, i);

        switch (recordEvalType(s)) {
            case EVAL_exportSelection: {
                continue;
            }
            case EVAL_export: {
                s = cm_get(s, SYM(value));
                break;
            }
            default: {
                // The rest of the types are intentionally left un-handled.
                break;
            }
        }

        statements = listAppend(statements, s);
    }

    zvalue exportValues = EMPTY_LIST;
    zassoc exports = zassocFromMap(cm_get(info, SYM(exports)));
    for (zint i = 0; i < exports.size; i++) {
        zvalue name = exports.elems[i].key;
        exportValues = cm_cat(exportValues,
            cm_new_List(makeLiteral(name), makeVarFetch(name)));
    }

    zvalue yieldExports = (exports.size == 0)
        ? LITS(EMPTY_SYMBOL_TABLE)
        : makeCall(LITS(SymbolTable), SYMS(new), exportValues);
    zvalue yieldInfo = makeLiteral(info);
    zvalue yieldNode = makeCall(LITS(Record), SYMS(new),
        cm_new_List(
            SYMS(module),
            makeCall(LITS(SymbolTable), SYMS(new),
                cm_new_List(
                    SYMS(exports), yieldExports,
                    SYMS(info),    yieldInfo))));

    return cm_cat(node,
        symtabFrom3(
            SYM(info),       info,
            SYM(statements), statements,
            SYM(yield),      yieldNode));
}

// Documented in spec.
zvalue withName(zvalue node, zvalue name) {
    return cm_cat(node, cm_new_SymbolTable(SYM(name), name));
}

// Documented in spec.
zvalue withResolvedImports(zvalue node, zvalue resolveFn) {
    zvalue rawStatements = cm_get(node, SYM(statements));
    zarray arr = zarrayFromList(rawStatements);
    zvalue elems[arr.size];

    for (zint i = 0; i < arr.size; i++) {
        zvalue s = elems[i] = arr.elems[i];
        bool exported = false;
        zvalue defNode = s;

        if (recordEvalTypeIs(s, EVAL_export)) {
            exported = true;
            defNode = cm_get(s, SYM(value));
        }

        switch (recordEvalType(defNode)) {
            case EVAL_importModule:
            case EVAL_importModuleSelection:
            case EVAL_importResource: {
                zvalue resolved = resolveImport(defNode, resolveFn);
                elems[i] = exported ? makeExport(resolved) : resolved;
            }
            default: {
                // The rest of the types are intentionally left un-handled.
                break;
            }
        }
    }

    zvalue converted = listFromZarray((zarray) {arr.size, elems});

    return cm_cat(node, cm_new_SymbolTable(SYM(statements), converted));
}

// Documented in spec.
zvalue withTop(zvalue node) {
    return cm_cat(node, cm_new_SymbolTable(SYM(top), BOOL_TRUE));
}


// Documented in spec.
zvalue withYieldDef(zvalue node, zvalue name) {
    zvalue yieldDef = cm_get(node, SYM(yieldDef));
    zvalue newBindings;

    if (yieldDef != NULL) {
        zvalue defStat = makeVarDef(name, SYM(result), makeVarFetch(yieldDef));
        newBindings = cm_new_SymbolTable(
            SYM(statements),
            listPrepend(defStat, cm_get(node, SYM(statements))));
    } else {
        newBindings = cm_new_SymbolTable(SYM(yieldDef), name);
    }

    return cm_cat(node, newBindings);
};

// Documented in spec.
zvalue withoutIntermediates(zvalue node) {
    return METH_CALL(node, del, SYM(box), SYM(interpolate), SYM(lvalue));
}

// Documented in spec.
zvalue withoutTops(zvalue node) {
    zvalue rawStatements = cm_get(node, SYM(statements));
    zint size = get_size(rawStatements);

    zvalue tops = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(rawStatements, i);
        zvalue defNode = recordEvalTypeIs(s, EVAL_export)
            ? cm_get(s, SYM(value))
            : s;

        if (cm_get(defNode, SYM(top)) != NULL) {
            zvalue box = cm_get(defNode, SYM(box));
            switch (symbolEvalType(box)) {
                case EVAL_cell:
                case EVAL_promise: {
                    // Nothing to do.
                    break;
                }
                case EVAL_result: {
                    box = SYM(promise);
                    break;
                }
                default: {
                    die("Bad `box` for `top` variable.");
                    break;
                }
            }
            tops = listAppend(tops,
                makeVarDef(cm_get(defNode, SYM(name)), box, NULL));
        }
    }

    zvalue mains = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(rawStatements, i);
        zvalue defNode = recordEvalTypeIs(s, EVAL_export)
            ? cm_get(s, SYM(value))
            : s;

        if (cm_get(defNode, SYM(top)) != NULL) {
            mains = listAppend(mains,
                makeVarStore(cm_get(defNode, SYM(name)),
                    cm_get(defNode, SYM(value))));
        } else {
            mains = listAppend(mains, s);
        }
    }

    zvalue exports = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(rawStatements, i);

        if (!recordEvalTypeIs(s, EVAL_export)) {
            continue;
        }

        zvalue defNode = cm_get(s, SYM(value));
        if (cm_get(defNode, SYM(top)) == NULL) {
            continue;
        }

        exports = listAppend(exports, cm_get(defNode, SYM(name)));
    };

    zvalue optSelection = (get_size(exports) == 0)
        ? EMPTY_LIST
        : cm_new_List(makeExportSelection(exports));

    return cm_cat(node,
        cm_new_SymbolTable(
            SYM(statements), cm_cat(tops, mains, optSelection)));
}
