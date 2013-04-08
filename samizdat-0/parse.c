/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "parse.h"

#include <stddef.h>

typedef struct {
    /** token list being parsed. */
    zvalue tokens;

    /** Size of `tokens`. */
    zint size;

    /** Current read position. */
    zint at;
}
ParseState;


/*
 * Helper functions
 */

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

    samNote("=== read   at %lld", state->at);

    zvalue result = samListletGet(state->tokens, state->at);
    state->at++;

    return result;
}

/**
 * Reads the next token if its type matches the given token's type.
 */
static zvalue readMatch(ParseState *state, zvalue token) {
    if (isEof(state)) {
        return NULL;
    }

    zvalue result = samListletGet(state->tokens, state->at);
    zvalue tokenType = samMapletGet(token, STR_TYPE);
    zvalue resultType = samMapletGet(result, STR_TYPE);

    if (samCompare(tokenType, resultType) != 0) {
        return NULL;
    }

    samNote("=== read-m at %lld", state->at);
    state->at++;

    return result;
}

/**
 * Gets the current read position.
 */
static zint cursor(ParseState *state) {
    samNote("=== cursor at %lld", state->at);
    return state->at;
}

/**
 * Resets the current read position to the given one.
 */
static void reset(ParseState *state, zint mark) {
    if (mark > state->at) {
        samDie("Cannot reset forward: %lld > %lld", mark, state->at);
    }

    samNote("=== reset  at %lld", mark);
    state->at = mark;
}

/* Defined below. */
static zvalue parseBasicExpression(ParseState *state);
static zvalue parseBasicExpressions(ParseState *state);
static zvalue parseExpression(ParseState *state);
static zvalue parseStatements(ParseState *state);

/**
 * Parses a `call` node.
 */
static zvalue parseCall(ParseState *state) {
    zint mark = cursor(state);

    zvalue function = parseBasicExpression(state);

    if (function == NULL) {
        return NULL;
    }

    // Always succeeds, since a zero-length list is valid.
    zvalue actuals = parseBasicExpressions(state);

    zvalue value = samMapletPut(samMapletEmpty(), STR_FUNCTION, function);
    value = samMapletPut(value, STR_ACTUALS, actuals);
    return valueToken(TOK_CALL, value);
}

/**
 * Parses a `formals` node.
 */
static zvalue parseFormals(ParseState *state) {
    zint mark = cursor(state);
    zvalue identifiers = samListletEmpty();

    for (;;) {
        zvalue identifier = readMatch(state, TOK_IDENTIFIER);
        if (identifier == NULL) {
            break;
        }
        identifier = samMapletGet(identifier, STR_VALUE);
        identifiers = samListletAppend(identifiers, identifier);
    }

    if (samSize(identifiers) != 0) {
        if (readMatch(state, TOK_CH_COLONCOLON) == NULL) {
            reset(state, mark);
            return NULL;
        }
    }

    return valueToken(TOK_FORMALS, identifiers);
}

/**
 * Parses a `function` node.
 */
static zvalue parseFunction(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, TOK_CH_OP_CURLY) == NULL) {
        return NULL;
    }

    // Both of these always succeed.
    zvalue formals = parseFormals(state);
    zvalue statements = parseStatements(state);

    if (readMatch(state, TOK_CH_CL_CURLY) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = samMapletPut(samMapletEmpty(), STR_FORMALS, formals);
    value = samMapletPut(value, STR_STATEMENTS, statements);
    return valueToken(TOK_FUNCTION, value);
}

/**
 * Parses a `uniqlet` node.
 */
static zvalue parseUniqlet(ParseState *state) {
    if (readMatch(state, TOK_CH_ATAT) == NULL) {
        return NULL;
    }

    return TOK_UNIQLET;
}

/**
 * Parses an `emptyMaplet` node.
 */
static zvalue parseEmptyMaplet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, TOK_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, TOK_CH_OP_SQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, TOK_CH_EQUAL) == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, TOK_CH_CL_SQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return valueToken(TOK_LITERAL, samMapletEmpty());
}

/**
 * Parses a `binding` node.
 */
static zvalue parseBinding(ParseState *state) {
    zint mark = cursor(state);
    zvalue key = parseBasicExpression(state);

    if (key == NULL) {
        return NULL;
    }

    if (readMatch(state, TOK_CH_EQUAL) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = parseBasicExpression(state);

    if (value == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue binding = samMapletPut(samMapletEmpty(), STR_KEY, key);
    binding = samMapletPut(binding, STR_VALUE, value);
    return valueToken(TOK_BINDING, binding);
}

/**
 * Parses a `maplet` node.
 */
static zvalue parseMaplet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, TOK_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, TOK_CH_OP_SQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue bindings = samListletEmpty();

    for (;;) {
        zvalue binding = parseBinding(state);

        if (binding == NULL) {
            reset(state, mark);
            return NULL;
        }

        bindings = samListletAppend(bindings, binding);
    }

    if (samSize(bindings) == 0) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, TOK_CH_CL_SQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return valueToken(TOK_MAPLET, bindings);
}

/**
 * Parses a `listlet` node.
 */
static zvalue parseListlet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, TOK_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, TOK_CH_OP_SQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    // Always succeeds, since a zero-length list is valid.
    zvalue expressions = parseBasicExpressions(state);

    if (readMatch(state, TOK_CH_CL_SQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    expressions = samMapletGet(expressions, STR_VALUE);
    return valueToken(TOK_LISTLET, expressions);
}

/**
 * Parses a `stringlet` node.
 */
static zvalue parseStringlet(ParseState *state) {
    zvalue string = readMatch(state, TOK_STRING);

    if (string == NULL) {
        return NULL;
    }

    zvalue value = samMapletGet(string, STR_VALUE);
    return valueToken(TOK_LITERAL, value);
}

/**
 * Parses an `integer` node.
 */
static zvalue parseInteger(ParseState *state) {
    zvalue integer = readMatch(state, TOK_INTEGER);

    if (integer == NULL) {
        return NULL;
    }

    return valueToken(TOK_LITERAL, integer);
}

/**
 * Parses an `intlet` node.
 */
static zvalue parseIntlet(ParseState *state) {
    zvalue integer = readMatch(state, TOK_INTEGER);

    if (integer == NULL) {
        return NULL;
    }

    zvalue value = samMapletGet(integer, STR_VALUE);
    return valueToken(TOK_LITERAL, value);
}

/**
 * Parses a `varRef` node.
 */
static zvalue parseVarRef(ParseState *state) {
    zvalue identifier = readMatch(state, TOK_IDENTIFIER);

    if (identifier == NULL) {
        return NULL;
    }

    zvalue name = samMapletGet(identifier, STR_VALUE);
    return valueToken(TOK_VAR_REF, name);
}

/**
 * Parses a `varDef` node.
 */
static zvalue parseVarDef(ParseState *state) {
    zint mark = cursor(state);
    zvalue identifier = readMatch(state, TOK_IDENTIFIER);

    if (identifier == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, TOK_CH_EQUAL) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue expression = parseExpression(state);

    if (expression == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue name = samMapletGet(identifier, STR_VALUE);
    zvalue value = samMapletPut(samMapletEmpty(), STR_NAME, name);
    value = samMapletPut(value, STR_VALUE, expression);
    return valueToken(TOK_VAR_DEF, value);
}

/**
 * Parses a `parenExpression` node.
 */
static zvalue parseParenExpression(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, TOK_CH_OP_PAREN) == NULL) {
        return NULL;
    }

    zvalue expression = parseCall(state);

    if (expression == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, TOK_CH_CL_PAREN) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return expression;
}

/**
 * Parses a `basicExpression` node.
 */
static zvalue parseBasicExpression(ParseState *state) {
    zvalue result = NULL;

    if (result == NULL) { result = parseVarRef(state); }
    if (result == NULL) { result = parseIntlet(state); }
    if (result == NULL) { result = parseInteger(state); }
    if (result == NULL) { result = parseStringlet(state); }
    if (result == NULL) { result = parseListlet(state); }
    if (result == NULL) { result = parseMaplet(state); }
    if (result == NULL) { result = parseEmptyMaplet(state); }
    if (result == NULL) { result = parseUniqlet(state); }
    if (result == NULL) { result = parseFunction(state); }
    if (result == NULL) { result = parseCall(state); }
    if (result == NULL) { result = parseParenExpression(state); }

    return result;
}

/**
 * Parses a `basicExpressions` node.
 */
static zvalue parseBasicExpressions(ParseState *state) {
    zvalue result = samListletEmpty();

    for (;;) {
        zvalue expression = parseExpression(state);
        if (expression == NULL) {
            break;
        }
        result = samListletAppend(result, expression);
    }

    return valueToken(TOK_EXPRESSIONS, result);
}

/**
 * Parses an `expression` node.
 */
static zvalue parseExpression(ParseState *state) {
    zvalue result = NULL;

    if (result == NULL) { result = parseCall(state); }
    if (result == NULL) { result = parseBasicExpression(state); }

    return result;
}

/**
 * Parses a `return` node.
 */
static zvalue parseReturn(ParseState *state) {
    if (readMatch(state, TOK_CH_CARET) == NULL) {
        return NULL;
    }

    zint mark = cursor(state);
    zvalue expression = parseExpression(state);

    if (expression == NULL) {
        reset(state, mark);
        return NULL;
    }

    return valueToken(TOK_RETURN, expression);
}

/**
 * Parses a `statement` node.
 */
static zvalue parseStatement(ParseState *state) {
    zint mark = cursor(state);
    zvalue result = NULL;

    if (result == NULL) { result = parseVarDef(state); }
    if (result == NULL) { result = parseExpression(state); }
    if (result == NULL) { result = parseReturn(state); }

    if (result == NULL) {
        return NULL;
    }

    if (readMatch(state, TOK_CH_SEMICOLON)) {
        return result;
    } else {
        reset(state, mark);
        return NULL;
    }
}

/**
 * Parses a `statements` node.
 */
static zvalue parseStatements(ParseState *state) {
    zvalue result = samListletEmpty();

    for (;;) {
        zvalue statement = parseStatement(state);
        if (statement == NULL) {
            break;
        }
        result = samListletAppend(result, statement);
    }

    return valueToken(TOK_STATEMENTS, result);
}


/*
 * Exported functions
 */

/** Documented in `parse.h`. */
zvalue parse(zvalue tokens) {
    ParseState state = { tokens, samSize(tokens), 0 };
    zvalue result = parseStatements(&state);

    if (!isEof(&state)) {
        samDie("Extra tokens at end of program.");
    }

    return result;
}
