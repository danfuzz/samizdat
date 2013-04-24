/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "consts.h"
#include "impl.h"
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

    zvalue result = datListletNth(state->tokens, state->at);
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

    zvalue result = datListletNth(state->tokens, state->at);
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


/*
 * Node constructors
 */

/**
 * Constructs a `varRef` node.
 */
static zvalue makeVarRef(zvalue name) {
    return datHighletFrom(STR_VAR_REF, name);
}

/**
 * Constructs a `call` node.
 */
static zvalue makeCall(zvalue function, zvalue actuals) {
    zvalue value = datMapletPut(datMapletEmpty(), STR_FUNCTION, function);
    value = datMapletPut(value, STR_ACTUALS, actuals);
    return datHighletFrom(STR_CALL, value);
}


/*
 * Parsing functions
 */

/* Definitions to help avoid boilerplate in the parser functions. */
#define DEF_PARSE(name) static zvalue parse_##name(ParseState *state)
#define PARSE(name) parse_##name(state)
#define MATCH(tokenType) readMatch(state, (STR_##tokenType))
#define MARK() zint mark = cursor(state); zvalue tempResult
#define REJECT() do { reset(state, mark); return NULL; } while (0)
#define REJECT_IF(condition) \
    do { if ((condition)) REJECT(); } while (0)
#define MATCH_OR_REJECT(tokenType) \
    tempResult = MATCH(tokenType); \
    REJECT_IF(tempResult == NULL)
#define PARSE_OR_REJECT(name) \
    tempResult = PARSE(name); \
    REJECT_IF(tempResult == NULL)

/* Defined below. */
DEF_PARSE(atom);
DEF_PARSE(expression);
DEF_PARSE(function);

/**
 * Parses `atom+`. Returns a listlet of parsed expressions.
 */
DEF_PARSE(atomPlus) {
    MARK();

    zvalue result = datListletEmpty();

    for (;;) {
        zvalue atom = PARSE(atom);
        if (atom == NULL) {
            break;
        }

        result = datListletAppend(result, atom);
    }

    REJECT_IF(datSize(result) == 0);

    return result;
}

/**
 * Parses `@"(" @")"` as part of parsing a `call` node.
 */
DEF_PARSE(call1) {
    MARK();

    MATCH_OR_REJECT(CH_OPAREN);
    MATCH_OR_REJECT(CH_CPAREN);

    return datListletEmpty();
}

/**
 * Parses a `call` node.
 */
DEF_PARSE(call) {
    MARK();

    zvalue function = PARSE_OR_REJECT(atom);

    zvalue actuals = PARSE(call1);
    if (actuals == NULL) {
        actuals = PARSE_OR_REJECT(atomPlus);
    }

    return makeCall(function, actuals);
}

/**
 * Parses a `highlet` node.
 */
DEF_PARSE(highlet) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    MATCH_OR_REJECT(CH_COLON);
    zvalue innerType = PARSE_OR_REJECT(atom);
    zvalue innerValue = PARSE(atom); // It's okay for this to be NULL.
    MATCH_OR_REJECT(CH_COLON);
    MATCH_OR_REJECT(CH_CSQUARE);

    zvalue args = datListletAppend(datListletEmpty(), innerType);

    if (innerValue != NULL) {
        args = datListletAppend(args, innerValue);
    }

    return makeCall(makeVarRef(STR_MAKE_HIGHLET), args);
}

/**
 * Parses a `uniqlet` node.
 */
DEF_PARSE(uniqlet) {
    MARK();

    MATCH_OR_REJECT(CH_ATAT);

    return makeCall(makeVarRef(STR_MAKE_UNIQLET), datListletEmpty());
}

/**
 * Parses a `binding` node.
 */
DEF_PARSE(binding) {
    MARK();

    zvalue key = PARSE_OR_REJECT(atom);
    MATCH_OR_REJECT(CH_EQUAL);
    zvalue value = PARSE_OR_REJECT(atom);

    return datListletAppend(datListletAppend(datListletEmpty(), key), value);
}

/**
 * Parses a `maplet` node.
 */
DEF_PARSE(maplet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    MATCH_OR_REJECT(CH_OSQUARE);

    zvalue bindings = datListletEmpty();

    for (;;) {
        zvalue binding = PARSE(binding);
        if (binding == NULL) {
            break;
        }

        bindings = datListletAdd(bindings, binding);
    }

    REJECT_IF(datSize(bindings) == 0);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeCall(makeVarRef(STR_MAKE_MAPLET), bindings);
}

/**
 * Parses an `emptyMaplet` node.
 */
DEF_PARSE(emptyMaplet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    MATCH_OR_REJECT(CH_OSQUARE);
    MATCH_OR_REJECT(CH_EQUAL);
    MATCH_OR_REJECT(CH_CSQUARE);

    return datHighletFrom(STR_LITERAL, datMapletEmpty());
}

/**
 * Parses a `listlet` node.
 */
DEF_PARSE(listlet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    MATCH_OR_REJECT(CH_OSQUARE);
    zvalue atoms = PARSE_OR_REJECT(atomPlus);
    MATCH_OR_REJECT(CH_CSQUARE);

    return makeCall(makeVarRef(STR_MAKE_LISTLET), atoms);
}

/**
 * Parses an `emptyListlet` node.
 */
DEF_PARSE(emptyListlet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    MATCH_OR_REJECT(CH_OSQUARE);
    MATCH_OR_REJECT(CH_CSQUARE);

    return datHighletFrom(STR_LITERAL, datListletEmpty());
}

/**
 * Parses a `stringlet` node.
 */
DEF_PARSE(stringlet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);

    zvalue string = MATCH(STRING);
    if (string == NULL) {
        string = MATCH_OR_REJECT(IDENTIFIER);
    }

    zvalue value = datHighletValue(string);
    return datHighletFrom(STR_LITERAL, value);
}

/**
 * Parses an `intlet` node.
 */
DEF_PARSE(intlet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    bool negative = (MATCH(CH_MINUS) != NULL);
    zvalue integer = MATCH_OR_REJECT(INTEGER);
    zvalue value = datHighletValue(integer);

    if (negative) {
        value = datIntletFromInt(-datIntFromIntlet(value));
    }

    return datHighletFrom(STR_LITERAL, value);
}

/**
 * Parses a `varRef` node.
 */
DEF_PARSE(varRef) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);

    return makeVarRef(datHighletValue(identifier));
}

/**
 * Parses a `varDef` node.
 */
DEF_PARSE(varDef) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    MATCH_OR_REJECT(CH_EQUAL);
    zvalue expression = PARSE_OR_REJECT(expression);

    zvalue name = datHighletValue(identifier);
    zvalue value = datMapletPut(datMapletEmpty(), STR_NAME, name);
    value = datMapletPut(value, STR_VALUE, expression);
    return datHighletFrom(STR_VAR_DEF, value);
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
    if (result == NULL) { result = PARSE(intlet); }
    if (result == NULL) { result = PARSE(stringlet); }
    if (result == NULL) { result = PARSE(emptyListlet); }
    if (result == NULL) { result = PARSE(listlet); }
    if (result == NULL) { result = PARSE(emptyMaplet); }
    if (result == NULL) { result = PARSE(maplet); }
    if (result == NULL) { result = PARSE(uniqlet); }
    if (result == NULL) { result = PARSE(highlet); }
    if (result == NULL) { result = PARSE(function); }
    if (result == NULL) { result = PARSE(parenExpression); }

    return result;
}

/**
 * Parses an `expression` node.
 */
DEF_PARSE(expression) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(call); }
    if (result == NULL) { result = PARSE(atom); }

    return result;
}

/**
 * Parses a `statement` node.
 *
 * Note: Per the grammar, statement parsing always succeeds, because the
 * empty token list is a valid statement. So, this function never needs to
 * backtrack. That said, this function *will* return `NULL` to indicate that
 * what it parsed was in fact an empty statement.
 */
DEF_PARSE(statement) {
    zvalue result = PARSE(varDef);
    return (result != NULL) ? result : PARSE(expression);
}

/**
 * Parses a `yield` node.
 */
DEF_PARSE(yield) {
    MARK();

    MATCH_OR_REJECT(CH_DIAMOND);
    zvalue expression = PARSE_OR_REJECT(expression);
    MATCH(CH_SEMICOLON); // Optional semicolon.

    return expression;
}


/**
 * Parses a `formals` node.
 */
DEF_PARSE(formals) {
    MARK();
    zvalue formals = datListletEmpty();

    for (;;) {
        zvalue identifier = MATCH(IDENTIFIER);
        if (identifier == NULL) {
            break;
        }

        zvalue formal = datMapletPut(datMapletEmpty(), STR_NAME,
                                     datHighletValue(identifier));

        if (MATCH(CH_STAR) != NULL) {
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
        if (MATCH(CH_COLONCOLON) == NULL) {
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
DEF_PARSE(program) {
    // Note: An empty token list is a valid program, as is one with
    // a formals list but no statements or yield. So, we never have
    // to backtrack during this rule.

    zvalue formals = PARSE(formals); // Always succeeds.
    zvalue statements = datListletEmpty();
    zvalue yield = NULL; // NULL is ok, as it's optional.

    for (;;) {
        yield = PARSE(yield);

        if (yield != NULL) {
            break;
        }

        // See note in `parseStatement()` header.
        zvalue statement = PARSE(statement);
        if (statement != NULL) {
            statements = datListletAppend(statements, statement);
        }

        if (MATCH(CH_SEMICOLON) == NULL) {
            break;
        }
    }

    zvalue value = datMapletPut(datMapletEmpty(), STR_FORMALS, formals);
    value = datMapletPut(value, STR_STATEMENTS, statements);

    if (yield != NULL) {
        value = datMapletPut(value, STR_YIELD, yield);
    }

    return datHighletFrom(STR_FUNCTION, value);
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
zvalue langNodeFromProgramText(zvalue programText) {
    zvalue tokens = tokenize(programText);
    ParseState state = { tokens, datSize(tokens), 0 };
    zvalue result = parse_program(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of program.");
    }

    return result;
}
