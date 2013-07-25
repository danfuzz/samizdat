/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "lang.h"
#include "util.h"

#include <stddef.h>


/*
 * ParseState definition and functions
 */

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

    zvalue result = datListNth(state->tokens, state->at);
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

    zvalue result = datListNth(state->tokens, state->at);
    zvalue resultType = datDerivType(result);

    if (!datEq(type, resultType)) {
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
    zvalue result = EMPTY_MAP;

    if (v1 != NULL) { result = datMapPut(result, k1, v1); }
    if (v2 != NULL) { result = datMapPut(result, k2, v2); }
    if (v3 != NULL) { result = datMapPut(result, k3, v3); }

    return result;
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
    return datListFromArray(1, &e1);
}

/**
 * Makes a 2 element list.
 */
static zvalue listFrom2(zvalue e1, zvalue e2) {
    zvalue elems[2] = { e1, e2 };
    return datListFromArray(2, elems);
}

/**
 * Constructs a `literal` node.
 */
static zvalue makeLiteral(zvalue value) {
    return datDerivFrom(STR_LITERAL, value);
}

/**
 * Constructs a `varDef` node.
 */
static zvalue makeVarDef(zvalue name, zvalue value) {
    zvalue payload = mapFrom2(STR_NAME, name, STR_VALUE, value);
    return datDerivFrom(STR_VAR_DEF, payload);
}

/**
 * Constructs a `varRef` node.
 */
static zvalue makeVarRef(zvalue name) {
    return datDerivFrom(STR_VAR_REF, name);
}

/**
 * Constructs a `call` node.
 */
static zvalue makeCall(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_FUNCTION, function, STR_ACTUALS, actuals);
    return datDerivFrom(STR_CALL, value);
}

/**
 * Constructs a thunk node (function of no arguments), from an expression node.
 */
static zvalue makeThunk(zvalue expression) {
    zvalue value = mapFrom2(STR_STATEMENTS, EMPTY_LIST, STR_YIELD, expression);
    return datDerivFrom(STR_CLOSURE, value);
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
#define MATCH(tokenType) readMatch(state, (STR_##tokenType))
#define PEEK(tokenType) peekMatch(state, (STR_##tokenType))
#define MARK() zint mark = cursor(state); zvalue tempResult
#define RESET() do { reset(state, mark); } while (0)
#define REJECT() do { RESET(); return NULL; } while (0)
#define REJECT_IF(condition) \
    do { if ((condition)) REJECT(); } while (0)
#define MATCH_OR_REJECT(tokenType) \
    tempResult = MATCH(tokenType); \
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

        result = datListAppend(result, one);
    }

    return result;
}

/**
 * Parses `x+` for an arbitrary rule `x`. Returns a list of parsed `x` results.
 */
zvalue parsePlus(parserFunction rule, ParseState *state) {
    MARK();

    zvalue result = parseStar(rule, state);
    REJECT_IF(datSize(result) == 0);

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

        item = rule(state);
        if (item == NULL) {
            RESET();
            break;
        }

        result = datListAppend(result, item);
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

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(programBody);

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(expression);

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(voidableExpression);

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    MATCH_OR_REJECT(CH_GT);

    return datDerivData(identifier);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(optYieldDef) {
    zvalue result = PARSE(yieldDef);
    return (result != NULL) ? mapFrom1(STR_YIELD_DEF, result) : EMPTY_MAP;
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

    return datDerivType(result);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(formal) {
    MARK();

    zvalue name = MATCH(IDENTIFIER);

    if (name != NULL) {
        name = datDerivData(name);
    } else {
        // If there was no identifier, then the only valid form for a formal
        // is if this is an unnamed / unused argument.
        MATCH_OR_REJECT(CH_DOT);
    }

    zvalue repeat = PARSE(formal1); // Okay for it to be `NULL`.

    return mapFrom2(STR_NAME, name, STR_REPEAT, repeat);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(formalsList) {
    zvalue result = PARSE_COMMA_SEQ(formal);

    return (datSize(result) == 0) ? EMPTY_MAP : mapFrom1(STR_FORMALS, result);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(programDeclarations) {
    MARK();

    // Both of these are always maps (possibly empty).
    zvalue yieldDef = PARSE(optYieldDef);
    zvalue formals = PARSE(formalsList);

    if (PEEK(CH_DIAMOND) == NULL) {
        MATCH_OR_REJECT(CH_COLONCOLON);
    }

    return datMapAdd(formals, yieldDef);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(program) {
    zvalue declarations = PARSE(programDeclarations); // `NULL` is ok here.
    zvalue value = PARSE(programBody); // This never fails.

    if (declarations != NULL) {
        value = datMapAdd(value, declarations);
    }

    return datDerivFrom(STR_CLOSURE, value);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(closure) {
    MARK();

    MATCH_OR_REJECT(CH_OCURLY);

    // This always succeeds. See note in `parseProgram` above.
    zvalue result = PARSE(program);

    MATCH_OR_REJECT(CH_CCURLY);

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(nullaryClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(closure);

    if (datMapGet(datDerivData(c), STR_FORMALS) != NULL) {
        die("Invalid formal argument in code block.");
    }

    return c;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(codeOnlyClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(nullaryClosure);

    if (datMapGet(datDerivData(c), STR_YIELD_DEF) != NULL) {
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

    return listFrom1(makeVarDef(result, makeVarRef(STR_RETURN)));
}

/**
 * Helper for `fnCommon`: Parses `(@identifier | [:])` with appropriate map
 * wrapping.
 */
DEF_PARSE(fnCommon2) {
    zvalue n = MATCH(IDENTIFIER);

    if (n == NULL) {
        return EMPTY_MAP;
    }

    return mapFrom1(STR_NAME, datDerivData(n));
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(fnCommon) {
    MARK();

    MATCH_OR_REJECT(FN);

    zvalue returnDef = PARSE(fnCommon1); // This never fails.
    zvalue name = PARSE(fnCommon2); // This never fails.
    MATCH_OR_REJECT(CH_OPAREN);
    zvalue formals = PARSE(formalsList); // This never fails.
    MATCH_OR_REJECT(CH_CPAREN);
    zvalue code = PARSE_OR_REJECT(codeOnlyClosure);

    zvalue codeMap = datDerivData(code);
    zvalue statements =
        datListAdd(returnDef, datMapGet(codeMap, STR_STATEMENTS));

    zvalue result = datMapAdd(codeMap, name);
    result = datMapAdd(result, formals);
    result = datMapAdd(result,
        mapFrom2(STR_YIELD_DEF, STR_RETURN, STR_STATEMENTS, statements));
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(fnDef) {
    MARK();

    zvalue funcMap = PARSE_OR_REJECT(fnCommon);

    if (datMapGet(funcMap, STR_NAME) == NULL) {
        return NULL;
    }

    return datDerivFrom(STR_FN_DEF, funcMap);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(fnExpression) {
    MARK();

    zvalue funcMap = PARSE_OR_REJECT(fnCommon);
    zvalue closure = datDerivFrom(STR_CLOSURE, funcMap);

    zvalue name = datMapGet(funcMap, STR_NAME);
    if (name == NULL) {
        return closure;
    }

    zvalue mainClosure = datDerivFrom(
        STR_CLOSURE,
        mapFrom2(
            STR_STATEMENTS,
            listFrom1(datDerivFrom(STR_FN_DEF, funcMap)),
            STR_YIELD,
            makeVarRef(name)));

    return makeCall(mainClosure, NULL);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(int) {
    MARK();

    zvalue intval = MATCH_OR_REJECT(INT);

    return makeLiteral(datDerivData(intval));
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(string) {
    MARK();

    zvalue string = MATCH_OR_REJECT(STRING);

    return makeLiteral(datDerivData(string));
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(identifierString) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(STRING); }
    if (result == NULL) { result = MATCH(IDENTIFIER); }
    if (result == NULL) { result = MATCH(DEF); }
    if (result == NULL) { result = MATCH(FN); }
    if (result == NULL) { result = MATCH(RETURN); }
    if (result == NULL) { return NULL; }

    zvalue value = datDerivData(result);
    if (value == NULL) {
        value = datDerivType(result);
    }

    return makeLiteral(value);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(unadornedList) {
    return PARSE_COMMA_SEQ(voidableExpression);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(list) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    zvalue expressions = PARSE(unadornedList);
    MATCH_OR_REJECT(CH_CSQUARE);

    if (datSize(expressions) == 0) {
        return makeLiteral(EMPTY_LIST);
    } else {
        return makeCall(makeVarRef(STR_MAKE_LIST), expressions);
    }
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(emptyMap) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    MATCH_OR_REJECT(CH_COLON);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeLiteral(EMPTY_MAP);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(keyAtom) {
    MARK();

    zvalue k = PARSE(identifierString);

    if (k != NULL) {
        if ((PEEK(CH_COLON) != NULL) || (PEEK(CH_CSQUARE) != NULL)) {
            return k;
        }
        RESET();
    }

    return PARSE(expression);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(mapKey) {
    return PARSE(keyAtom);
}

/**
 * Helper for `mapping`: Parses `mapKey @":" expression`.
 */
DEF_PARSE(mapping1) {
    MARK();

    zvalue key = PARSE_OR_REJECT(mapKey);
    MATCH_OR_REJECT(CH_COLON);
    zvalue value = PARSE_OR_REJECT(expression);

    return makeCall(makeVarRef(STR_MAKE_MAPPING),
        listFrom2(key, datDerivFrom(STR_EXPRESSION, value)));
}

/**
 * Helper for `mapping`: Parses a guaranteed-interpolate `expression`.
 */
DEF_PARSE(mapping2) {
    MARK();

    zvalue map = PARSE_OR_REJECT(expression);
    REJECT_IF(!datDerivTypeIs(map, STR_INTERPOLATE));

    return datDerivData(map);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(mapping) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(mapping1); }
    if (result == NULL) { result = PARSE(mapping2); }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(map) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);

    if (MATCH(CH_COLON)) {
        MATCH_OR_REJECT(CH_COMMA);
    }

    zvalue mappings = PARSE_COMMA_SEQ(mapping);
    zint size = datSize(mappings);
    REJECT_IF(size == 0);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeCall(makeVarRef(STR_MAP_ADD), mappings);
}

/**
 * Helper for `token`: Parses `@"[" keyAtom (@":" expression)? @"]"`.
 */
DEF_PARSE(token1) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);

    zvalue type = PARSE_OR_REJECT(keyAtom);
    zvalue result;

    if (MATCH(CH_COLON)) {
        // Note: Strictly speaking this doesn't quite follow the spec.
        // However, there is no meaningful difference, in that the only
        // difference is *how* errors are recognized, not *whether* they
        // are.
        zvalue value = PARSE_OR_REJECT(expression);
        result = listFrom2(type, value);
    } else {
        result = listFrom1(type);
    }

    MATCH_OR_REJECT(CH_CSQUARE);

    return result;
}

/**
 * Helper for `token`: Parses `identifierString` returning a list of the
 * parsed value if successful.
 */
DEF_PARSE(token2) {
    MARK();

    zvalue result = PARSE_OR_REJECT(identifierString);

    return listFrom1(result);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(token) {
    MARK();

    MATCH_OR_REJECT(CH_AT);

    zvalue args = NULL;
    if (args == NULL) { args = PARSE(token1); }
    if (args == NULL) { args = PARSE(token2); }
    REJECT_IF(args == NULL);

    return makeCall(makeVarRef(STR_MAKE_VALUE), args);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(uniqlet) {
    MARK();

    MATCH_OR_REJECT(CH_ATAT);

    return makeCall(makeVarRef(STR_MAKE_UNIQLET), EMPTY_LIST);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(varRef) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);

    return makeVarRef(datDerivData(identifier));
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(varDef) {
    MARK();

    MATCH_OR_REJECT(DEF);
    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    MATCH_OR_REJECT(CH_EQUAL);
    zvalue expression = PARSE_OR_REJECT(expression);

    zvalue name = datDerivData(identifier);
    return makeVarDef(name, expression);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(parenExpression) {
    MARK();

    MATCH_OR_REJECT(CH_OPAREN);
    zvalue expression = PARSE_OR_REJECT(expression);
    MATCH_OR_REJECT(CH_CPAREN);

    return datDerivFrom(STR_EXPRESSION, expression);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(atom) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varRef); }
    if (result == NULL) { result = PARSE(int); }
    if (result == NULL) { result = PARSE(string); }
    if (result == NULL) { result = PARSE(list); }
    if (result == NULL) { result = PARSE(emptyMap); }
    if (result == NULL) { result = PARSE(map); }
    if (result == NULL) { result = PARSE(uniqlet); }
    if (result == NULL) { result = PARSE(token); }
    if (result == NULL) { result = PARSE(closure); }
    if (result == NULL) { result = PARSE(parenExpression); }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(actualsList) {
    MARK();

    if (MATCH(CH_OPAREN)) {
        zvalue normalActuals = PARSE(unadornedList); // This never fails.
        MATCH_OR_REJECT(CH_CPAREN);
        zvalue closureActuals = PARSE_STAR(closure); // This never fails.
        return datListAdd(closureActuals, normalActuals);
    }

    return PARSE_PLUS(closure);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(prefixOperator) {
    // We differ from the spec here, merely returning a token if matched.
    // The corresponding `unaryExpression` code just notes the number of
    // minuses matched in order to make the right number of calls.
    return MATCH(CH_MINUS);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(postfixOperator) {
    // We differ from the spec here, returning an actuals list directly
    // or a `*` token. The corresponding `unaryExpression` code decodes
    // these as appropriate.

    zvalue result = NULL;
    if (result == NULL) { result = PARSE(actualsList); }
    if (result == NULL) { result = MATCH(CH_STAR); }
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(unaryExpression) {
    MARK();

    zvalue prefixes = PARSE_STAR(prefixOperator);
    zvalue result = PARSE_OR_REJECT(atom);
    zvalue postfixes = PARSE_STAR(postfixOperator);

    zint size = datSize(postfixes);
    for (zint i = 0; i < size; i++) {
        zvalue one = datListNth(postfixes, i);
        if (datTypeIs(one, DAT_LIST)) {
            result = makeCall(result, one);
        } else if (datEq(one, TOK_CH_STAR)) {
            result = datDerivFrom(STR_INTERPOLATE, result);
        } else {
            die("Unexpected postfix.");
        }
    }

    for (zint i = datSize(prefixes) - 1; i >= 0; i--) {
        zvalue one = datListNth(prefixes, i);
        if (datEq(one, TOK_CH_MINUS)) {
            result = makeCall(makeVarRef(STR_INEG), listFrom1(result));
        } else {
            die("Unexpected prefix.");
        }
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(voidableExpression) {
    MARK();

    bool voidable = (MATCH(CH_AND) != NULL);
    zvalue ex = PARSE_OR_REJECT(unaryExpression);

    if (voidable) {
        return datDerivFrom(STR_VOIDABLE, ex);
    } else {
        return ex;
    }
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(expression) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(unaryExpression); }
    if (result == NULL) { result = PARSE(fnExpression); }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(statement) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varDef); }
    if (result == NULL) { result = PARSE(fnDef); }
    if (result == NULL) { result = PARSE(expression); }

    return result;
}

/**
 * Documented in Samizdat Layer 0 spec. This implementation differs from the
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

    MATCH_OR_REJECT(RETURN);
    return makeVarRef(STR_RETURN);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(nonlocalExit) {
    zvalue name = NULL;

    if (name == NULL) { name = PARSE(nonlocalExit1); }
    if (name == NULL) { name = PARSE(nonlocalExit2); }
    if (name == NULL) { return NULL; }

    zvalue value = PARSE(expression); // It's okay for this to be `NULL`.
    zvalue actuals = (value == NULL)
        ? listFrom1(name) : listFrom2(name, makeThunk(value));

    return makeCall(makeVarRef(STR_NONLOCAL_EXIT), actuals);
}

/* Documented in Samizdat Layer 0 spec. */
DEF_PARSE(programBody) {
    zvalue statements = EMPTY_LIST;
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
        statements = datListAppend(statements, statement);
    }

    zvalue statement = PARSE(statement);

    if (statement == NULL) {
        statement = PARSE(nonlocalExit);
    }

    if (statement != NULL) {
        statements = datListAppend(statements, statement);
    } else {
        yield = PARSE(yield);
    }

    PARSE(optSemicolons);

    return mapFrom2(STR_STATEMENTS, statements, STR_YIELD, yield);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue langTree0(zvalue program) {
    constInit();

    zvalue tokens;

    if (datTypeIs(program, DAT_STRING)) {
        tokens = langTokenize0(program);
    } else {
        tokens = program;
    }

    ParseState state = { tokens, datSize(tokens), 0 };
    zvalue result = parse_program(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of program.");
    }

    return result;
}
