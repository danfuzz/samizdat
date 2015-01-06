// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Bool.h"
#include "type/Class.h"
#include "type/Cmp.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Record.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "type/String.h"
#include "util.h"

#include "langnode.h"


//
// Private functions
//

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
          || !nodeRecTypeIs(yieldNode, NODE_nonlocalExit)) {
        return yieldNode;
    }

    zvalue function = cm_get(yieldNode, SYM(function));
    zvalue value = cm_get(yieldNode, SYM(value));
    zvalue yieldDef = cm_get(node, SYM(yieldDef));
    zvalue functionTarget = cm_get(function, SYM(target));

    if (     nodeRecTypeIs(function, NODE_fetch)
          && nodeRecTypeIs(functionTarget, NODE_varRef)
          && (yieldDef != NULL)
          && cmpEq(cm_get(functionTarget, SYM(name)), yieldDef)) {
        return value;
    }

    zvalue exitCall;

    switch (nodeRecType(value)) {
        case NODE_void: {
            exitCall = makeFunCall(function, NULL);
            break;
        }
        case NODE_maybe: {
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

        if (!cmpEq(scope, get_name(one))) {
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
        if (nodeRecTypeIs(one, NODE_mapping)) {
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
// Exported functions
//

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
    } else if (nodeRecTypeIs(target, NODE_fetch)) {
        zvalue innerTarget = cm_get(target, SYM(target));
        return cm_new_Record(SYM(store),
            SYM(target), innerTarget, SYM(value), value);
    } else {
        die("Improper `lvalue` binding.");
    }
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
        if (!(cmpEq(one, SYM(access)) || cmpEq(one, SYM(new)))) {
            die("Invalid attribute: %s", cm_debugString(one));
        }
    }

    zvalue accessSecret = cm_get(attribMap, SYM(access));
    if (accessSecret != NULL) {
        accessSecret = cm_new_Record(SYM(mapping),
            SYM(keys),  cm_new_List(SYMS(access)),
            SYM(value), accessSecret);
        accessSecret = cm_new_List(accessSecret);
    } else {
        accessSecret = EMPTY_LIST;
    }

    zvalue newSecret = cm_get(attribMap, SYM(new));
    if (newSecret != NULL) {
        newSecret = cm_new_Record(SYM(mapping),
            SYM(keys),  cm_new_List(SYMS(new)),
            SYM(value), newSecret);
        newSecret = cm_new_List(newSecret);
    } else {
        newSecret = EMPTY_LIST;
    }

    zvalue config = makeSymbolTableExpression(
        cm_cat(accessSecret, newSecret));

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

    switch (nodeRecType(node)) {
        case NODE_importModule: {
            zvalue stat = makeVarDef(name, SYM(result),
                makeFunCall(REFS(loadModule),
                    cm_new_List(makeLiteral(source))));
            return cm_new_List(stat);
        }
        case NODE_importModuleSelection: {
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
        case NODE_importResource: {
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
            cm_new_SymbolTable(
                SYM(formals),    formals,
                SYM(statements), statements,
                SYM(yield),      yieldNode)));
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

        if (cmpEq(select, SYM(CH_STAR))) {
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
        if (nodeRecTypeIs(cm_get(data, SYM(source)), NODE_external)) {
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

        switch (nodeRecType(s)) {
            case NODE_exportSelection: {
                zvalue select = cm_get(s, SYM(select));
                zint sz = get_size(select);
                for (zint j = 0; j < sz; j++) {
                    zvalue name = cm_nth(select, j);
                    exports = addTypeBinding(exports, name);
                }
                break;
            }
            case NODE_export: {
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
        switch (nodeRecType(s)) {
            case NODE_importModule: {
                imports = addImportBinding(imports,
                    cm_get(s, SYM(source)), SYM(module));
                break;
            }
            case NODE_importModuleSelection: {
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
            case NODE_importResource: {
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

    return cm_new_SymbolTable(
        SYM(exports),   exports,
        SYM(imports),   imports,
        SYM(resources), resources);
}

// Documented in spec.
zvalue makeInterpolate(zvalue node) {
    return cm_new_Record(SYM(fetch),
        SYM(target),      node,
        SYM(interpolate), node,
        SYM(lvalue),      EMPTY_LIST);
}

// Documented in spec.
zvalue makeMapExpression(zvalue mappings) {
    return makeMapLikeExpression(mappings, LITS(Map), LITS(EMPTY_MAP));
};

// Documented in spec.
zvalue makeMaybeValue(zvalue expression) {
    zvalue box = cm_get(expression, SYM(box));

    if (box != NULL) {
        return box;
    } else {
        return makeFunCall(METHODS(If, maybeValue),
            cm_new_List(makeThunk(expression)));
    }
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
zvalue withModuleDefs(zvalue node) {
    if (!cmpEqNullOk(cm_get(node, SYM(yield)), TOK_void)) {
        die("Invalid node for `withModuleDefs` (has non-void `yield`).");
    }

    zvalue info = makeInfoTable(node);

    zvalue rawStatements = cm_get(node, SYM(statements));
    zint size = get_size(rawStatements);
    zvalue statements = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(rawStatements, i);

        switch (nodeRecType(s)) {
            case NODE_exportSelection: {
                continue;
            }
            case NODE_export: {
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
        cm_new_SymbolTable(
            SYM(info),       info,
            SYM(statements), statements,
            SYM(yield),      yieldNode));
}

// Documented in spec.
zvalue withTop(zvalue node) {
    return cm_cat(node, cm_new_SymbolTable(SYM(top), BOOL_TRUE));
}

// Documented in spec.
zvalue withoutTops(zvalue node) {
    zvalue rawStatements = cm_get(node, SYM(statements));
    zint size = get_size(rawStatements);

    zvalue tops = EMPTY_LIST;
    for (zint i = 0; i < size; i++) {
        zvalue s = cm_nth(rawStatements, i);
        zvalue defNode = nodeRecTypeIs(s, NODE_export)
            ? cm_get(s, SYM(value))
            : s;

        if (cm_get(defNode, SYM(top)) != NULL) {
            zvalue box = cm_get(defNode, SYM(box));
            switch (nodeSymbolType(box)) {
                case NODE_cell:
                case NODE_promise: {
                    // Nothing to do.
                    break;
                }
                case NODE_result: {
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
        zvalue defNode = nodeRecTypeIs(s, NODE_export)
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

        if (!nodeRecTypeIs(s, NODE_export)) {
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
