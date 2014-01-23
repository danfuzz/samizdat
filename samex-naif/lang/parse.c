/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
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

    zvalue result = seqNth(state->tokens, state->at);
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

    zvalue result = seqNth(state->tokens, state->at);
    zvalue resultType = typeOf(result);

    if (!valEq(type, resultType)) {
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
 * Appends an element to a list.
 */
static zvalue listAppend(zvalue list, zvalue elem) {
    return GFN_CALL(cat, list, listFrom1(elem));
}

/* Documented in spec. */
static zvalue makeInterpolate(zvalue value) {
    return makeTransValue(STR_interpolate, mapFrom1(STR_value, value));
}

/* Documented in spec. */
static zvalue makeLiteral(zvalue value) {
    return makeTransValue(STR_literal, mapFrom1(STR_value, value));
}

/* Documented in spec. */
static zvalue makeThunk(zvalue expression) {
    return makeTransValue(STR_closure,
        mapFrom3(
            STR_formals, EMPTY_LIST,
            STR_statements, EMPTY_LIST,
            STR_yield, expression));
}

/* Documented in spec. */
static zvalue makeVarBind(zvalue name, zvalue value) {
    return makeTransValue(STR_varBind,
        mapFrom2(STR_name, name, STR_value, value));
}

/* Documented in spec. */
static zvalue makeVarDeclare(zvalue name) {
    return makeTransValue(STR_varDeclare, mapFrom1(STR_name, name));
}

/* Documented in spec. */
static zvalue makeVarDef(zvalue name, zvalue value) {
    return makeTransValue(STR_varDef,
        mapFrom2(STR_name, name, STR_value, value));
}

/* Documented in spec. */
static zvalue makeVarRef(zvalue name) {
    return makeTransValue(STR_varRef, mapFrom1(STR_name, name));
}

/* Documented in spec. */
static zvalue makeCall(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_function, function, STR_actuals, actuals);
    return makeTransValue(STR_call, value);
}

/* Documented in spec. */
static zvalue makeCallName(zvalue name, zvalue actuals) {
    return makeCall(makeVarRef(name), actuals);
}

/* Documented in spec. */
static zvalue makeOptValueExpression(zvalue expression) {
    return makeCallName(STR_optValue, listFrom1(makeThunk(expression)));
}

/* Documented in spec. */
static zvalue makeCallNonlocalExit(zvalue name, zvalue optExpression) {
    zvalue actuals;

    if (optExpression != NULL) {
        actuals = listFrom2(name,
            makeInterpolate(makeOptValueExpression(optExpression)));
    } else {
        actuals = listFrom1(name);
    }

    return makeCallName(STR_nonlocalExit, actuals);
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
#define MATCH(typeOf) readMatch(state, (STR_##typeOf))
#define PEEK(typeOf) peekMatch(state, (STR_##typeOf))
#define MARK() zint mark = cursor(state); zvalue tempResult
#define RESET() do { reset(state, mark); } while (0)
#define REJECT() do { RESET(); return NULL; } while (0)
#define REJECT_IF(condition) \
    do { if ((condition)) REJECT(); } while (0)
#define MATCH_OR_REJECT(typeOf) \
    tempResult = MATCH(typeOf); \
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
    REJECT_IF(collSize(result) == 0);

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
 * rules from the Samizdat Layer 0 spec.
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

    return makeTransValue(STR_expression, mapFrom1(STR_value, expression));
}

/* Documented in spec. */
DEF_PARSE(identifier) {
    return MATCH(identifier);
}

/* Documented in spec. */
DEF_PARSE(varRef) {
    MARK();

    zvalue identifier = PARSE_OR_REJECT(identifier);

    return makeVarRef(dataOf(identifier));
}

/* Documented in spec. */
DEF_PARSE(varDef) {
    MARK();

    MATCH_OR_REJECT(def);
    zvalue name = PARSE_OR_REJECT(identifier);

    if (!MATCH(CH_EQUAL)) {
        return makeVarDeclare(dataOf(name));
    }

    zvalue expression = PARSE_OR_REJECT(expression);

    return makeVarDef(dataOf(name), expression);
}

/* Documented in spec. */
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue identifier = PARSE_OR_REJECT(identifier);
    MATCH_OR_REJECT(CH_GT);

    return dataOf(identifier);
}

/* Documented in spec. */
DEF_PARSE(optYieldDef) {
    zvalue result = PARSE(yieldDef);
    return (result != NULL) ? mapFrom1(STR_yieldDef, result) : EMPTY_MAP;
}

/**
 * Helper for `formal`: Parses `[@"?" @"*" @"+"]?`. Returns either the
 * parsed token payload or `NULL` to indicate that no alternate matched.
 */
DEF_PARSE(formal1) {
    MARK();

    zvalue result = NULL;

    if (result == NULL) { result = MATCH(CH_QMARK); }
    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_PLUS); }

    REJECT_IF(result == NULL);

    return typeOf(result);
}

/* Documented in spec. */
DEF_PARSE(formal) {
    MARK();

    zvalue name = PARSE(identifier);

    if (name != NULL) {
        name = dataOf(name);
    } else {
        // If there was no identifier, then the only valid form for a formal
        // is if this is an unnamed / unused argument.
        MATCH_OR_REJECT(CH_DOT);
    }

    zvalue repeat = PARSE(formal1); // Okay for it to be `NULL`.

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

    return makeTransValue(STR_closure, GFN_CALL(cat, declarations, body));
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

    zvalue formals = collGet(dataOf(c), STR_formals);
    if (!valEq(formals, EMPTY_LIST)) {
        die("Invalid formal argument in code block.");
    }

    return c;
}

/* Documented in spec. */
DEF_PARSE(codeOnlyClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(nullaryClosure);

    if (collGet(dataOf(c), STR_yieldDef) != NULL) {
        die("Invalid yield definition in code block.");
    }

    return c;
}

/**
 * Helper for `fnCommon`: Parses `(yieldDef)?` with variable definition
 * and list wrapping.
 */
DEF_PARSE(fnCommon1) {
    zvalue result = PARSE(yieldDef);

    if (result == NULL) {
        return EMPTY_LIST;
    }

    return listFrom1(makeVarDef(result, makeVarRef(STR_return)));
}

/**
 * Helper for `fnCommon`: Parses `(@identifier | [:])` with appropriate map
 * wrapping.
 */
DEF_PARSE(fnCommon2) {
    zvalue n = PARSE(identifier);

    if (n == NULL) {
        return EMPTY_MAP;
    }

    return mapFrom1(STR_name, dataOf(n));
}

/* Documented in spec. */
DEF_PARSE(fnCommon) {
    MARK();

    MATCH_OR_REJECT(fn);

    zvalue returnDef = PARSE(fnCommon1); // This never fails.
    zvalue name = PARSE(fnCommon2); // This never fails.
    MATCH_OR_REJECT(CH_OPAREN);
    zvalue formals = PARSE(formalsList); // This never fails.
    MATCH_OR_REJECT(CH_CPAREN);
    zvalue code = PARSE_OR_REJECT(codeOnlyClosure);

    zvalue codeMap = dataOf(code);
    zvalue statements =
        GFN_CALL(cat, returnDef, collGet(codeMap, STR_statements));
    zvalue closureMap = GFN_CALL(cat,
        codeMap,
        name,
        mapFrom3(
            STR_formals,    formals,
            STR_yieldDef,   STR_return,
            STR_statements, statements));

    return makeTransValue(STR_closure, closureMap);
}

/* Documented in spec. */
DEF_PARSE(fnDef) {
    MARK();

    zvalue closure = PARSE_OR_REJECT(fnCommon);
    zvalue name = collGet(dataOf(closure), STR_name);

    if (name == NULL) {
        return NULL;
    }

    return makeTransValue(STR_topDeclaration,
        mapFrom2(
            STR_top,  makeVarDeclare(name),
            STR_main, makeVarBind(name, closure)));
}

/* Documented in spec. */
DEF_PARSE(fnExpression) {
    MARK();

    zvalue closure = PARSE_OR_REJECT(fnCommon);
    zvalue name = collGet(dataOf(closure), STR_name);

    if (name == NULL) {
        return closure;
    }

    zvalue mainClosure = makeTransValue(
        STR_closure,
        mapFrom3(
            STR_formals,    EMPTY_LIST,
            STR_statements, listFrom1(makeVarDeclare(name)),
            STR_yield,      makeVarBind(name, closure)));

    return makeCall(mainClosure, NULL);
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

    if (result == NULL) { result = MATCH(string); }
    if (result == NULL) { result = PARSE(identifier); }
    if (result == NULL) { result = MATCH(def); }
    if (result == NULL) { result = MATCH(fn); }
    if (result == NULL) { result = MATCH(return); }
    if (result == NULL) { return NULL; }

    zvalue value = dataOf(result);
    if (value == NULL) {
        value = typeOf(result);
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

    if (collSize(keys) == 0) {
        // No keys were specified, so the value must be either a
        // whole-map interpolation or a variable-name-to-its-value
        // binding.
        zvalue type = typeOf(value);
        zvalue data = dataOf(value);

        if (valEq(type, STR_interpolate)) {
            return collGet(data, STR_value);
        } else if (valEq(type, STR_varRef)) {
            return makeCallName(STR_makeValueMap,
                listFrom2(makeLiteral(collGet(data, STR_name)), value));
        }

        REJECT();
    }

    // One or more keys. The `value` is wrapped in an
    // `expression` node here to prevent interpolation from
    // being applied to `makeValueMap`.

    return makeCallName(STR_makeValueMap,
        listAppend(keys,
            makeTransValue(STR_expression, mapFrom1(STR_value, value))));
}

/* Documented in spec. */
DEF_PARSE(map) {
    MARK();

    // This one isn't just a transliteration of the reference code, but the
    // effect is the same.

    MATCH_OR_REJECT(CH_OCURLY);
    zvalue mappings = PARSE_COMMA_SEQ(mapping);
    MATCH_OR_REJECT(CH_CCURLY);

    switch (collSize(mappings)) {
        case 0:  return makeLiteral(EMPTY_MAP);
        case 1:  return seqNth(mappings, 0);
        default: return makeCallName(STR_cat, mappings);
    }
}

/* Documented in spec. */
DEF_PARSE(listItem) {
    MARK();

    if (PARSE(identifierString) && MATCH(CH_COLON)) {
        die("Mapping syntax not valid as a list item or call argument.");
    }

    RESET();

    if (MATCH(CH_AND)) {
        zvalue ex = PARSE(unaryExpression);
        if (ex != NULL) {
            return makeTransValue(STR_voidable, mapFrom1(STR_value, ex));
        }
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

    return (collSize(expressions) == 0)
        ? makeLiteral(EMPTY_LIST)
        : makeCallName(STR_makeList, expressions);
}

/* Documented in spec. */
DEF_PARSE(deriv) {
    MARK();

    MATCH_OR_REJECT(CH_AT);

    zvalue type = PARSE(identifierString);
    if (type == NULL) {
        type = PARSE_OR_REJECT(parenExpression);
    }

    // Value is optional; these are allowed to all fail.
    zvalue value = PARSE(parenExpression);
    if (value == NULL) value = PARSE(map);
    if (value == NULL) value = PARSE(list);

    zvalue args = (value == NULL)
        ? listFrom1(type)
        : listFrom2(type, value);

    return makeCallName(STR_makeValue, args);
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

    zint size = collSize(postfixes);
    for (zint i = 0; i < size; i++) {
        zvalue one = seqNth(postfixes, i);
        if (hasType(one, TYPE_List)) {
            result = makeCall(result, one);
        } else if (valEq(one, TOK_CH_STAR)) {
            result = makeInterpolate(result);
        } else if (valEq(one, TOK_CH_QMARK)) {
            result = makeOptValueExpression(result);
        } else if (hasType(one, STR_literal)) {
            result = makeCallName(STR_get, listFrom2(result, one));
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

    if (!(hasType(base, STR_varRef) && MATCH(CH_COLONEQUAL))) {
        return base;
    }

    // This code isn't parallel to the in-language code but has the
    // same effect, given that the only valid lvalues are variable references.
    // In this case, we ensured (above) that we've got a `varRef` and
    // recombine it here into a `varBind`.
    zvalue ex = PARSE_OR_REJECT(expression);
    zvalue name = collGet(dataOf(base), STR_name);

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
 * Helper for `nonlocalExit`: Parses `@"return"`.
 */
DEF_PARSE(nonlocalExit2) {
    MARK();

    MATCH_OR_REJECT(return);
    return makeVarRef(STR_return);
}

/* Documented in spec. */
DEF_PARSE(nonlocalExit) {
    zvalue name = NULL;

    if (name == NULL) { name = PARSE(nonlocalExit1); }
    if (name == NULL) { name = PARSE(nonlocalExit2); }
    if (name == NULL) { return NULL; }

    zvalue value = PARSE(expression); // It's okay for this to be `NULL`.
    return makeCallNonlocalExit(name, value);
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
    zint size = collSize(rawStatements);

    for (zint i = 0; i < size; i++) {
        zvalue one = seqNth(rawStatements, i);
        if (hasType(one, STR_topDeclaration)) {
            zvalue data = dataOf(one);
            tops = listAppend(tops, collGet(data, STR_top));
            mains = listAppend(mains, collGet(data, STR_main));
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
    MOD_USE(lang);

    zvalue tokens;

    if (hasType(expression, TYPE_String)) {
        tokens = langTokenize0(expression);
    } else {
        tokens = expression;
    }

    ParseState state = { tokens, collSize(tokens), 0 };
    zvalue result = parse_expression(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of expression.");
    }

    return result;
}

/* Documented in header. */
zvalue langParseProgram0(zvalue program) {
    MOD_USE(lang);

    zvalue tokens;

    if (hasType(program, TYPE_String)) {
        tokens = langTokenize0(program);
    } else {
        tokens = program;
    }

    ParseState state = { tokens, collSize(tokens), 0 };
    zvalue result = parse_program(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of program.");
    }

    return result;
}
