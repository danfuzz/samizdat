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

    zvalue result = samListletGet(state->tokens, state->at);
    state->at++;

    return result;
}

/**
 * Gets the current read position.
 */
static zint mark(ParseState *state) {
    return state->at;
}

/**
 * Resets the current read position to the given one.
 */
static void reset(ParseState *state, zint mark) {
    if (mark > state->at) {
        samDie("Cannot reset forward: %lld > %lld", mark, state->at);
    }

    state->at = mark;
}

/**
 * Reads the next token if its type matches the given token's type.
 */
static zvalue readMatch(ParseState *state, zvalue token) {
    zint at = mark(state);
    zvalue result = read(state);

    if (result == NULL) {
        return NULL;
    }

    zvalue tokenType = samMapletGet(token, STR_TYPE);
    zvalue resultType = samMapletGet(result, STR_TYPE);

    if (samCompare(tokenType, resultType) != 0) {
        reset(state, at);
        return NULL;
    }

    return result;
}

/* Defined below. */
static zvalue parseExpression(ParseState *state);
static zvalue parseStatements(ParseState *state);

/**
 * Parses a `call` node.
 */
static zvalue parseCall(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `formals` node.
 */
static zvalue parseFormals(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `function` node.
 */
static zvalue parseFunction(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `uniqlet` node.
 */
static zvalue parseUniqlet(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses an `emptyMaplet` node.
 */
static zvalue parseEmptyMaplet(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `binding` node.
 */
static zvalue parseBinding(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `maplet` node.
 */
static zvalue parseMaplet(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `listlet` node.
 */
static zvalue parseListlet(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `stringlet` node.
 */
static zvalue parseStringlet(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses an `integer` node.
 */
static zvalue parseInteger(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses an `intlet` node.
 */
static zvalue parseIntlet(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `varRef` node.
 */
static zvalue parseVarRef(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses a `varDef` node.
 */
static zvalue parseVarDef(ParseState *state) {
    samDie("TODO");
}

/**
 * Parses the `expression` alternate consisting of a parenthesized
 * expression.
 */
static zvalue parseExpression1(ParseState *state) {
    zint at = mark(state);

    if (readMatch(state, TOK_CH_OP_PAREN) == NULL) {
        return NULL;
    }

    zvalue expression = parseExpression(state);

    if (expression == NULL) {
        reset(state, at);
        return NULL;
    }

    if (readMatch(state, TOK_CH_OP_PAREN) == NULL) {
        reset(state, at);
        return NULL;
    }

    return expression;
}

/**
 * Parses an `expression` node.
 */
static zvalue parseExpression(ParseState *state) {
    zvalue result = NULL;

    if (result == NULL) { parseVarRef(state); }
    if (result == NULL) { result = parseIntlet(state); }
    if (result == NULL) { result = parseInteger(state); }
    if (result == NULL) { result = parseStringlet(state); }
    if (result == NULL) { result = parseListlet(state); }
    if (result == NULL) { result = parseMaplet(state); }
    if (result == NULL) { result = parseEmptyMaplet(state); }
    if (result == NULL) { result = parseUniqlet(state); }
    if (result == NULL) { result = parseFunction(state); }
    if (result == NULL) { result = parseCall(state); }
    if (result == NULL) { result = parseExpression1(state); }

    return result;
}

/**
 * Parses an `expressions` node.
 */
static zvalue parseExpressions(ParseState *state) {
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
 * Parses a `return` node.
 */
static zvalue parseReturn(ParseState *state) {
    if (readMatch(state, TOK_CH_CARET) == NULL) {
        return NULL;
    }

    zvalue expression = parseExpression(state);

    if (expression == NULL) {
        return NULL;
    }

    return valueToken(TOK_RETURN, expression);
}

/**
 * Parses a `statement` node.
 */
static zvalue parseStatement(ParseState *state) {
    zint at = mark(state);
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
        reset(state, at);
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
