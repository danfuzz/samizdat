/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"
#include "zlimits.h"

#include <stddef.h>


typedef struct {
    /** String being parsed. */
    zvalue str;

    /** Size of string. */
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
 * Peeks at the next character.
 */
static zint peek(ParseState *state) {
    return isEof(state) ? -1 : datStringNth(state->str, state->at);
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
        } else {
            read(state);
        }
    }
}

/**
 * Parses an integer token, updating the given input position.
 */
static zvalue tokenizeInteger(ParseState *state) {
    zint value = 0;
    zint sign = 1;
    bool any = false;

    if (peek(state) == '-') {
        read(state);
        sign = -1;
    }

    for (;;) {
        zint ch = peek(state);

        if ((ch < '0') || (ch > '9')) {
            break;
        }

        read(state);
        any = true;
        value = (value * 10) + (ch - '0');

        if (value >= 0x80000000) {
            die("Overlarge integer token.");
        }
    }

    if (!any) {
        die("Invalid integer token (no digits).");
    }

    zvalue integer = datIntegerFromInt(value * sign);
    return datHighletFrom(STR_INTEGER, integer);
}

/**
 * Parses an identifier token, updating the given input position.
 */
static zvalue tokenizeIdentifier(ParseState *state) {
    zint size = 0;
    zchar chars[LANG_MAX_IDENTIFIER_CHARS];

    for (;;) {
        zint ch = peek(state);

        if (!((ch == '_') ||
              ((ch >= '0') && (ch <= '9')) ||
              ((ch >= 'a') && (ch <= 'z')) ||
              ((ch >= 'A') && (ch <= 'Z')))) {
            break;
        } else if (size == sizeof(chars)) {
            die("Overlong identifier token.");
        }

        chars[size] = ch;
        size++;
        read(state);
    }

    if (size == 0) {
        return NULL;
    }

    zvalue string = datStringFromChars(size, chars);
    return datHighletFrom(STR_IDENTIFIER, string);
}

/**
 * Parses a string token, updating the given input position.
 */
static zvalue tokenizeString(ParseState *state) {
    // Skip the initial quote.
    read(state);

    zint size = 0;
    zchar chars[LANG_MAX_STRING_CHARS];

    for (;;) {
        zint ch = peek(state);

        if (ch == -1) {
            die("Unterminated string.");
        } else if (ch == '\"') {
            read(state);
            break;
        } else if (size == sizeof(chars)) {
            die("Overlong string token.");
        } else if (ch == '\\') {
            read(state);
            ch = peek(state);
            if ((ch == '\"') || (ch == '\\')) {
                // These pass through as-is.
            } else if (ch == 'n') {
                ch = '\n';
            } else {
                die("Invalid string escape character: %#llx", ch);
            }
        }

        chars[size] = ch;
        size++;
        read(state);
    }

    zvalue string = datStringFromChars(size, chars);
    return datHighletFrom(STR_STRING, string);
}

/**
 * Looks for a second character for a two-character token. If
 * found, returns the indicated token. If not found, either
 * returns the given one-character token or errors (the latter
 * if the one-character token was passed as `NULL`).
 */
static zvalue tokenizeOneOrTwoChars(ParseState *state, zint ch2,
                                    zvalue token1, zvalue token2) {
    zint ch1 = read(state);

    if (peek(state) == ch2) {
        read(state);
        return token2;
    } else if (token1 != NULL) {
        return token1;
    }

    die("Invalid partial \"%c%c\" token.", (char) ch1, (char) ch2);
}

/**
 * Parses a single token, updating the given input position.
 */
static zvalue tokenizeOne(ParseState *state) {
    zint ch = peek(state);

    switch (ch) {
        case '}':  read(state); return TOK_CH_CCURLY;
        case ')':  read(state); return TOK_CH_CPAREN;
        case ']':  read(state); return TOK_CH_CSQUARE;
        case '=':  read(state); return TOK_CH_EQUAL;
        case '>':  read(state); return TOK_CH_GT;
        case '{':  read(state); return TOK_CH_OCURLY;
        case '(':  read(state); return TOK_CH_OPAREN;
        case '[':  read(state); return TOK_CH_OSQUARE;
        case '?':  read(state); return TOK_CH_QMARK;
        case ';':  read(state); return TOK_CH_SEMICOLON;
        case '*':  read(state); return TOK_CH_STAR;
        case '\"':
            return tokenizeString(state);
        case '@':
            return tokenizeOneOrTwoChars(state, '@',
                                         TOK_CH_AT, TOK_CH_ATAT);
        case ':':
            return tokenizeOneOrTwoChars(state, ':',
                                         TOK_CH_COLON, TOK_CH_COLONCOLON);
        case '<':
            return tokenizeOneOrTwoChars(state, '>',
                                         TOK_CH_LT, TOK_CH_DIAMOND);
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '-':
            return tokenizeInteger(state);
    }

    zvalue result = tokenizeIdentifier(state);

    if (result == NULL) {
        die("Invalid character in token stream: \"%c\" (%lld)", (char) ch, ch);
    }

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue tokenize(zvalue string) {
    constInit();

    zvalue result[LANG_MAX_TOKENS];
    ParseState state = { string, datSize(string), 0 };
    zint out = 0;

    for (;;) {
        skipWhitespace(&state);

        if (isEof(&state)) {
            break;
        }

        zvalue one = tokenizeOne(&state);

        result[out] = one;
        out++;
    }

    return datListFromArray(out, result);
}
