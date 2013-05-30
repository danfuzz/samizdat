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
    zvalue resultType = datTokenType(result);

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
 * Constructs a `literal` node.
 */
static zvalue makeLiteral(zvalue value) {
    return datTokenFrom(STR_LITERAL, value);
}

/**
 * Constructs a `varRef` node.
 */
static zvalue makeVarRef(zvalue name) {
    return datTokenFrom(STR_VAR_REF, name);
}

/**
 * Constructs a `call` node.
 */
static zvalue makeCall(zvalue function, zvalue actuals) {
    if (actuals == NULL) {
        actuals = EMPTY_LIST;
    }

    zvalue value = mapFrom2(STR_FUNCTION, function, STR_ACTUALS, actuals);

    return datTokenFrom(STR_CALL, value);
}


/*
 * Parsing functions
 */

/* Definitions to help avoid boilerplate in the parser functions. */
#define RULE(name) parse_##name
#define DEF_PARSE(name) static zvalue RULE(name)(ParseState *state)
#define PARSE(name) RULE(name)(state)
#define PARSE_STAR(name) parseStar(RULE(name), state)
#define PARSE_PLUS(name) parsePlus(RULE(name), state)
#define MATCH(tokenType) readMatch(state, (STR_##tokenType))
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

/* Defined below. */
DEF_PARSE(atom);
DEF_PARSE(expression);
DEF_PARSE(function);

/**
 * Parses `x*` for an arbitrary rule `x`. Returns a list of parsed expressions.
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
 * Parses `x+` for an arbitrary rule `x`. Returns a list of parsed expressions.
 */
zvalue parsePlus(parserFunction rule, ParseState *state) {
    MARK();

    zvalue result = parseStar(rule, state);
    REJECT_IF(datSize(result) == 0);

    return result;
}

/**
 * Parses an `int` node.
 */
DEF_PARSE(int) {
    MARK();

    zvalue intval = MATCH_OR_REJECT(INT);

    return makeLiteral(datTokenValue(intval));
}

/**
 * Parses a `string` node.
 */
DEF_PARSE(string) {
    MARK();

    zvalue string = MATCH_OR_REJECT(STRING);

    return makeLiteral(datTokenValue(string));
}

/**
 * Parses an `emptyMap` node.
 */
DEF_PARSE(emptyMap) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    MATCH_OR_REJECT(CH_EQUAL);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeLiteral(EMPTY_MAP);
}

/**
 * Parses a `mapping` node.
 */
DEF_PARSE(mapping) {
    MARK();

    zvalue key = PARSE_OR_REJECT(atom);
    MATCH_OR_REJECT(CH_EQUAL);
    zvalue value = PARSE_OR_REJECT(atom);

    return datListAppend(datListAppend(EMPTY_LIST, key), value);
}

/**
 * Parses a `map` node.
 */
DEF_PARSE(map) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);

    zvalue mappings = EMPTY_LIST;

    for (;;) {
        zvalue mapping = PARSE(mapping);
        if (mapping == NULL) {
            break;
        }

        mappings = datListAdd(mappings, mapping);
    }

    REJECT_IF(datSize(mappings) == 0);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeCall(makeVarRef(STR_MAKE_MAP), mappings);
}

/**
 * Parses an `emptyList` node.
 */
DEF_PARSE(emptyList) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeLiteral(EMPTY_LIST);
}

/**
 * Parses an `unadornedList` node.
 */
DEF_PARSE(unadornedList) {
    zvalue item = PARSE(expression);

    if (item == NULL) {
        return EMPTY_LIST;
    }

    zvalue result = datListAppend(EMPTY_LIST, item);

    for (;;) {
        MARK();

        if (! MATCH(CH_COMMA)) {
            break;
        }

        item = PARSE(expression);
        if (item == NULL) {
            RESET();
            break;
        }

        result = datListAppend(result, item);
    }

    return result;
}

/**
 * Parses a `list` node.
 */
DEF_PARSE(list) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    zvalue atoms = PARSE_PLUS(atom);
    REJECT_IF(atoms == NULL);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeCall(makeVarRef(STR_MAKE_LIST), atoms);
}

/**
 * Parses a `token` node.
 */
DEF_PARSE(token) {
    MARK();

    zvalue innerType;
    zvalue innerValue;

    MATCH_OR_REJECT(CH_AT);

    innerType = MATCH(STRING);
    if (innerType == NULL) {
        innerType = MATCH(IDENTIFIER);
    }
    if (innerType != NULL) {
        innerType = makeLiteral(datTokenValue(innerType));
        innerValue = NULL;
    }

    if (innerType == NULL) {
        MATCH_OR_REJECT(CH_OSQUARE);
        innerType = PARSE_OR_REJECT(atom);
        innerValue = PARSE(atom); // It's okay for this to be NULL.
        MATCH_OR_REJECT(CH_CSQUARE);
    }

    zvalue args = datListAppend(EMPTY_LIST, innerType);

    if (innerValue != NULL) {
        args = datListAppend(args, innerValue);
    }

    return makeCall(makeVarRef(STR_MAKE_TOKEN), args);
}

/**
 * Parses a `uniqlet` node.
 */
DEF_PARSE(uniqlet) {
    MARK();

    MATCH_OR_REJECT(CH_ATAT);

    return makeCall(makeVarRef(STR_MAKE_UNIQLET), EMPTY_LIST);
}


/**
 * Parses a `varRef` node.
 */
DEF_PARSE(varRef) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);

    return makeVarRef(datTokenValue(identifier));
}

/**
 * Parses a `varDef` node.
 */
DEF_PARSE(varDef) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    MATCH_OR_REJECT(CH_EQUAL);
    zvalue expression = PARSE_OR_REJECT(expression);

    zvalue name = datTokenValue(identifier);
    zvalue value = mapFrom2(STR_NAME, name, STR_VALUE, expression);
    return datTokenFrom(STR_VAR_DEF, value);
}

/**
 * Parses a `parenExpression` node.
 */
DEF_PARSE(parenExpression) {
    MARK();

    MATCH_OR_REJECT(CH_OPAREN);
    zvalue expression = PARSE_OR_REJECT(expression);
    MATCH_OR_REJECT(CH_CPAREN);

    return expression;
}

/**
 * Parses an `atom` node.
 */
DEF_PARSE(atom) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varRef); }
    if (result == NULL) { result = PARSE(int); }
    if (result == NULL) { result = PARSE(string); }
    if (result == NULL) { result = PARSE(emptyList); }
    if (result == NULL) { result = PARSE(list); }
    if (result == NULL) { result = PARSE(emptyMap); }
    if (result == NULL) { result = PARSE(map); }
    if (result == NULL) { result = PARSE(uniqlet); }
    if (result == NULL) { result = PARSE(token); }
    if (result == NULL) { result = PARSE(function); }
    if (result == NULL) { result = PARSE(parenExpression); }

    return result;
}

/**
 * Parses an `actualsList` node.
 */
DEF_PARSE(actualsList) {
    MARK();

    if (MATCH(CH_PARENPAREN)) {
        return PARSE_STAR(function);
    }

    if (MATCH(CH_OPAREN)) {
        zvalue normalActuals = PARSE(unadornedList); // This never fails.
        MATCH_OR_REJECT(CH_CPAREN);
        zvalue functionActuals = PARSE_STAR(function); // This never fails.
        return datListAdd(normalActuals, functionActuals);
    }

    return PARSE_PLUS(function);
}

/**
 * Parses a `callExpression` node.
 */
DEF_PARSE(callExpression) {
    MARK();

    zvalue result = PARSE_OR_REJECT(atom);

    for (;;) {
        zvalue actualsList = PARSE(actualsList);
        if (actualsList == NULL) {
            break;
        }

        result = makeCall(result, actualsList);
    }

    return result;
}

/**
 * Parses an `expression` node.
 */
DEF_PARSE(expression) {
    return PARSE(callExpression);
}

/**
 * Parses a `statement` node.
 */
DEF_PARSE(statement) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varDef); }
    if (result == NULL) { result = PARSE(expression); }

    return result;
}

/**
 * Parses zero or more semicolons. Always returns `NULL`.
 */
DEF_PARSE(optSemicolons) {
    while (MATCH(CH_SEMICOLON) != NULL) /* empty */ ;
    return NULL;
}

/**
 * Parses a `yield` node. Returns `NULL` either if no diamond is present
 * or if it is a void yield.
 */
DEF_PARSE(yield) {
    MARK();

    MATCH_OR_REJECT(CH_DIAMOND);
    return PARSE(expression);
}

/**
 * Parses a `yieldDef` node.
 */
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    MATCH_OR_REJECT(CH_GT);

    return datTokenValue(identifier);
}

/**
 * Parses a `nonlocalExit` node.
 */
DEF_PARSE(nonlocalExit) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue name = PARSE_OR_REJECT(varRef);
    MATCH_OR_REJECT(CH_GT);

    zvalue value = PARSE(expression); // It's okay for this to be `NULL`.
    zvalue actuals =
        (value == NULL) ? NULL : datListAppend(EMPTY_LIST, value);

    return makeCall(name, actuals);
}

/**
 * Helper for `formal`: Parses `(@"?" | @"*" | @"+")?`. Returns either the
 * parsed token payload or `NULL` to indicate that no alternate matched.
 */
DEF_PARSE(formal1) {
    MARK();

    zvalue result = NULL;

    if (result == NULL) { result = MATCH(CH_QMARK); }
    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_PLUS); }

    REJECT_IF(result == NULL);

    return datTokenType(result);
}

/**
 * Parses a `formal` node.
 */
DEF_PARSE(formal) {
    MARK();

    zvalue name = MATCH(IDENTIFIER);

    if (name != NULL) {
        name = datTokenValue(name);
    } else {
        // If there was no identifier, then the only valid form for a formal
        // is if this is an unnamed / unused argument.
        MATCH_OR_REJECT(CH_DOT);
    }

    zvalue repeat = PARSE(formal1); // Okay for it to be `NULL`.

    return mapFrom2(STR_NAME, name, STR_REPEAT, repeat);
}

/**
 * Parses `formal*`.
 */
DEF_PARSE(formalStar) {
    zvalue formals = EMPTY_LIST;

    for (;;) {
        zvalue formal = PARSE(formal);
        if (formal == NULL) {
            break;
        }

        formals = datListAppend(formals, formal);
    }

    return formals;
}

/**
 * Parses a `programBody` node.
 */
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

/**
 * Parses a `programDeclarations` node.
 */
DEF_PARSE(programDeclarations) {
    MARK();

    zvalue formals = PARSE(formalStar);
    zvalue yieldDef = PARSE(yieldDef); // It's okay for this to be `NULL`.

    if (datSize(formals) == 0) {
        // The spec indicates that the "formals" mapping should be omitted
        // when there aren't any formals.
        formals = NULL;
    }

    return mapFrom2(STR_FORMALS, formals, STR_YIELD_DEF, yieldDef);
}

/**
 * Helper for `program`: Parses `(programDeclarations @"::")`.
 */
DEF_PARSE(program1) {
    MARK();

    zvalue result = PARSE(programDeclarations); // This never fails.
    MATCH_OR_REJECT(CH_COLONCOLON);
    return result;
}

/**
 * Parses a `program` node.
 */
DEF_PARSE(program) {
    zvalue declarations = PARSE(program1); // `NULL` is ok, as it's optional.
    zvalue value = PARSE(programBody); // This never fails.

    if (declarations != NULL) {
        value = datMapAdd(value, declarations);
    }

    return datTokenFrom(STR_FUNCTION, value);
}

/**
 * Parses a `function` node.
 */
DEF_PARSE(function) {
    MARK();

    MATCH_OR_REJECT(CH_OCURLY);

    // This always succeeds. See note in `parseProgram` above.
    zvalue result = PARSE(program);

    MATCH_OR_REJECT(CH_CCURLY);

    return result;
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
