/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Number.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"


/*
 * ParseState definition and functions
 */

/** State of parsing in-progress. */
typedef struct {
    /** token list being parsed. */
    zvalue tokens;

    /** Size of `tokens`. */
    zint size;

    /** Current read position. */
    zint at;
} ParseState;

/**
 * Is the parse state at EOF?
 */
static bool isEof(ParseState *state) {
    return (state->at >= state->size);
}

/**
 * Reads the next token.
 */
static zvalue read(ParseState *state) {
    if (isEof(state)) {
        return NULL;
    }

    zvalue result = nth(state->tokens, state->at);
    state->at++;

    return result;
}

/**
 * Reads the next token if its type matches the given type.
 */
static zvalue readMatch(ParseState *state, zvalue type) {
    if (isEof(state)) {
        return NULL;
    }

    zvalue result = nth(state->tokens, state->at);

    if (!hasType(result, type)) {
        return NULL;
    }

    state->at++;
    return result;
}

/**
 * Gets the current read position.
 */
static zint cursor(ParseState *state) {
    return state->at;
}

/**
 * Resets the current read position to the given one.
 */
static void reset(ParseState *state, zint mark) {
    if (mark > state->at) {
        die("Cannot reset forward: %lld > %lld", mark, state->at);
    }

    state->at = mark;
}

/**
 * Peeks at the next token, checking against the given type.
 */
static zvalue peekMatch(ParseState *state, zvalue type) {
    zint mark = cursor(state);
    zvalue result = readMatch(state, type);

    if (result == NULL) {
        return NULL;
    }

    reset(state, mark);
    return result;
}


/*
 * Node constructors and related helpers
 */

/**
 * Makes a 0-3 mapping map. Values are allowed to be `NULL`, in
 * which case the corresponding key isn't included in the result.
 */
static zvalue mapFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
                       zvalue k3, zvalue v3) {
    zmapping elems[3];
    zint at = 0;

    if (v1 != NULL) { elems[at].key = k1; elems[at].value = v1; at++; }
    if (v2 != NULL) { elems[at].key = k2; elems[at].value = v2; at++; }
    if (v3 != NULL) { elems[at].key = k3; elems[at].value = v3; at++; }

    return mapFromArray(at, elems);
}

/**
 * Makes a 0-2 mapping map.
 */
static zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return mapFrom3(k1, v1, k2, v2, NULL, NULL);
}

/**
 * Makes a 0-1 mapping map.
 */
static zvalue mapFrom1(zvalue k1, zvalue v1) {
    return mapFrom3(k1, v1, NULL, NULL, NULL, NULL);
}

/**
 * Makes a 1 element list.
 */
static zvalue listFrom1(zvalue e1) {
    return listFromArray(1, &e1);
}

/**
 * Makes a 2 element list.
 */
static zvalue listFrom2(zvalue e1, zvalue e2) {
    zvalue elems[2] = { e1, e2 };
    return listFromArray(2, elems);
}

/**
 * Makes a 3 element list.
 */
static zvalue listFrom3(zvalue e1, zvalue e2, zvalue e3) {
    zvalue elems[3] = { e1, e2, e3 };
    return listFromArray(3, elems);
}

/**
 * Appends an element to a list.
 */
static zvalue listAppend(zvalue list, zvalue elem) {
    return GFN_CALL(cat, list, listFrom1(elem));
}

/** Equivalent to `REFS::<name>` in the spec. */
#define REFS(name) (makeVarRef(STR_##name))

/** Equivalent to `get_<name>(node)` in the spec. */
#define GET(name, node) (get((node), STR_##name))

/* Defined below.*/
static zvalue makeVarRef(zvalue name);

/* Documented in spec. */
static zvalue makeApply(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_function, function, STR_actuals, actuals);
    return makeValue(TYPE_apply, value, NULL);
}

/* Documented in spec. */
static zvalue makeCall(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_function, function, STR_actuals, actuals);
    return makeValue(TYPE_call, value, NULL);
}

/**
 * Documented in spec. This is a fairly direct (but not exact) transliteration
 * of the corresponding code in `Lang0Node`.
 */
static zvalue makeCallOrApply(zvalue function, zvalue actuals) {
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
        zvalue node = GET(interpolate, one);
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
static zvalue makeInterpolate(zvalue node) {
    return makeValue(TYPE_call,
        mapFrom3(
            STR_function,    REFS(interpolate),
            STR_actuals,     listFrom1(node),
            STR_interpolate, node),
        NULL);
}

/* Documented in spec. */
static zvalue makeJump(zvalue function, zvalue optValue) {
    return makeValue(TYPE_jump,
        mapFrom2(STR_function, function, STR_value, optValue),
        NULL);
}

/* Documented in spec. */
static zvalue makeLiteral(zvalue value) {
    return makeValue(TYPE_literal, mapFrom1(STR_value, value), NULL);
}

/* Documented in spec. */
static zvalue makeThunk(zvalue expression) {
    return makeValue(TYPE_closure,
        mapFrom3(
            STR_formals,    EMPTY_LIST,
            STR_statements, EMPTY_LIST,
            STR_yield,      expression),
        NULL);
}

/* Documented in spec. */
static zvalue makeVarBind(zvalue name, zvalue value) {
    return makeValue(TYPE_varBind,
        mapFrom2(STR_name, name, STR_value, value),
        NULL);
}

/* Documented in spec. */
static zvalue makeVarDef(zvalue name, zvalue value) {
    return makeValue(TYPE_varDef,
        mapFrom2(STR_name, name, STR_value, value),
        NULL);
}

/* Documented in spec. */
static zvalue makeVarDefMutable(zvalue name, zvalue value) {
    return makeValue(TYPE_varDefMutable,
        mapFrom2(STR_name, name, STR_value, value),
        NULL);
}

/* Documented in spec. */
static zvalue makeVarRef(zvalue name) {
    return makeValue(TYPE_varRef, mapFrom1(STR_name, name), NULL);
}

/* Documented in spec. */
static zvalue makeOptValue(zvalue expression) {
    return makeCall(REFS(optValue), listFrom1(makeThunk(expression)));
}

/* Documented in spec. */
static zvalue withFormals(zvalue node, zvalue formals) {
    return makeValue(
        get_type(node),
        collPut(dataOf(node), STR_formals, formals),
        NULL);
}

/* Documented in spec. */
static zvalue withoutBind(zvalue node) {
    return makeValue(
        get_type(node),
        collDel(dataOf(node), STR_bind),
        NULL);
}

/* Documented in spec. */
static zvalue withoutInterpolate(zvalue node) {
    return makeValue(
        get_type(node),
        collDel(dataOf(node), STR_interpolate),
        NULL);
}


/*
 * Parsing helper functions
 */

/* Definitions to help avoid boilerplate in the parser functions. */
#define RULE(name) parse_##name
#define DEF_PARSE(name) static zvalue RULE(name)(ParseState *state)
#define PARSE(name) RULE(name)(state)
#define PARSE_STAR(name) parseStar(RULE(name), state)
#define PARSE_PLUS(name) parsePlus(RULE(name), state)
#define PARSE_COMMA_SEQ(name) parseCommaSequence(RULE(name), state)
#define MATCH(type) readMatch(state, (TYPE_##type))
#define PEEK(type) peekMatch(state, (TYPE_##type))
#define MARK() zint mark = cursor(state); zvalue tempResult
#define RESET() do { reset(state, mark); } while (0)
#define REJECT() do { RESET(); return NULL; } while (0)
#define REJECT_IF(condition) \
    do { if ((condition)) REJECT(); } while (0)
#define MATCH_OR_REJECT(type) \
    tempResult = MATCH(type); \
    REJECT_IF(tempResult == NULL)
#define PARSE_OR_REJECT(name) \
    tempResult = PARSE(name); \
    REJECT_IF(tempResult == NULL)

/* Function prototype for all parser functions */
typedef zvalue (*parserFunction)(ParseState *);

/**
 * Parses `x*` for an arbitrary rule `x`. Returns a list of parsed `x` results.
 */
zvalue parseStar(parserFunction rule, ParseState *state) {
    zvalue result = EMPTY_LIST;

    for (;;) {
        zvalue one = rule(state);
        if (one == NULL) {
            break;
        }

        result = listAppend(result, one);
    }

    return result;
}

/**
 * Parses `x+` for an arbitrary rule `x`. Returns a list of parsed `x` results.
 */
zvalue parsePlus(parserFunction rule, ParseState *state) {
    MARK();

    zvalue result = parseStar(rule, state);
    REJECT_IF(get_size(result) == 0);

    return result;
}

/**
 * Parses `(x (@"," x)*)?` for an arbitrary rule `x`. Returns a list of
 * parsed `x` results.
 */
zvalue parseCommaSequence(parserFunction rule, ParseState *state) {
    zvalue item = rule(state);

    if (item == NULL) {
        return EMPTY_LIST;
    }

    zvalue result = listFrom1(item);

    for (;;) {
        MARK();

        if (! MATCH(CH_COMMA)) {
            break;
        }

        zstackPointer save = datFrameStart();
        item = rule(state);

        datFrameReturn(save, item);

        if (item == NULL) {
            RESET();
            break;
        }

        result = listAppend(result, item);
    }

    return result;
}

/**
 * Parses zero or more semicolons. Always returns `NULL`.
 */
DEF_PARSE(optSemicolons) {
    while (MATCH(CH_SEMICOLON) != NULL) /* empty */ ;
    return NULL;
}


/*
 * Samizdat 0 Tree Grammar
 *
 * The following is a near-direct transliterations of the tree syntax
 * rules from the spec.
 */

/* Documented in spec. */
DEF_PARSE(assignExpression);
DEF_PARSE(fnExpression);
DEF_PARSE(opExpression);
DEF_PARSE(programBody);
DEF_PARSE(unaryExpression);

/* Documented in spec. */
DEF_PARSE(expression) {
    zstackPointer save = datFrameStart();
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(assignExpression); }
    if (result == NULL) { result = PARSE(fnExpression); }

    datFrameReturn(save, result);
    return result;
}

/* Documented in spec. */
DEF_PARSE(parenExpression) {
    MARK();

    MATCH_OR_REJECT(CH_OPAREN);
    zvalue expression = PARSE_OR_REJECT(expression);

    if (MATCH(CH_COMMA)) {
        die("Comma not allowed within parenthesized expression.");
    }

    MATCH_OR_REJECT(CH_CPAREN);

    return withoutInterpolate(expression);
}

/* Documented in spec. */
DEF_PARSE(int) {
    MARK();

    zvalue neg = MATCH(CH_MINUS);
    zvalue intToken = MATCH_OR_REJECT(int);

    zvalue value = dataOf(intToken);
    if (neg != NULL) {
        value = GFN_CALL(neg, value);
    }

    return makeLiteral(value);
}

/* Documented in spec. */
DEF_PARSE(string) {
    MARK();

    zvalue string = MATCH_OR_REJECT(string);

    return makeLiteral(dataOf(string));
}

/* Documented in spec. */
DEF_PARSE(identifierString) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(string);     }
    if (result == NULL) { result = MATCH(identifier); }
    if (result == NULL) { result = MATCH(break);      }
    if (result == NULL) { result = MATCH(continue);   }
    if (result == NULL) { result = MATCH(def);        }
    if (result == NULL) { result = MATCH(fn);         }
    if (result == NULL) { result = MATCH(return);     }
    if (result == NULL) { result = MATCH(var);        }
    if (result == NULL) { return NULL; }

    zvalue value = dataOf(result);
    if (value == NULL) {
        value = typeName(get_type(result));
    }

    return makeLiteral(value);
}

/**
 * Helper for `key`: Parses `identifierString @":"`.
 */
DEF_PARSE(key1) {
    MARK();

    zvalue result = PARSE_OR_REJECT(identifierString);
    MATCH_OR_REJECT(CH_COLON);

    return result;
}

/**
 * Helper for `key`: Parses `expression @":"`.
 */
DEF_PARSE(key2) {
    MARK();

    zvalue result = PARSE_OR_REJECT(expression);
    MATCH_OR_REJECT(CH_COLON);

    return result;
}

/* Documented in spec. */
DEF_PARSE(key) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(key1); }
    if (result == NULL) { result = PARSE(key2); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(mapping) {
    MARK();

    zvalue keys = PARSE_STAR(key);
    zvalue value = PARSE_OR_REJECT(expression);

    if (get_size(keys) == 0) {
        // No keys were specified, so the value must be either a
        // whole-map interpolation or a variable-name-to-its-value
        // binding.
        zvalue interp = GET(interpolate, value);
        if (interp != NULL) {
            return interp;
        } else if (hasType(value, TYPE_varRef)) {
            return makeCall(REFS(makeValueMap),
                listFrom2(makeLiteral(GET(name, value)), value));
        }

        REJECT();
    }

    // One or more keys.
    return makeCallOrApply(REFS(makeValueMap),
        listAppend(keys, withoutInterpolate(value)));
}

/* Documented in spec. */
DEF_PARSE(map) {
    MARK();

    // This one isn't just a transliteration of the reference code, but the
    // effect is the same.

    MATCH_OR_REJECT(CH_OCURLY);
    zvalue mappings = PARSE_COMMA_SEQ(mapping);
    MATCH_OR_REJECT(CH_CCURLY);

    switch (get_size(mappings)) {
        case 0:  return makeLiteral(EMPTY_MAP);
        case 1:  return nth(mappings, 0);
        default: return makeCall(REFS(cat), mappings);
    }
}

/* Documented in spec. */
DEF_PARSE(listItem) {
    MARK();

    if (PARSE(identifierString) && MATCH(CH_COLON)) {
        die("Mapping syntax not valid as a list item or call argument.");
    }

    RESET();

    return PARSE(expression);
}

/* Documented in spec. */
DEF_PARSE(unadornedList) {
    return PARSE_COMMA_SEQ(listItem);
}

/* Documented in spec. */
DEF_PARSE(list) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    zvalue expressions = PARSE(unadornedList);
    MATCH_OR_REJECT(CH_CSQUARE);

    return (get_size(expressions) == 0)
        ? makeLiteral(EMPTY_LIST)
        : makeCallOrApply(REFS(makeList), expressions);
}

/* Documented in spec. */
DEF_PARSE(typeName) {
    MARK();

    zvalue name = PARSE(identifierString);
    if (name == NULL) { name = PARSE_OR_REJECT(parenExpression); }

    if (hasType(name, TYPE_literal)) {
        return makeLiteral(makeDerivedDataType(GET(value, name)));
    } else {
        return makeCall(REFS(makeDerivedDataType), listFrom1(name));
    }
}

/* Documented in spec. */
DEF_PARSE(type) {
    MARK();

    MATCH_OR_REJECT(CH_ATAT);
    return PARSE_OR_REJECT(typeName);
}

/* Documented in spec. */
DEF_PARSE(deriv) {
    MARK();

    MATCH_OR_REJECT(CH_AT);

    zvalue type = PARSE_OR_REJECT(typeName);

    // Value is optional; these are allowed to all fail.
    zvalue value = PARSE(parenExpression);
    if (value == NULL) value = PARSE(map);
    if (value == NULL) value = PARSE(list);

    zvalue args = (value == NULL) ? listFrom1(type) : listFrom2(type, value);

    return makeCall(REFS(makeValue), args);
}

/* Documented in spec. */
DEF_PARSE(varRef) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(identifier);

    return makeVarRef(dataOf(identifier));
}

/* Documented in spec. */
DEF_PARSE(varDef) {
    MARK();

    bool isMutable;

    if (MATCH(def)) {
        isMutable = false;
    } else {
        MATCH_OR_REJECT(var);
        isMutable = true;
    }

    zvalue name = MATCH_OR_REJECT(identifier);

    zvalue expr;
    if (MATCH(CH_EQUAL)) {
        expr = PARSE_OR_REJECT(expression);
    } else {
        expr = NULL;
    }

    if (isMutable) {
        return makeVarDefMutable(dataOf(name), expr);
    } else {
        return makeVarDef(dataOf(name), expr);
    }
}

/* Documented in spec. */
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue identifier = MATCH_OR_REJECT(identifier);
    MATCH_OR_REJECT(CH_GT);

    return dataOf(identifier);
}

/* Documented in spec. */
DEF_PARSE(optYieldDef) {
    zvalue result = PARSE(yieldDef);
    return (result != NULL) ? mapFrom1(STR_yieldDef, result) : EMPTY_MAP;
}

/**
 * Helper for `formal`: Parses `[@"?" @"*" @"+"]?`. Returns the parsed token,
 * or `NULL` to indicate that no alternate matched.
 */
DEF_PARSE(formal1) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(CH_QMARK); }
    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_PLUS); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(formal) {
    MARK();

    zvalue name = MATCH(identifier);

    if (name != NULL) {
        name = dataOf(name);
    } else {
        // If there was no identifier, then the only valid form for a formal
        // is if this is an unnamed / unused argument.
        MATCH_OR_REJECT(CH_DOT);
    }

    zvalue repeat = PARSE(formal1); // Okay for it to be `NULL`.
    if (repeat != NULL) {
        repeat = typeName(get_type(repeat));
    }

    return mapFrom2(STR_name, name, STR_repeat, repeat);
}

/* Documented in spec. */
DEF_PARSE(formalsList) {
    return PARSE_COMMA_SEQ(formal);
}

/**
 * Helper for `programDeclarations`: Parses the main part of the syntax.
 */
DEF_PARSE(programDeclarations1) {
    MARK();

    // Both of these are always maps (possibly empty).
    zvalue yieldDef = PARSE(optYieldDef);
    zvalue formals = PARSE(formalsList);

    if (PEEK(CH_DIAMOND) == NULL) {
        MATCH_OR_REJECT(CH_RARROW);
    }

    return GFN_CALL(cat, mapFrom1(STR_formals, formals), yieldDef);
}

/* Documented in spec. */
DEF_PARSE(programDeclarations) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(programDeclarations1); }
    if (result == NULL) { result = mapFrom1(STR_formals, EMPTY_LIST); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(program) {
    zvalue declarations = PARSE(programDeclarations); // This never fails.
    zvalue body = PARSE(programBody); // This never fails.

    return makeValue(TYPE_closure,
        GFN_CALL(cat, declarations, body),
        NULL);
}

/* Documented in spec. */
DEF_PARSE(closure) {
    MARK();

    MATCH_OR_REJECT(CH_OCURLY);

    // This always succeeds. See note in `parseProgram` above.
    zvalue result = PARSE(program);

    MATCH_OR_REJECT(CH_CCURLY);

    return result;
}

/* Documented in spec. */
DEF_PARSE(nullaryClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(closure);

    zvalue formals = GET(formals, c);
    if (!valEq(formals, EMPTY_LIST)) {
        die("Invalid formal argument in code block.");
    }

    return c;
}

/**
 * Helper for `fnCommon`: Parses the first `name` alternate.
 */
DEF_PARSE(fnCommon1) {
    MARK();

    zvalue n = MATCH_OR_REJECT(identifier);
    REJECT_IF(MATCH(CH_DOT));

    return mapFrom1(STR_name, dataOf(n));
}

/**
 * Helper for `fnCommon`: Parses the second `name` alternate.
 */
DEF_PARSE(fnCommon2) {
    MARK();

    zvalue bind = NULL;
    if (bind == NULL) { bind = PARSE(varRef); }
    if (bind == NULL) { bind = PARSE(type); }
    REJECT_IF(bind == NULL);

    MATCH_OR_REJECT(CH_DOT);
    zvalue n = PARSE_OR_REJECT(identifierString);

    return mapFrom2(STR_bind, bind, STR_name, GET(value, n));
}

/**
 * Helper for `fnCommon`: Parses the `name` sub-expression.
 */
DEF_PARSE(fnCommon3) {
    zvalue name = NULL;

    if (name == NULL) { name = PARSE(fnCommon1); }
    if (name == NULL) { name = PARSE(fnCommon2); }
    if (name == NULL) { name = EMPTY_MAP;        }

    return name;
}

/* Documented in spec. */
DEF_PARSE(fnCommon) {
    MARK();

    MATCH_OR_REJECT(fn);

    zvalue name = PARSE(fnCommon3); // This never fails.
    MATCH_OR_REJECT(CH_OPAREN);
    zvalue formals = PARSE(formalsList); // This never fails.
    MATCH_OR_REJECT(CH_CPAREN);
    zvalue code = PARSE_OR_REJECT(nullaryClosure);

    zvalue yieldDef = GET(yieldDef, code);
    zvalue returnDef = (yieldDef == NULL)
        ? EMPTY_LIST
        : listFrom1(makeVarDef(yieldDef, REFS(return)));
    zvalue statements = GFN_CALL(cat, returnDef, GET(statements, code));
    zvalue closureMap = GFN_CALL(cat,
        dataOf(code),
        name,
        mapFrom3(
            STR_formals,    formals,
            STR_yieldDef,   STR_return,
            STR_statements, statements));

    return makeValue(TYPE_closure, closureMap, NULL);
}

/* Documented in spec. */
DEF_PARSE(fnDef) {
    MARK();

    zvalue closure = PARSE_OR_REJECT(fnCommon);
    zvalue name = GET(name, closure);

    if (name == NULL) {
        return NULL;
    }

    zvalue bind = GET(bind, closure);

    if (bind != NULL) {
        zvalue formals = GFN_CALL(cat,
            listFrom1(mapFrom1(STR_name, STR_this)),
            GET(formals, closure));
        zvalue finalClosure = withFormals(withoutBind(closure), formals);
        return makeCall(REFS(genericBind),
            listFrom3(makeVarRef(name), bind, finalClosure));
    } else {
        return makeValue(TYPE_topDeclaration,
            mapFrom2(
                STR_top,  makeVarDef(name, NULL),
                STR_main, makeVarBind(name, closure)),
            NULL);
    }
}

/* Documented in spec. */
DEF_PARSE(fnExpression) {
    MARK();

    zvalue closure = PARSE_OR_REJECT(fnCommon);
    REJECT_IF(GET(bind, closure) != NULL);
    zvalue name = GET(name, closure);

    if (name == NULL) {
        return closure;
    }

    zvalue mainClosure = makeValue(TYPE_closure,
        mapFrom3(
            STR_formals,    EMPTY_LIST,
            STR_statements, listFrom1(makeVarDef(name, NULL)),
            STR_yield,      makeVarBind(name, closure)),
        NULL);

    return makeCall(mainClosure, NULL);
}

/* Documented in spec. */
DEF_PARSE(term) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varRef); }
    if (result == NULL) { result = PARSE(int); }
    if (result == NULL) { result = PARSE(string); }
    if (result == NULL) { result = PARSE(map); }
    if (result == NULL) { result = PARSE(list); }
    if (result == NULL) { result = PARSE(deriv); }
    if (result == NULL) { result = PARSE(type); }
    if (result == NULL) { result = PARSE(closure); }
    if (result == NULL) { result = PARSE(parenExpression); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(actualsList) {
    MARK();

    if (MATCH(CH_OPAREN)) {
        zvalue normalActuals = PARSE(unadornedList); // This never fails.
        MATCH_OR_REJECT(CH_CPAREN);
        zvalue closureActuals = PARSE_STAR(closure); // This never fails.
        return GFN_CALL(cat, closureActuals, normalActuals);
    }

    return PARSE_PLUS(closure);
}

/* Documented in spec. */
DEF_PARSE(postfixOperator) {
    // We differ from the spec here, returning a payload or single token
    // (`*` or `?`) directly. The corresponding `unaryExpression` code
    // decodes these as appropriate.

    MARK();

    zvalue result = NULL;

    if (result == NULL) { result = PARSE(actualsList); }

    if ((result == NULL) && (MATCH(CH_COLONCOLON) != NULL)) {
        result = PARSE_OR_REJECT(identifierString);
    }

    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_QMARK); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(unaryExpression) {
    MARK();

    zvalue result = PARSE_OR_REJECT(term);
    zvalue postfixes = PARSE_STAR(postfixOperator);

    zint size = get_size(postfixes);
    for (zint i = 0; i < size; i++) {
        zvalue one = nth(postfixes, i);
        if (hasType(one, TYPE_List)) {
            result = makeCallOrApply(result, one);
        } else if (valEq(one, TOK_CH_STAR)) {
            result = makeInterpolate(result);
        } else if (valEq(one, TOK_CH_QMARK)) {
            result = makeOptValue(result);
        } else if (hasType(one, TYPE_literal)) {
            result = makeCallOrApply(REFS(get), listFrom2(result, one));
        } else {
            die("Unexpected postfix.");
        }
    }

    return result;
}

/* Documented in spec. */
DEF_PARSE(opExpression) {
    return PARSE(unaryExpression);
}

/* Documented in spec. */
DEF_PARSE(assignExpression) {
    MARK();

    zvalue base = PARSE_OR_REJECT(opExpression);

    if (!(hasType(base, TYPE_varRef) && MATCH(CH_COLONEQUAL))) {
        return base;
    }

    // This code isn't parallel to the in-language code but has the
    // same effect, given that the only valid lvalues are variable references.
    // In this case, we ensured (above) that we've got a `varRef` and
    // recombine it here into a `varBind`.
    zvalue ex = PARSE_OR_REJECT(expression);
    zvalue name = GET(name, base);

    return makeVarBind(name, ex);
}

/* Documented in spec. */
DEF_PARSE(statement) {
    zstackPointer save = datFrameStart();
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varDef); }
    if (result == NULL) { result = PARSE(fnDef); }
    if (result == NULL) { result = PARSE(expression); }

    datFrameReturn(save, result);
    return result;
}

/**
 * Documented in spec. This implementation differs from the
 * spec in that it will return `NULL` either if no diamond is present
 * or if it is a void yield. This is compensated for by matching changes to
 * the implementation of `programBody`, below.
 */
DEF_PARSE(yield) {
    MARK();

    MATCH_OR_REJECT(CH_DIAMOND);
    return PARSE(expression);
}

/**
 * Helper for `nonlocalExit`: Parses `@"<" varRef @">"`.
 */
DEF_PARSE(nonlocalExit1) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue name = PARSE_OR_REJECT(varRef);
    MATCH_OR_REJECT(CH_GT);

    return name;
}

/**
 * Helper for `nonlocalExit`: Parses `@break | @continue | @return`.
 */
DEF_PARSE(nonlocalExit2) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(break); }
    if (result == NULL) { result = MATCH(continue); }
    if (result == NULL) { result = MATCH(return); }
    if (result == NULL) { return NULL; }

    return makeVarRef(typeName(get_type(result)));
}

/* Documented in spec. */
DEF_PARSE(nonlocalExit) {
    zvalue name = NULL;

    if (name == NULL) { name = PARSE(nonlocalExit1); }
    if (name == NULL) { name = PARSE(nonlocalExit2); }
    if (name == NULL) { return NULL; }

    zvalue optValue = PARSE(expression); // It's okay for this to be `NULL`.
    return makeJump(name, optValue);
}

/* Documented in spec. */
DEF_PARSE(programBody) {
    zvalue rawStatements = EMPTY_LIST;
    zvalue yield = NULL; // `NULL` is ok, as it's optional.

    PARSE(optSemicolons);

    for (;;) {
        MARK();

        zvalue statement = PARSE(statement);
        if (statement == NULL) {
            break;
        }

        if (MATCH(CH_SEMICOLON) == NULL) {
            RESET();
            break;
        }

        PARSE(optSemicolons);
        rawStatements = listAppend(rawStatements, statement);
    }

    zvalue statement = PARSE(statement);

    if (statement == NULL) {
        statement = PARSE(nonlocalExit);
    }

    if (statement != NULL) {
        rawStatements = listAppend(rawStatements, statement);
    } else {
        yield = PARSE(yield);
    }

    PARSE(optSemicolons);

    zvalue tops = EMPTY_LIST;
    zvalue mains = EMPTY_LIST;
    zint size = get_size(rawStatements);

    for (zint i = 0; i < size; i++) {
        zvalue one = nth(rawStatements, i);
        if (hasType(one, TYPE_topDeclaration)) {
            tops = listAppend(tops, get(one, STR_top));
            mains = listAppend(mains, get(one, STR_main));
        } else {
            mains = listAppend(mains, one);
        }
    }

    zvalue statements = GFN_CALL(cat, tops, mains);

    return mapFrom2(STR_statements, statements, STR_yield, yield);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue langParseExpression0(zvalue expression) {
    zvalue tokens;

    if (hasType(expression, TYPE_String)) {
        tokens = langTokenize0(expression);
    } else {
        tokens = expression;
    }

    ParseState state = { tokens, get_size(tokens), 0 };
    zvalue result = parse_expression(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of expression.");
    }

    return result;
}

/* Documented in header. */
zvalue langParseProgram0(zvalue program) {
    zvalue tokens;

    if (hasType(program, TYPE_String)) {
        tokens = langTokenize0(program);
    } else {
        tokens = program;
    }

    ParseState state = { tokens, get_size(tokens), 0 };
    zvalue result = parse_program(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of program.");
    }

    return result;
}
