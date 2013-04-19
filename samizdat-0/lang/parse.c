/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>

typedef struct {
    /** token list being parsed. */
    zvalue tokens;

    /** Size of `tokens`. */
    zint size;

    /** Current read position. */
    zint at;
} ParseState;


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

    zvalue result = datListletGet(state->tokens, state->at);
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

    zvalue result = datListletGet(state->tokens, state->at);
    zvalue resultType = datHighletType(result);

    if (datOrder(type, resultType) != 0) {
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

/* Defined below. */
static zvalue parseAtom(ParseState *state);
static zvalue parseExpression(ParseState *state);
static zvalue parseFunction(ParseState *state);

/**
 * Parses `atom+`. Returns a listlet of parsed expressions.
 */
static zvalue parseAtomPlus(ParseState *state) {
    zvalue result = datListletEmpty();

    for (;;) {
        zvalue atom = parseAtom(state);
        if (atom == NULL) {
            break;
        }
        result = datListletAppend(result, atom);
    }

    if (datSize(result) == 0) {
        return NULL;
    }

    return result;
}

/**
 * Parses `@"(" @")"` as part of parsing a `call` node.
 */
static zvalue parseCall1(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_OPAREN) == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_CPAREN) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return datListletEmpty();
}

/**
 * Parses a `call` node.
 */
static zvalue parseCall(ParseState *state) {
    zint mark = cursor(state);
    zvalue function = parseAtom(state);

    if (function == NULL) {
        return NULL;
    }

    zvalue actuals = parseCall1(state);

    if (actuals == NULL) {
        actuals = parseAtomPlus(state);
    }

    if (actuals == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = datMapletPut(datMapletEmpty(), STR_FUNCTION, function);
    value = datMapletPut(value, STR_ACTUALS, actuals);
    return datHighletFrom(STR_CALL, value);
}

/**
 * Parses a `highlet` node.
 */
static zvalue parseHighlet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_OSQUARE) == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_COLON) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue innerType = parseAtom(state);
    if (innerType == NULL) {
        reset(state, mark);
        return NULL;
    }

    // It's okay for this to be NULL.
    zvalue innerValue = parseAtom(state);

    if (readMatch(state, STR_CH_COLON) == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_CSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = datMapletPut(datMapletEmpty(), STR_TYPE, innerType);

    if (innerValue != NULL) {
        value = datMapletPut(value, STR_VALUE, innerValue);
    }

    return datHighletFrom(STR_HIGHLET, value);
}

/**
 * Parses a `uniqlet` node.
 */
static zvalue parseUniqlet(ParseState *state) {
    if (readMatch(state, STR_CH_ATAT) == NULL) {
        return NULL;
    }

    return TOK_UNIQLET;
}

/**
 * Parses a `binding` node.
 */
static zvalue parseBinding(ParseState *state) {
    zint mark = cursor(state);
    zvalue key = parseAtom(state);

    if (key == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_EQUAL) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = parseAtom(state);

    if (value == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue binding = datMapletPut(datMapletEmpty(), STR_KEY, key);
    return datMapletPut(binding, STR_VALUE, value);
}

/**
 * Parses a `maplet` node.
 */
static zvalue parseMaplet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_OSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue bindings = datListletEmpty();

    for (;;) {
        zvalue binding = parseBinding(state);

        if (binding == NULL) {
            break;
        }

        bindings = datListletAppend(bindings, binding);
    }

    if (datSize(bindings) == 0) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_CSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return datHighletFrom(STR_MAPLET, bindings);
}

/**
 * Parses an `emptyMaplet` node.
 */
static zvalue parseEmptyMaplet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_OSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_EQUAL) == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_CSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return datHighletFrom(STR_LITERAL, datMapletEmpty());
}

/**
 * Parses a `listlet` node.
 */
static zvalue parseListlet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_OSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue atoms = parseAtomPlus(state);

    if (atoms == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_CSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return datHighletFrom(STR_LISTLET, atoms);
}

/**
 * Parses an `emptyListlet` node.
 */
static zvalue parseEmptyListlet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_AT) == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_OSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_CSQUARE) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return datHighletFrom(STR_LITERAL, datListletEmpty());
}

/**
 * Parses a `stringlet` node.
 */
static zvalue parseStringlet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_AT) == NULL) {
        return NULL;
    }

    zvalue string = readMatch(state, STR_STRING);

    if (string == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = datHighletValue(string);
    return datHighletFrom(STR_LITERAL, value);
}

/**
 * Parses an `integer` node.
 */
static zvalue parseInteger(ParseState *state) {
    zvalue integer = readMatch(state, STR_INTEGER);

    if (integer == NULL) {
        return NULL;
    }

    return datHighletFrom(STR_LITERAL, integer);
}

/**
 * Parses an `intlet` node.
 */
static zvalue parseIntlet(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_AT) == NULL) {
        return NULL;
    }

    zvalue integer = readMatch(state, STR_INTEGER);

    if (integer == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue value = datHighletValue(integer);
    return datHighletFrom(STR_LITERAL, value);
}

/**
 * Parses a `varRef` node.
 */
static zvalue parseVarRef(ParseState *state) {
    zvalue identifier = readMatch(state, STR_IDENTIFIER);

    if (identifier == NULL) {
        return NULL;
    }

    zvalue name = datHighletValue(identifier);
    return datHighletFrom(STR_VAR_REF, name);
}

/**
 * Parses a `varDef` node.
 */
static zvalue parseVarDef(ParseState *state) {
    zint mark = cursor(state);
    zvalue identifier = readMatch(state, STR_IDENTIFIER);

    if (identifier == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_EQUAL) == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue expression = parseExpression(state);

    if (expression == NULL) {
        reset(state, mark);
        return NULL;
    }

    zvalue name = datHighletValue(identifier);
    zvalue value = datMapletPut(datMapletEmpty(), STR_NAME, name);
    value = datMapletPut(value, STR_VALUE, expression);
    return datHighletFrom(STR_VAR_DEF, value);
}

/**
 * Parses a `parenExpression` node.
 */
static zvalue parseParenExpression(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_OPAREN) == NULL) {
        return NULL;
    }

    zvalue expression = parseExpression(state);

    if (expression == NULL) {
        reset(state, mark);
        return NULL;
    }

    if (readMatch(state, STR_CH_CPAREN) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return expression;
}

/**
 * Parses an `atom` node.
 */
static zvalue parseAtom(ParseState *state) {
    zvalue result = NULL;

    if (result == NULL) { result = parseVarRef(state); }
    if (result == NULL) { result = parseIntlet(state); }
    if (result == NULL) { result = parseInteger(state); }
    if (result == NULL) { result = parseStringlet(state); }
    if (result == NULL) { result = parseEmptyListlet(state); }
    if (result == NULL) { result = parseListlet(state); }
    if (result == NULL) { result = parseEmptyMaplet(state); }
    if (result == NULL) { result = parseMaplet(state); }
    if (result == NULL) { result = parseUniqlet(state); }
    if (result == NULL) { result = parseHighlet(state); }
    if (result == NULL) { result = parseFunction(state); }
    if (result == NULL) { result = parseParenExpression(state); }

    return result;
}

/**
 * Parses an `expression` node.
 */
static zvalue parseExpression(ParseState *state) {
    zvalue result = NULL;

    if (result == NULL) { result = parseCall(state); }
    if (result == NULL) { result = parseAtom(state); }

    return result;
}

/**
 * Parses a `statement` node.
 */
static zvalue parseStatement(ParseState *state) {
    zint mark = cursor(state);
    zvalue result = NULL;

    if (result == NULL) { result = parseVarDef(state); }
    if (result == NULL) { result = parseExpression(state); }

    if (result == NULL) {
        return NULL;
    }

    if (readMatch(state, STR_CH_SEMICOLON)) {
        return result;
    } else {
        reset(state, mark);
        return NULL;
    }
}

/**
 * Parses a `yield` node.
 */
static zvalue parseYield(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_DIAMOND) == NULL) {
        return NULL;
    }

    zvalue expression = parseExpression(state);

    if ((expression == NULL) ||
        (readMatch(state, STR_CH_SEMICOLON) == NULL)) {
        reset(state, mark);
        return NULL;
    }

    return expression;
}

/**
 * Parses a `block` node.
 */
static zvalue parseBlock(ParseState *state) {
    zvalue statements = datListletEmpty();

    for (;;) {
        zvalue statement = parseStatement(state);
        if (statement == NULL) {
            break;
        }
        statements = datListletAppend(statements, statement);
    }

    zvalue yield = parseYield(state);
    zvalue result = datMapletPut(datMapletEmpty(), STR_STATEMENTS, statements);

    if (yield != NULL) {
        result = datMapletPut(result, STR_YIELD, yield);
    }

    return datHighletFrom(STR_BLOCK, result);
}

/**
 * Parses a `formals` node.
 */
static zvalue parseFormals(ParseState *state) {
    zint mark = cursor(state);
    zvalue formals = datListletEmpty();

    for (;;) {
        zvalue identifier = readMatch(state, STR_IDENTIFIER);
        if (identifier == NULL) {
            break;
        }

        zvalue formal =
            datMapletPut(datMapletEmpty(), STR_NAME, datHighletValue(identifier));

        if (readMatch(state, STR_CH_STAR) != NULL) {
            // In Samizdat Layer 0, the only modifier for a formal is
            // `*` which has to be on the last formal.
            formal = datMapletPut(formal, STR_REPEAT, TOK_CH_STAR);
            formals = datListletAppend(formals, formal);
            break;
        }

        formal = datMapletPut(formal, STR_REPEAT, TOK_CH_DOT);
        formals = datListletAppend(formals, formal);
    }

    if (datSize(formals) != 0) {
        if (readMatch(state, STR_CH_COLONCOLON) == NULL) {
            // We didn't find the expected `::` which means there
            // was no formals list at all. So reset the parse, but
            // still succeed with an empty formals list.
            reset(state, mark);
            formals = datListletEmpty();
        }
    }

    return datHighletFrom(STR_FORMALS, formals);
}

/**
 * Parses a `program` node.
 */
static zvalue parseProgram(ParseState *state) {
    // Note: Both of these always succeed. That is, an empty token
    // list is a valid program.
    zvalue formals = parseFormals(state);
    zvalue block = parseBlock(state);

    zvalue value = datMapletPut(datMapletEmpty(), STR_FORMALS, formals);
    value = datMapletPut(value, STR_BLOCK, block);
    return datHighletFrom(STR_FUNCTION, value);
}

/**
 * Parses a `function` node.
 */
static zvalue parseFunction(ParseState *state) {
    zint mark = cursor(state);

    if (readMatch(state, STR_CH_OCURLY) == NULL) {
        return NULL;
    }

    // This always succeeds. See note in `parseProgram` above.
    zvalue result = parseProgram(state);

    if (readMatch(state, STR_CH_CCURLY) == NULL) {
        reset(state, mark);
        return NULL;
    }

    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue langNodeFromProgramText(zvalue programText) {
    zvalue tokens = tokenize(programText);
    ParseState state = { tokens, datSize(tokens), 0 };
    zvalue result = parseProgram(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of program.");
    }

    return result;
}
