/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Value.h"
#include "zlimits.h"


/*
 * Private Definitions
 */

/** State of tokenization in-progress. */
typedef struct {
    /** String being parsed. */
    zchar str[LANG_MAX_TOKENS];

    /** Size of string. */
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
 * Peeks at the next character.
 */
static zint peek(ParseState *state) {
    return isEof(state) ? -1 : state->str[state->at];
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
 * Skips a single-line comment.
 */
static void skipComment(ParseState *state) {
    read(state); // Skip the initial `#`.

    if (isEof(state)) {
        // `#` at end-of-file.
        return;
    }

    zint ch = read(state);

    if (ch == '\n') {
        // `#` at end-of-line.
        return;
    } else if ((ch != '#') && (ch != '!') && (ch != ' ')) {
        die("Invalid character after `#`: %c", (char) ch);
    }

    while (!isEof(state)) {
        if (read(state) == '\n') {
            break;
        }
    }
}

/**
 * Skips whitespace.
 */
static void skipWhitespace(ParseState *state) {
    while (!isEof(state)) {
        zint ch = peek(state);

        if (ch == '#') {
            skipComment(state);
        } else if ((ch != ' ') && (ch != '\n')) {
            break;
        } else {
            read(state);
        }
    }
}

/**
 * Parses an int token, updating the given input position.
 */
static zvalue tokenizeInt(ParseState *state) {
    zint value = 0;
    bool any = false;

    for (;;) {
        zint ch = peek(state);

        if ((ch < '0') || (ch > '9')) {
            break;
        }

        read(state);
        any = true;
        value = (value * 10) + (ch - '0');

        if (value >= 0x80000000) {
            die("Overlarge int token.");
        }
    }

    if (!any) {
        die("Invalid int token (no digits).");
    }

    zvalue intval = intFromZint(value);
    return makeTransValue(STR_int, intval);
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
        } else if (size == LANG_MAX_IDENTIFIER_CHARS) {
            die("Overlong identifier token.");
        }

        chars[size] = ch;
        size++;
        read(state);
    }

    if (size == 0) {
        return NULL;
    }

    zvalue string = stringFromZchars(size, chars);

    switch (chars[0]) {
        case 'd': { if (valEq(string, STR_def))    return TOK_def;    break; }
        case 'f': { if (valEq(string, STR_fn))     return TOK_fn;     break; }
        case 'r': { if (valEq(string, STR_return)) return TOK_return; break; }
    }

    return makeTransValue(STR_identifier, string);
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
        } else if (ch == '\n') {
            die("Invalid character in string: `\n`");
        } else if (ch == '\"') {
            read(state);
            break;
        } else if (size == LANG_MAX_STRING_CHARS) {
            die("Overlong string token.");
        } else if (ch == '\\') {
            read(state);
            ch = peek(state);
            switch (ch) {
                case '0': { ch = '\0'; break; }
                case 'n': { ch = '\n'; break; }
                case 'r': { ch = '\r'; break; }
                case 't': { ch = '\t'; break; }
                case '\"':
                case '\\': {
                    // These all pass through as-is.
                    break;
                }
                default: {
                    die("Invalid string escape character: %#llx", ch);
                }
            }
        }

        chars[size] = ch;
        size++;
        read(state);
    }

    zvalue string = stringFromZchars(size, chars);
    return makeTransValue(STR_string, string);
}

/**
 * Parses a quoted identifier token, updating the given input position.
 */
static zvalue tokenizeQuotedIdentifier(ParseState *state) {
    // Skip the backslash.
    read(state);

    if (peek(state) != '\"') {
        die("Invalid quoted identifier.");
    }

    zvalue result = tokenizeString(state);
    zvalue string = dataOf(result);
    return makeTransValue(STR_identifier, string);
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
        case '&':  read(state); return TOK_CH_AND;
        case '@':  read(state); return TOK_CH_AT;
        case '}':  read(state); return TOK_CH_CCURLY;
        case ')':  read(state); return TOK_CH_CPAREN;
        case ']':  read(state); return TOK_CH_CSQUARE;
        case ',':  read(state); return TOK_CH_COMMA;
        case '=':  read(state); return TOK_CH_EQUAL;
        case '>':  read(state); return TOK_CH_GT;
        case '{':  read(state); return TOK_CH_OCURLY;
        case '(':  read(state); return TOK_CH_OPAREN;
        case '[':  read(state); return TOK_CH_OSQUARE;
        case '?':  read(state); return TOK_CH_QMARK;
        case '+':  read(state); return TOK_CH_PLUS;
        case ';':  read(state); return TOK_CH_SEMICOLON;
        case '*':  read(state); return TOK_CH_STAR;
        case '\"':
            return tokenizeString(state);
        case '\\':
            return tokenizeQuotedIdentifier(state);
        case '-':
            return tokenizeOneOrTwoChars(state, '>',
                                         TOK_CH_MINUS, TOK_CH_RARROW);
        case ':':
            return tokenizeOneOrTwoChars(state, ':',
                                         TOK_CH_COLON, TOK_CH_COLONCOLON);
        case '.':
            return tokenizeOneOrTwoChars(state, '.',
                                         TOK_CH_DOT, TOK_CH_DOTDOT);
        case '<':
            return tokenizeOneOrTwoChars(state, '>',
                                         TOK_CH_LT, TOK_CH_DIAMOND);
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return tokenizeInt(state);
    }

    zvalue result = tokenizeIdentifier(state);

    if (result == NULL) {
        die("Invalid character in token stream: \"%c\" (%lld)", (char) ch, ch);
    }

    return result;
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue langTokenize0(zvalue string) {
    MOD_USE(lang);

    zstackPointer save = pbFrameStart();

    zint size = collSize(string);

    if (size > LANG_MAX_TOKENS) {
        die("Too many characters for tokenization: %lld", size);
    }

    zvalue result[LANG_MAX_TOKENS];
    ParseState state = { .size = size, .at = 0 };
    zint out = 0;

    zcharsFromString(state.str, string);

    for (;;) {
        skipWhitespace(&state);

        if (isEof(&state)) {
            break;
        }

        zvalue one = tokenizeOne(&state);

        result[out] = one;
        out++;
    }

    zvalue resultList = listFromArray(out, result);
    pbFrameReturn(save, resultList);
    return resultList;
}
