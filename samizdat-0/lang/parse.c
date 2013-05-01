/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
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
 * Node constructors and related helpers
 */

/**
 * Makes a 0-3 binding maplet. Values are allowed to be `NULL`, in
 * which case the corresponding key isn't included in the result.
 */
static zvalue mapletFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
                          zvalue k3, zvalue v3) {
    zvalue result = EMPTY_MAPLET;

    if (v1 != NULL) { result = datMapletPut(result, k1, v1); }
    if (v2 != NULL) { result = datMapletPut(result, k2, v2); }
    if (v3 != NULL) { result = datMapletPut(result, k3, v3); }

    return result;
}

/**
 * Makes a 0-2 binding maplet.
 */
static zvalue mapletFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2) {
    return mapletFrom3(k1, v1, k2, v2, NULL, NULL);
}

/**
 * Makes a 0-1 binding maplet.
 */
static zvalue mapletFrom1(zvalue k1, zvalue v1) {
    return mapletFrom3(k1, v1, NULL, NULL, NULL, NULL);
}

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
    zvalue value = mapletFrom2(STR_FUNCTION, function, STR_ACTUALS, actuals);
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

/* Defined below. */
DEF_PARSE(atom);
DEF_PARSE(expression);
DEF_PARSE(function);

/**
 * Parses `atom+`. Returns a listlet of parsed expressions.
 */
DEF_PARSE(atomPlus) {
    MARK();

    zvalue result = EMPTY_LISTLET;

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

    return EMPTY_LISTLET;
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

    zvalue args = datListletAppend(EMPTY_LISTLET, innerType);

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

    return makeCall(makeVarRef(STR_MAKE_UNIQLET), EMPTY_LISTLET);
}

/**
 * Parses a `binding` node.
 */
DEF_PARSE(binding) {
    MARK();

    zvalue key = PARSE_OR_REJECT(atom);
    MATCH_OR_REJECT(CH_EQUAL);
    zvalue value = PARSE_OR_REJECT(atom);

    return datListletAppend(datListletAppend(EMPTY_LISTLET, key), value);
}

/**
 * Parses a `maplet` node.
 */
DEF_PARSE(maplet) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    MATCH_OR_REJECT(CH_OSQUARE);

    zvalue bindings = EMPTY_LISTLET;

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

    return datHighletFrom(STR_LITERAL, EMPTY_MAPLET);
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

    return datHighletFrom(STR_LITERAL, EMPTY_LISTLET);
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
    zvalue value = mapletFrom2(STR_NAME, name, STR_VALUE, expression);
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
 * Parses a `yield` node.
 */
DEF_PARSE(yield) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    MATCH_OR_REJECT(CH_GT);
    zvalue yield = PARSE_OR_REJECT(expression);
    PARSE(optSemicolons);

    return yield;
}

/**
 * Parses a `yieldDef` node.
 */
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    MATCH_OR_REJECT(CH_GT);

    return datHighletValue(identifier);
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
        (value == NULL) ? NULL : datListletAppend(EMPTY_LISTLET, value);

    return makeCall(name, actuals);
}

/**
 * Helper for `formal`: Parses `(@"?" | @"*")?`. Returns either the
 * parsed token or `NULL` to indicate that neither was present.
 */
DEF_PARSE(formal1) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(CH_QMARK); }
    if (result == NULL) { result = MATCH(CH_STAR); }

    return result;
}

/**
 * Parses a `formal` node.
 */
DEF_PARSE(formal) {
    MARK();

    zvalue identifier = MATCH_OR_REJECT(IDENTIFIER);
    zvalue repeat = PARSE(formal1); // Okay for it to be `NULL`.

    return mapletFrom2(STR_NAME, datHighletValue(identifier),
                       STR_REPEAT, repeat);
}

/**
 * Parses a `formals` node.
 */
DEF_PARSE(formals) {
    MARK();
    zvalue formals = EMPTY_LISTLET;

    for (;;) {
        zvalue formal = PARSE(formal);
        if (formal == NULL) {
            break;
        }

        formals = datListletAppend(formals, formal);
    }

    REJECT_IF(datSize(formals) == 0);

    return datHighletFrom(STR_FORMALS, formals);
}

/**
 * Helper for `program`: Parses `(formals? yieldDef? @"::")`. Returns
 * a maplet of bindings to include in the final result, or `NULL` if
 * there were no valid declarations.
 */
DEF_PARSE(program1) {
    MARK();

    // It's okay for either of these to be `NULL`.
    zvalue formals = PARSE(formals);
    zvalue yieldDef = PARSE(yieldDef);

    MATCH_OR_REJECT(CH_COLONCOLON);

    return mapletFrom2(STR_FORMALS, formals, STR_YIELD_DEF, yieldDef);
}

/**
 * Parses a `program` node.
 */
DEF_PARSE(program) {
    zvalue declarations = PARSE(program1); // `NULL` is ok, as it's optional.
    zvalue statements = EMPTY_LISTLET;
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

        statements = datListletAppend(statements, statement);
        PARSE(optSemicolons);
    }

    zvalue statement = PARSE(statement);

    if (statement == NULL) {
        statement = PARSE(nonlocalExit);
    }

    if (statement != NULL) {
        statements = datListletAppend(statements, statement);
    } else {
        yield = PARSE(yield);
    }

    zvalue value = mapletFrom2(STR_STATEMENTS, statements, STR_YIELD, yield);

    if (declarations != NULL) {
        value = datMapletAdd(value, declarations);
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
    constInit();

    zvalue tokens = tokenize(programText);
    ParseState state = { tokens, datSize(tokens), 0 };
    zvalue result = parse_program(&state);

    if (!isEof(&state)) {
        die("Extra tokens at end of program.");
    }

    return result;
}
