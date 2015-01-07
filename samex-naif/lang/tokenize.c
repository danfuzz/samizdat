// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "langnode.h"
#include "util.h"
#include "type/Cmp.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Symbol.h"

#include "impl.h"


//
// ParseState definition and functions
//

/** State of tokenization in-progress. */
typedef struct {
    /** String being parsed. */
    zstring str;

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
    return (state->at >= state->str.size);
}

/**
 * Peeks at the next character.
 */
static zint peek(ParseState *state) {
    return isEof(state) ? (zint) -1 : state->str.chars[state->at];
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
        die("Cannot reset forward: %d > %d", mark, state->at);
    }

    state->at = mark;
}


//
// Samizdat 0 Tree Grammar
//
// This is *not* a direct transliteration of the spec's reference tokenizer,
// but it is nonetheless intended to implement the same grammar.
//

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
    return cm_new_Record(SYM(int), SYM(value), intval);
}

/**
 * Parses an identifier token, updating the given input position.
 */
static zvalue tokenizeIdentifier(ParseState *state) {
    zchar chars[LANG_MAX_STRING_CHARS];
    zstring s = {0, chars};

    for (;;) {
        zint ch = peek(state);

        if (!((ch == '_') ||
              (ch == '$') ||
              ((ch >= 'a') && (ch <= 'z')) ||
              ((ch >= 'A') && (ch <= 'Z')) ||
              ((ch >= '0') && (ch <= '9')))) {
            break;
        } else if (s.size == LANG_MAX_STRING_CHARS) {
            die("Overlong identifier token.");
        }

        chars[s.size] = ch;
        s.size++;
        read(state);
    }

    if (s.size == 0) {
        return NULL;
    }

    zvalue name = symbolFromZstring(s);

    switch (chars[0]) {
        case 'b': { if (cmpEq(name, SYM(break)))    return TOK_break;    break; }
        case 'd': { if (cmpEq(name, SYM(def)))      return TOK_def;      break; }
        case 'e': { if (cmpEq(name, SYM(export)))   return TOK_export;   break; }
        case 'i': { if (cmpEq(name, SYM(import)))   return TOK_import;   break; }
        case 'n': { if (cmpEq(name, SYM(null)))     return TOK_null;     break; }
        case 'r': { if (cmpEq(name, SYM(return)))   return TOK_return;   break; }
        case 't': { if (cmpEq(name, SYM(ztrue)))    return TOK_ztrue;    break; }
        case 'v': { if (cmpEq(name, SYM(var)))      return TOK_var;      break; }
        case 'y': { if (cmpEq(name, SYM(yield)))    return TOK_yield;    break; }
        case 'c': {
                    if (cmpEq(name, SYM(class)))    return TOK_class;
                    if (cmpEq(name, SYM(continue))) return TOK_continue;
                    break;
        }
        case 'f': {
                    if (cmpEq(name, SYM(zfalse)))   return TOK_zfalse;
                    if (cmpEq(name, SYM(fn)))       return TOK_fn;
                    break;
        }
    }

    return cm_new_Record(SYM(identifier), SYM(value), name);
}

/**
 * Parses a string token, updating the given input position.
 */
static zvalue tokenizeString(ParseState *state) {
    // Skip the initial quote.
    read(state);

    zchar chars[LANG_MAX_STRING_CHARS];
    zstring s = {0, chars};

    for (;;) {
        zint ch = peek(state);

        if (ch == -1) {
            die("Unterminated string.");
        } else if (ch == '\n') {
            die("Invalid character in string: `\n`");
        } else if (ch == '\"') {
            read(state);
            break;
        } else if (s.size == LANG_MAX_STRING_CHARS) {
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
                    die("Invalid string escape character: %x", ch);
                }
            }
        }

        chars[s.size] = ch;
        s.size++;
        read(state);
    }

    zvalue string = stringFromZstring(s);
    return cm_new_Record(SYM(string), SYM(value), string);
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
    zvalue name = symbolFromString(cm_get(result, SYM(value)));
    return cm_new_Record(SYM(identifier), SYM(value), name);
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
        case ':': { read(state); return TOK_CH_COLONCOLON; }
        case '=': { read(state); return TOK_CH_COLONEQUAL; }
        default:  {              return TOK_CH_COLON;      }
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

    zchar chars[LANG_MAX_STRING_CHARS];
    zstring s = {0, chars};

    for (;;) {
        zint ch = read(state);

        if ((ch == -1) || (ch == '\n')) {
            break;
        } else if (s.size == LANG_MAX_STRING_CHARS) {
            die("Overlong directive token.");
        } else if ((s.size == 0) && (ch == ' ')) {
            // Skip initial spaces.
            continue;
        }

        chars[s.size] = ch;
        s.size++;
    }

    // Trim spaces at EOL.
    while ((s.size > 0) && (chars[s.size - 1] == ' ')) {
        s.size--;
    }

    zvalue value = stringFromZstring(s);
    return cm_new_Record(SYM(directive),
        SYM(name), cm_get(name, SYM(value)),
        SYM(value), value);
}

/**
 * Parses a single token, updating the given input position. This skips
 * initial whitespace, if any.
 */
static zvalue tokenizeAnyToken(ParseState *state) {
    skipWhitespace(state);

    zint ch = peek(state);

    switch (ch) {
        case -1:   {              return NULL;                            }
        case '@':  { read(state); return TOK_CH_AT;                       }
        case '}':  { read(state); return TOK_CH_CCURLY;                   }
        case ')':  { read(state); return TOK_CH_CPAREN;                   }
        case ']':  { read(state); return TOK_CH_CSQUARE;                  }
        case ',':  { read(state); return TOK_CH_COMMA;                    }
        case '=':  { read(state); return TOK_CH_EQUAL;                    }
        case '{':  { read(state); return TOK_CH_OCURLY;                   }
        case '(':  { read(state); return TOK_CH_OPAREN;                   }
        case '[':  { read(state); return TOK_CH_OSQUARE;                  }
        case '?':  { read(state); return TOK_CH_QMARK;                    }
        case '+':  { read(state); return TOK_CH_PLUS;                     }
        case ';':  { read(state); return TOK_CH_SEMICOLON;                }
        case '/':  { read(state); return TOK_CH_SLASH;                    }
        case '*':  { read(state); return TOK_CH_STAR;                     }
        case '\"': {              return tokenizeString(state);           }
        case '\\': {              return tokenizeQuotedIdentifier(state); }
        case ':':  {              return tokenizeColon(state);            }
        case '#':  {              return tokenizeDirective(state);        }
        case '-': {
            return tokenizeOneOrTwo(state, '>', TOK_CH_MINUS, TOK_CH_RARROW);
        }
        case '.': {
            return tokenizeOneOrTwo(state, '.', TOK_CH_DOT,   TOK_CH_DOTDOT);
        }
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            return tokenizeInt(state);
        }
    }

    zvalue result = tokenizeIdentifier(state);

    if (result == NULL) {
        die("Invalid character in token stream: %c", (char) ch);
    }

    return result;
}


//
// Exported Definitions
//

// Documented in header.
zvalue langLanguageOf0(zvalue string) {
    ParseState state = {.str = zstringFromString(string), .at = 0};
    zvalue result = tokenizeAnyToken(&state);

    if ((result != NULL)
        && nodeRecTypeIs(result, NODE_directive)
        && cmpEq(cm_get(result, SYM(name)), SYM(language))) {
        return cm_get(result, SYM(value));
    }

    return NULL;
}

// Documented in header.
zvalue langTokenize0(zvalue string) {
    zstackPointer save = datFrameStart();
    ParseState state = {.str = zstringFromString(string), .at = 0};

    zvalue result[LANG_MAX_TOKENS];
    zint out = 0;

    for (;;) {
        zvalue one = tokenizeAnyToken(&state);
        if (one == NULL) {
            break;
        } else if (!nodeRecTypeIs(one, NODE_directive)) {
            if (out >= LANG_MAX_TOKENS) {
                die("Too many tokens.");
            }

            result[out] = one;
            out++;
        }
    }

    zvalue resultList = listFromZarray((zarray) {out, result});
    datFrameReturn(save, resultList);
    return resultList;
}
