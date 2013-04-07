/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "sam-exec.h"

#include <stddef.h>

enum {
    /** Maximum number of tokens in a given parse. */
    MAX_TOKENS = 100000,

    /** Maximum number of characters in an identifier. */
    MAX_IDENTIFIER_CHARS = 40,

    /** Maximum number of characters in a string. */
    MAX_STRING_CHARS = 200
};

typedef struct {
    /** Stringlet being parsed. */
    zvalue str;

    /** Size of stringlet. */
    zint size;

    /** Current read position. */
    zint at;
}
ParseState;

/* Prefab stringlets. Lazily initialized. */
static zvalue STR_CH_AT;
static zvalue STR_CH_CARET;
static zvalue STR_CH_CL_CURLY;
static zvalue STR_CH_CL_PAREN;
static zvalue STR_CH_CL_SQUARE;
static zvalue STR_CH_COLON;
static zvalue STR_CH_EQUAL;
static zvalue STR_CH_OP_CURLY;
static zvalue STR_CH_OP_PAREN;
static zvalue STR_CH_OP_SQUARE;
static zvalue STR_CH_SEMICOLON;
static zvalue STR_IDENTIFIER;
static zvalue STR_INTEGER;
static zvalue STR_STRING;
static zvalue STR_TYPE;
static zvalue STR_VALUE;

/* Prefab tokens. Lazily initialized. */
static zvalue TOK_CH_AT;
static zvalue TOK_CH_CARET;
static zvalue TOK_CH_CL_CURLY;
static zvalue TOK_CH_CL_PAREN;
static zvalue TOK_CH_CL_SQUARE;
static zvalue TOK_CH_COLON;
static zvalue TOK_CH_EQUAL;
static zvalue TOK_CH_OP_CURLY;
static zvalue TOK_CH_OP_PAREN;
static zvalue TOK_CH_OP_SQUARE;
static zvalue TOK_CH_SEMICOLON;
static zvalue TOK_IDENTIFIER;
static zvalue TOK_INTEGER;
static zvalue TOK_STRING;

/*
 * Helper functions
 */

/**
 * Initializes the static variables.
 */
static void init(void) {
    if (STR_CH_AT != NULL) {
        return;
    }

    STR_CH_AT        = samStringletFromUtf8String("@", -1);
    STR_CH_CARET     = samStringletFromUtf8String("^", -1);
    STR_CH_CL_CURLY  = samStringletFromUtf8String("}", -1);
    STR_CH_CL_PAREN  = samStringletFromUtf8String(")", -1);
    STR_CH_CL_SQUARE = samStringletFromUtf8String("]", -1);
    STR_CH_COLON     = samStringletFromUtf8String(":", -1);
    STR_CH_EQUAL     = samStringletFromUtf8String("=", -1);
    STR_CH_OP_CURLY  = samStringletFromUtf8String("{", -1);
    STR_CH_OP_PAREN  = samStringletFromUtf8String("(", -1);
    STR_CH_OP_SQUARE = samStringletFromUtf8String("[", -1);
    STR_CH_SEMICOLON = samStringletFromUtf8String(";", -1);
    STR_IDENTIFIER   = samStringletFromUtf8String("identifier", -1);
    STR_INTEGER      = samStringletFromUtf8String("integer", -1);
    STR_STRING       = samStringletFromUtf8String("string", -1);
    STR_TYPE         = samStringletFromUtf8String("type", -1);
    STR_VALUE        = samStringletFromUtf8String("value", -1);

    zvalue empty = samMapletEmpty();
    TOK_CH_AT        = samMapletPut(empty, STR_TYPE, STR_CH_AT);
    TOK_CH_CARET     = samMapletPut(empty, STR_TYPE, STR_CH_CARET);
    TOK_CH_CL_CURLY  = samMapletPut(empty, STR_TYPE, STR_CH_CL_CURLY);
    TOK_CH_CL_PAREN  = samMapletPut(empty, STR_TYPE, STR_CH_CL_PAREN);
    TOK_CH_CL_SQUARE = samMapletPut(empty, STR_TYPE, STR_CH_CL_SQUARE);
    TOK_CH_COLON     = samMapletPut(empty, STR_TYPE, STR_CH_COLON);
    TOK_CH_EQUAL     = samMapletPut(empty, STR_TYPE, STR_CH_EQUAL);
    TOK_CH_OP_CURLY  = samMapletPut(empty, STR_TYPE, STR_CH_OP_CURLY);
    TOK_CH_OP_PAREN  = samMapletPut(empty, STR_TYPE, STR_CH_OP_PAREN);
    TOK_CH_OP_SQUARE = samMapletPut(empty, STR_TYPE, STR_CH_OP_SQUARE);
    TOK_CH_SEMICOLON = samMapletPut(empty, STR_TYPE, STR_CH_SEMICOLON);
    TOK_IDENTIFIER   = samMapletPut(empty, STR_TYPE, STR_IDENTIFIER);
    TOK_INTEGER      = samMapletPut(empty, STR_TYPE, STR_INTEGER);
    TOK_STRING       = samMapletPut(empty, STR_TYPE, STR_STRING);
}

/**
 * Is the parse state at EOF?
 */
static bool isEof(ParseState *state) {
    return (state->at >= state->size);
}

/**
 * Peeks at the next character.
 */
static zint peek(ParseState *state) {
    return samListletGetInt(state->str, state->at);
}

/**
 * Reads the next character.
 */
static zint read(ParseState *state) {
    zint result = peek(state);
    state->at++;

    return result;
}

/**
 * Skips whitespace.
 */
static void skipWhitespace(ParseState *state) {
    while (!isEof(state)) {
        zint ch = peek(state);
        if (ch == '#') {
            read(state);
            while (!isEof(state)) {
                ch = read(state);
                if (ch == '\n') {
                    break;
                }
            }
        } else if ((ch != ' ') && (ch != '\n')) {
            break;
        }
    }
}

/**
 * Parses an integer token, updating the given input position.
 */
static zvalue tokenizeInteger(ParseState *state) {
    zint value = 0;
    bool negative = false;
    bool any = false;

    if (peek(state) == '-') {
        negative = true;
        read(state);
    }

    while (!isEof(state)) {
        zint ch = peek(state);

        if ((ch < '0') || (ch > '9')) {
            break;
        }
        
        read(state);
        value = (value * 10) + (ch - '0');

        if (value >= 0x80000000) {
            samDie("Overlarge integer token.");
        }
    }

    if (!any) {
        samDie("Invalid integer token (no digits).");
    }

    zvalue intlet = samIntletFromInt(negative ? -value : value);
    return samMapletPut(TOK_INTEGER, STR_VALUE, intlet);
}

/**
 * Parses an identifier token, updating the given input position.
 */
static zvalue tokenizeIdentifier(ParseState *state) {
    zint size = 1;
    zvalue chars[MAX_IDENTIFIER_CHARS];

    // First character is guaranteed valid, because this function
    // wouldn't have been called otherwise.
    chars[0] = samIntletFromInt(read(state));

    while (!isEof(state)) {
        zint ch = peek(state);

        if (!(((ch >= '0') && (ch <= '9')) ||
              ((ch >= 'a') && (ch <= 'z')) ||
              ((ch >= 'A') && (ch <= 'Z')))) {
            break;
        } else if (size == sizeof(chars)) {
            samDie("Overlong identifier token.");
        }

        chars[size] = samIntletFromInt(ch);
        size++;
        read(state);
    }

    zvalue stringlet = samListletFromValues(chars, size);
    return samMapletPut(TOK_IDENTIFIER, STR_VALUE, stringlet);
}

/**
 * Parses a string token, updating the given input position.
 */
static zvalue tokenizeString(ParseState *state) {
    // Skip the initial quote.
    read(state);

    zint size = 0;
    zvalue chars[MAX_STRING_CHARS];

    for (;;) {
        if (isEof(state)) {
            samDie("Unterminated string.");
        }

        zint ch = peek(state);

        if (ch == '\"') {
            read(state);
            break;
        } else if (size == sizeof(chars)) {
            samDie("Overlong string token.");
        } else if (ch == '\\') {
            read(state);
            ch = peek(state);
            if ((ch == '\"') || (ch == '\\')) {
                // These pass through as-is.
            } else if (ch == 'n') {
                ch = '\n';
            } else {
                samDie("Invalid string escape character: %#llx", ch);
            }
        }

        chars[size] = samIntletFromInt(ch);
        size++;
        read(state);
    }

    zvalue stringlet = samListletFromValues(chars, size);
    return samMapletPut(TOK_STRING, STR_VALUE, stringlet);
}

/**
 * Parses a single token, updating the given input position.
 */
static zvalue tokenizeOne(ParseState *state) {
    skipWhitespace(state);

    zint ch = peek(state);

    switch (ch) {
        case '@': read(state); return TOK_CH_AT;
        case ':': read(state); return TOK_CH_COLON;
        case ';': read(state); return TOK_CH_SEMICOLON;
        case '=': read(state); return TOK_CH_EQUAL;
        case '^': read(state); return TOK_CH_CARET;
        case '{': read(state); return TOK_CH_OP_CURLY;
        case '}': read(state); return TOK_CH_CL_CURLY;
        case '(': read(state); return TOK_CH_OP_PAREN;
        case ')': read(state); return TOK_CH_CL_PAREN;
        case '[': read(state); return TOK_CH_OP_SQUARE;
        case ']': read(state); return TOK_CH_CL_SQUARE;
        case '\"': return tokenizeString(state);
    }

    if (((ch >= '0') && (ch <= '9')) || (ch == '-')) {
        return tokenizeInteger(state);
    } else if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) {
        return tokenizeIdentifier(state);
    }

    samDie("Invalid character in token stream: %lld", ch);
}


/*
 * Public API
 */

/**
 * Tokenizes a stringlet using Samizdat Layer 0 token syntax. Returns
 * a listlet of tokens.
 */
zvalue tokenize(zvalue stringlet) {
    init();

    zvalue result[MAX_TOKENS];
    ParseState state = { stringlet, samSize(stringlet), 0 };
    zint out = 0;

    while (!isEof(&state)) {
        zvalue one = tokenizeOne(&state);
        if (one == NULL) {
            break;
        }
        result[out] = one;
        out++;
    }

    return samListletFromValues(result, out);
}
