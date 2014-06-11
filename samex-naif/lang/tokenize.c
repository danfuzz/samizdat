// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "const.h"
#include "util.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Value.h"
#include "zlimits.h"

#include "helpers.h"
#include "impl.h"


//
// ParseState definition and functions
//

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
 * Gets the current read position.
 */
static zint cursor(ParseState *state) {
    return state->at;
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
    return isEof(state) ? (zint) -1 : state->str[state->at];
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
 * Resets the current read position to the given one.
 */
static void reset(ParseState *state, zint mark) {
    if (mark > state->at) {
        die("Cannot reset forward: %lld > %lld", mark, state->at);
    }

    state->at = mark;
}


/*
 * Samizdat 0 Tree Grammar
 *
 * This is *not* a direct transliteration of the spec's reference tokenizer,
 * but it is nonetheless intended to implement the same grammar.
 */

/**
 * Skips a single-line comment. Should only be called when known to be
 * looking at a `#`. Returns `true` if a comment was parsed.
 */
static bool skipComment(ParseState *state) {
    zint at = cursor(state);
    read(state);  // Skip the initial `#`.

    zint ch = read(state);
    if ((ch != '#') && (ch != '!')) {
        reset(state, at);
        return false;
    }

    for (;;) {
        if (read(state) == '\n') {
            break;
        }
    }

    return true;
}

/**
 * Skips whitespace and comments.
 */
static void skipWhitespace(ParseState *state) {
    for (;;) {
        zint ch = peek(state);

        if (ch == '#') {
            if (!skipComment(state)) {
                break;
            }
        } else if ((ch == ' ') || (ch == '\n')) {
            read(state);
        } else {
            break;
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

        if (ch == '_') {
            read(state);
            continue;
        } else if ((ch < '0') || (ch > '9')) {
            break;
        }

        read(state);
        any = true;
        value = (value * 10) + (ch - '0');

        if (value >= 0x80000000) {
            die("Overvalue int token.");
        }
    }

    if (!any) {
        die("Invalid int token (no digits).");
    }

    zvalue intval = intFromZint(value);
    return makeValue(TYPE_int, intval, NULL);
}

/**
 * Parses an identifier token, updating the given input position.
 */
static zvalue tokenizeIdentifier(ParseState *state) {
    zint size = 0;
    zchar chars[LANG_MAX_STRING_CHARS];

    for (;;) {
        zint ch = peek(state);

        if (!((ch == '_') ||
              (ch == '$') ||
              ((ch >= 'a') && (ch <= 'z')) ||
              ((ch >= 'A') && (ch <= 'Z')) ||
              ((ch >= '0') && (ch <= '9')))) {
            break;
        } else if (size == LANG_MAX_STRING_CHARS) {
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
        case 'b': { if (valEq(string, STR_break))    return TOK_break;    }
        case 'c': { if (valEq(string, STR_continue)) return TOK_continue; }
        case 'd': { if (valEq(string, STR_def))      return TOK_def;      }
        case 'e': { if (valEq(string, STR_export))   return TOK_export;   }
        case 'f': { if (valEq(string, STR_fn))       return TOK_fn;       }
        case 'i': { if (valEq(string, STR_import))   return TOK_import;   }
        case 'r': { if (valEq(string, STR_return))   return TOK_return;   }
        case 'v': { if (valEq(string, STR_var))      return TOK_var;      }
        case 'y': { if (valEq(string, STR_yield))    return TOK_yield;    }
    }

    return makeValue(TYPE_identifier, string, NULL);
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
    return makeValue(TYPE_string, string, NULL);
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
    return makeValue(TYPE_identifier, string, NULL);
}

/**
 * Looks for a second character for a two-character token. If
 * found, returns the indicated token. If not found, either
 * returns the given one-character token or errors (the latter
 * if the one-character token was passed as `NULL`).
 */
static zvalue tokenizeOneOrTwo(ParseState *state, zint ch2,
        zvalue token1, zvalue token2) {
    zint ch1 = read(state);

    if (peek(state) == ch2) {
        read(state);
        return token2;
    } else {
        return token1;
    }
}

/**
 * Tokenizes `:`, `::`, or `:=`.
 */
static zvalue tokenizeColon(ParseState *state) {
    read(state);  // Skip the `:`

    switch (peek(state)) {
        case ':': read(state); return TOK_CH_COLONCOLON;
        case '=': read(state); return TOK_CH_COLONEQUAL;
        default:               return TOK_CH_COLON;
    }
}

/**
 * Tokenizes a directive, if possible.
 */
static zvalue tokenizeDirective(ParseState *state) {
    zint at = cursor(state);

    // Validate the `#=` prefix.
    if ((read(state) != '#') || (read(state) != '=')) {
        reset(state, at);
        return NULL;
    }

    // Skip spaces.
    while (peek(state) == ' ') {
        read(state);
    }

    zvalue name = tokenizeIdentifier(state);

    if (name == NULL) {
        die("Invalid directive name.");
    }

    zint size = 0;
    zchar chars[LANG_MAX_STRING_CHARS];

    for (;;) {
        zint ch = read(state);

        if ((ch == -1) || (ch == '\n')) {
            break;
        } else if (size == LANG_MAX_STRING_CHARS) {
            die("Overlong directive token.");
        } else if ((size == 0) && (ch == ' ')) {
            // Skip initial spaces.
            continue;
        }

        chars[size] = ch;
        size++;
    }

    // Trim spaces at EOL.
    while ((size > 0) && (chars[size - 1] == ' ')) {
        size--;
    }

    zvalue value = stringFromZchars(size, chars);
    return makeValue(TYPE_directive,
        mapFrom2(STR_name, dataOf(name), STR_value, value),
        NULL);
}

/**
 * Parses a single token, updating the given input position. This skips
 * initial whitespace, if any.
 */
static zvalue tokenizeAnyToken(ParseState *state) {
    skipWhitespace(state);

    zint ch = peek(state);

    switch (ch) {
        case -1:                return NULL;
        case '}':  read(state); return TOK_CH_CCURLY;
        case ')':  read(state); return TOK_CH_CPAREN;
        case ']':  read(state); return TOK_CH_CSQUARE;
        case ',':  read(state); return TOK_CH_COMMA;
        case '=':  read(state); return TOK_CH_EQUAL;
        case '{':  read(state); return TOK_CH_OCURLY;
        case '(':  read(state); return TOK_CH_OPAREN;
        case '[':  read(state); return TOK_CH_OSQUARE;
        case '?':  read(state); return TOK_CH_QMARK;
        case '+':  read(state); return TOK_CH_PLUS;
        case ';':  read(state); return TOK_CH_SEMICOLON;
        case '/':  read(state); return TOK_CH_SLASH;
        case '*':  read(state); return TOK_CH_STAR;
        case '\"':              return tokenizeString(state);
        case '\\':              return tokenizeQuotedIdentifier(state);
        case ':':               return tokenizeColon(state);
        case '#':               return tokenizeDirective(state);
        case '-':
            return tokenizeOneOrTwo(state, '>', TOK_CH_MINUS, TOK_CH_RARROW);
        case '.':
            return tokenizeOneOrTwo(state, '.', TOK_CH_DOT,   TOK_CH_DOTDOT);
        case '@':
            return tokenizeOneOrTwo(state, '@', TOK_CH_AT,    TOK_CH_ATAT);
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


//
// Exported Definitions
//

/* Documented in header. */
zvalue langLanguageOf0(zvalue string) {
    zint size = get_size(string);
    ParseState state = { .size = size, .at = 0 };

    zcharsFromString(state.str, string);
    zvalue result = tokenizeAnyToken(&state);

    if ((result != NULL)
        && hasType(result, TYPE_directive)
        && valEq(get(result, STR_name), STR_language)) {
        return get(result, STR_value);
    }

    return NULL;
}

/* Documented in header. */
zvalue langTokenize0(zvalue string) {
    zstackPointer save = datFrameStart();
    zint size = get_size(string);

    if (size > LANG_MAX_TOKENS) {
        die("Too many characters for tokenization: %lld", size);
    }

    zvalue result[size];
    ParseState state = { .size = size, .at = 0 };
    zint out = 0;

    zcharsFromString(state.str, string);

    for (;;) {
        zvalue one = tokenizeAnyToken(&state);
        if (one == NULL) {
            break;
        } else if (!hasType(one, TYPE_directive)) {
            result[out] = one;
            out++;
        }
    }

    zvalue resultList = listFromArray(out, result);
    datFrameReturn(save, resultList);
    return resultList;
}
