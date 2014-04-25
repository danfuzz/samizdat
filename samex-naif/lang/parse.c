/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Number.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"

#include "helpers.h"


/*
 * ParseState definition and functions
 */

/** State of parsing in-progress. */
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

    zvalue result = nth(state->tokens, state->at);
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

    zvalue result = nth(state->tokens, state->at);

    if (!hasType(result, type)) {
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

/**
 * Peeks at the next token, checking against the given type.
 */
static zvalue peekMatch(ParseState *state, zvalue type) {
    zint mark = cursor(state);
    zvalue result = readMatch(state, type);

    if (result == NULL) {
        return NULL;
    }

    reset(state, mark);
    return result;
}

/**
 * Dumps (some of) the pending tokens, as part of an error report.
 */
static void dumpState(ParseState *state) {
    note("Pending tokens:");

    for (zint i = 0; i < 25; i++) {
        zvalue one = read(state);
        if (one == NULL) {
            break;
        }

        char *oneStr = valDebugString(one);
        note("    %s", oneStr);
        utilFree(oneStr);
    }
}


/*
 * Parsing helper functions
 */

/* Definitions to help avoid boilerplate in the parser functions. */
#define RULE(name) parse_##name
#define DEF_PARSE(name) static zvalue RULE(name)(ParseState *state)
#define PARSE(name) RULE(name)(state)
#define PARSE_STAR(name) parseStar(RULE(name), state)
#define PARSE_PLUS(name) parsePlus(RULE(name), state)
#define PARSE_COMMA_SEQ(name) parseCommaSequence(RULE(name), state)
#define MATCH(type) readMatch(state, (TYPE_##type))
#define PEEK(type) peekMatch(state, (TYPE_##type))
#define MARK() zint mark = cursor(state); zvalue tempResult
#define RESET() do { reset(state, mark); } while (0)
#define REJECT() do { RESET(); return NULL; } while (0)
#define REJECT_IF(condition) \
    do { if ((condition)) REJECT(); } while (0)
#define MATCH_OR_REJECT(type) \
    tempResult = MATCH(type); \
    REJECT_IF(tempResult == NULL)
#define PARSE_OR_REJECT(name) \
    tempResult = PARSE(name); \
    REJECT_IF(tempResult == NULL)

/* Function prototype for all parser functions */
typedef zvalue (*parserFunction)(ParseState *);

/**
 * Parses `x*` for an arbitrary rule `x`. Returns a list of parsed `x` results.
 */
zvalue parseStar(parserFunction rule, ParseState *state) {
    zvalue result = EMPTY_LIST;

    for (;;) {
        zvalue one = rule(state);
        if (one == NULL) {
            break;
        }

        result = listAppend(result, one);
    }

    return result;
}

/**
 * Parses `x+` for an arbitrary rule `x`. Returns a list of parsed `x` results.
 */
zvalue parsePlus(parserFunction rule, ParseState *state) {
    MARK();

    zvalue result = parseStar(rule, state);
    REJECT_IF(get_size(result) == 0);

    return result;
}

/**
 * Parses `(x (@"," x)*)?` for an arbitrary rule `x`. Returns a list of
 * parsed `x` results.
 */
zvalue parseCommaSequence(parserFunction rule, ParseState *state) {
    zvalue item = rule(state);

    if (item == NULL) {
        return EMPTY_LIST;
    }

    zvalue result = listFrom1(item);

    for (;;) {
        MARK();

        if (! MATCH(CH_COMMA)) {
            break;
        }

        zstackPointer save = datFrameStart();
        item = rule(state);

        datFrameReturn(save, item);

        if (item == NULL) {
            RESET();
            break;
        }

        result = listAppend(result, item);
    }

    return result;
}

/**
 * Parses zero or more semicolons. Always returns `NULL`.
 */
DEF_PARSE(optSemicolons) {
    while (MATCH(CH_SEMICOLON) != NULL) /* empty */ ;
    return NULL;
}


/*
 * Samizdat 0 Tree Grammar
 *
 * The following is a near-direct transliterations of the tree syntax
 * rules from the spec.
 */

/* Documented in spec. */
DEF_PARSE(assignExpression);
DEF_PARSE(opExpression);
DEF_PARSE(closure);

/* Documented in spec. */
DEF_PARSE(expression) {
    zstackPointer save = datFrameStart();
    zvalue result = PARSE(assignExpression);

    datFrameReturn(save, result);
    return result;
}

/* Documented in spec. */
DEF_PARSE(parenExpression) {
    MARK();

    MATCH_OR_REJECT(CH_OPAREN);
    zvalue expression = PARSE_OR_REJECT(expression);
    MATCH_OR_REJECT(CH_CPAREN);

    return withoutInterpolate(expression);
}

/* Documented in spec. */
DEF_PARSE(name) {
    MARK();

    zvalue nameIdent = MATCH_OR_REJECT(identifier);
    return dataOf(nameIdent);
}

/* Documented in spec. */
DEF_PARSE(varRef) {
    MARK();

    zvalue name = PARSE_OR_REJECT(name);
    return makeVarRef(name);
}

/* Documented in spec. */
DEF_PARSE(int) {
    MARK();

    zvalue neg = MATCH(CH_MINUS);
    zvalue intToken = MATCH_OR_REJECT(int);

    zvalue value = dataOf(intToken);
    if (neg != NULL) {
        value = GFN_CALL(neg, value);
    }

    return makeLiteral(value);
}

/* Documented in spec. */
DEF_PARSE(string) {
    MARK();

    zvalue string = MATCH_OR_REJECT(string);

    return makeLiteral(dataOf(string));
}

/* Documented in spec. */
DEF_PARSE(identifierString) {
    MARK();

    zvalue result;

    result = PARSE(string);
    if (result != NULL) { return result; }

    result = PARSE(name);
    if (result != NULL) { return makeLiteral(result); }

    result = MATCH_OR_REJECT(Value);  // Equivalent to matching `.` in a pex.
    REJECT_IF(dataOf(result) != NULL);

    zvalue type = typeName(get_type(result));
    zchar firstCh = zcharFromString(nth(type, 0));

    REJECT_IF((firstCh < 'a') || (firstCh > 'z'));
    return makeLiteral(type);
}

/**
 * Helper for `key`: Parses `identifierString @":"`.
 */
DEF_PARSE(key1) {
    MARK();

    zvalue result = PARSE_OR_REJECT(identifierString);
    MATCH_OR_REJECT(CH_COLON);

    return result;
}

/**
 * Helper for `key`: Parses `expression @":"`.
 */
DEF_PARSE(key2) {
    MARK();

    zvalue result = PARSE_OR_REJECT(expression);
    MATCH_OR_REJECT(CH_COLON);

    return result;
}

/* Documented in spec. */
DEF_PARSE(key) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(key1); }
    if (result == NULL) { result = PARSE(key2); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(mapping) {
    MARK();

    zvalue keys = PARSE_STAR(key);
    zvalue value = PARSE_OR_REJECT(expression);

    if (get_size(keys) == 0) {
        // No keys were specified, so the value must be either a
        // whole-map interpolation or a variable-name-to-its-value
        // binding.
        zvalue interp = GET(interpolate, value);
        if (interp != NULL) {
            return interp;
        } else if (hasType(value, TYPE_varRef)) {
            return makeCall(REFS(makeValueMap),
                listFrom2(makeLiteral(GET(name, value)), value));
        }

        REJECT();
    }

    // One or more keys.
    return makeCallOrApply(REFS(makeValueMap),
        listAppend(keys, withoutInterpolate(value)));
}

/* Documented in spec. */
DEF_PARSE(map) {
    MARK();

    // This one isn't just a transliteration of the reference code, but the
    // effect is the same.

    MATCH_OR_REJECT(CH_OCURLY);
    zvalue mappings = PARSE_COMMA_SEQ(mapping);
    MATCH_OR_REJECT(CH_CCURLY);

    switch (get_size(mappings)) {
        case 0:  return makeLiteral(EMPTY_MAP);
        case 1:  return nth(mappings, 0);
        default: return makeCall(REFS(cat), mappings);
    }
}

/* Documented in spec. */
DEF_PARSE(listItem) {
    MARK();

    if (PARSE(identifierString) && MATCH(CH_COLON)) {
        die("Mapping syntax not valid as a list item or call argument.");
    }

    RESET();

    return PARSE(expression);
}

/* Documented in spec. */
DEF_PARSE(unadornedList) {
    return PARSE_COMMA_SEQ(listItem);
}

/* Documented in spec. */
DEF_PARSE(list) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    zvalue expressions = PARSE(unadornedList);
    MATCH_OR_REJECT(CH_CSQUARE);

    return (get_size(expressions) == 0)
        ? makeLiteral(EMPTY_LIST)
        : makeCallOrApply(REFS(makeList), expressions);
}

/* Documented in spec. */
DEF_PARSE(typeName) {
    MARK();

    zvalue name = PARSE(identifierString);
    if (name == NULL) { name = PARSE_OR_REJECT(parenExpression); }

    if (hasType(name, TYPE_literal)) {
        return makeLiteral(makeDerivedDataType(GET(value, name)));
    } else {
        return makeCall(REFS(makeDerivedDataType), listFrom1(name));
    }
}

/* Documented in spec. */
DEF_PARSE(type) {
    MARK();

    MATCH_OR_REJECT(CH_ATAT);
    return PARSE_OR_REJECT(typeName);
}

/* Documented in spec. */
DEF_PARSE(deriv) {
    MARK();

    MATCH_OR_REJECT(CH_AT);

    zvalue type = PARSE_OR_REJECT(typeName);

    // Value is optional; these are allowed to all fail.
    zvalue value = PARSE(parenExpression);
    if (value == NULL) value = PARSE(map);
    if (value == NULL) value = PARSE(list);

    zvalue args = (value == NULL) ? listFrom1(type) : listFrom2(type, value);

    return makeCall(REFS(makeValue), args);
}

/* Documented in spec. */
DEF_PARSE(closureWithLookahead) {
    if (PEEK(CH_OCURLY) == NULL) {
        return NULL;
    }

    return PARSE(closure);
}

/* Documented in spec. */
DEF_PARSE(nullaryClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(closureWithLookahead);

    zvalue formals = GET(formals, c);
    if (!valEq(formals, EMPTY_LIST)) {
        die("Invalid formal argument in code block.");
    }

    return c;
}

/* Documented in spec. */
DEF_PARSE(term) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varRef);               }
    if (result == NULL) { result = PARSE(int);                  }
    if (result == NULL) { result = PARSE(string);               }
    if (result == NULL) { result = PARSE(map);                  }
    if (result == NULL) { result = PARSE(list);                 }
    if (result == NULL) { result = PARSE(deriv);                }
    if (result == NULL) { result = PARSE(type);                 }
    if (result == NULL) { result = PARSE(closureWithLookahead); }
    if (result == NULL) { result = PARSE(parenExpression);      }

    return result;
}

/* Documented in spec. */
DEF_PARSE(actualsList) {
    MARK();

    if (MATCH(CH_OPAREN)) {
        zvalue normalActuals = PARSE(unadornedList);  // This never fails.
        MATCH_OR_REJECT(CH_CPAREN);
        zvalue closureActuals = PARSE_STAR(closureWithLookahead);
        return GFN_CALL(cat, closureActuals, normalActuals);
    }

    return PARSE_PLUS(closureWithLookahead);
}

/* Documented in spec. */
DEF_PARSE(postfixOperator) {
    // We differ from the spec here, returning a payload or single token
    // (`*` or `?`) directly. The corresponding `unaryExpression` code
    // decodes these as appropriate.

    MARK();

    zvalue result = NULL;

    if (result == NULL) { result = PARSE(actualsList); }

    if ((result == NULL) && (MATCH(CH_COLONCOLON) != NULL)) {
        result = PARSE_OR_REJECT(identifierString);
    }

    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_QMARK); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(unaryExpression) {
    MARK();

    zvalue result = PARSE_OR_REJECT(term);
    zvalue postfixes = PARSE_STAR(postfixOperator);

    zint size = get_size(postfixes);
    for (zint i = 0; i < size; i++) {
        zvalue one = nth(postfixes, i);
        if (hasType(one, TYPE_List)) {
            result = makeCallOrApply(result, one);
        } else if (valEq(one, TOK_CH_STAR)) {
            result = makeInterpolate(result);
        } else if (valEq(one, TOK_CH_QMARK)) {
            result = makeOptValue(result);
        } else if (hasType(one, TYPE_literal)) {
            result = makeCallOrApply(REFS(get), listFrom2(result, one));
        } else {
            die("Unexpected postfix.");
        }
    }

    return result;
}

/* Documented in spec. */
DEF_PARSE(opExpression) {
    return PARSE(unaryExpression);
}

/* Documented in spec. */
DEF_PARSE(assignExpression) {
    MARK();

    zvalue base = PARSE_OR_REJECT(opExpression);

    if (!(hasType(base, TYPE_varRef) && MATCH(CH_COLONEQUAL))) {
        return base;
    }

    // This code isn't parallel to the in-language code but has the
    // same effect, given that the only valid lvalues are variable references.
    // In this case, we ensured (above) that we've got a `varRef` and
    // recombine it here into a `varBind`.
    zvalue ex = PARSE_OR_REJECT(expression);
    zvalue name = GET(name, base);

    return makeVarBind(name, ex);
}

/**
 * Helper for `nonlocalExit`: Parses `@"<" varRef @">"`.
 */
DEF_PARSE(nonlocalExit1) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue name = PARSE_OR_REJECT(varRef);
    MATCH_OR_REJECT(CH_GT);

    return name;
}

/**
 * Helper for `nonlocalExit`: Parses `@break | @continue | @return`.
 */
DEF_PARSE(nonlocalExit2) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(break); }
    if (result == NULL) { result = MATCH(continue); }
    if (result == NULL) { result = MATCH(return); }
    if (result == NULL) { return NULL; }

    return makeVarRef(typeName(get_type(result)));
}

/* Documented in spec. */
DEF_PARSE(nonlocalExit) {
    zvalue name = NULL;

    if (name == NULL) { name = PARSE(nonlocalExit1); }
    if (name == NULL) { name = PARSE(nonlocalExit2); }
    if (name == NULL) { return NULL; }

    zvalue optValue = PARSE(expression); // It's okay for this to be `NULL`.
    return makeJumpNode(name, optValue);
}

/**
 * Documented in spec. This implementation differs from the
 * spec in that it will return `NULL` either if no diamond is present
 * or if it is a void yield. This is compensated for by matching changes to
 * the implementation of `closureBody`, below.
 */
DEF_PARSE(yield) {
    MARK();

    MATCH_OR_REJECT(CH_DIAMOND);
    return PARSE(expression);
}

/* Documented in spec. */
DEF_PARSE(varDef) {
    MARK();

    MATCH_OR_REJECT(def);
    zvalue name = PARSE_OR_REJECT(name);

    zvalue expr;
    if (MATCH(CH_EQUAL)) {
        expr = PARSE_OR_REJECT(expression);
    } else {
        expr = NULL;
    }

    return makeVarDef(name, expr);
}

/* Documented in spec. */
DEF_PARSE(varDefMutable) {
    MARK();

    MATCH_OR_REJECT(var);
    zvalue name = PARSE_OR_REJECT(name);

    zvalue expr;
    if (MATCH(CH_EQUAL)) {
        expr = PARSE_OR_REJECT(expression);
    } else {
        expr = NULL;
    }

    return makeVarDefMutable(name, expr);
}

/* Documented in spec. */
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_LT);
    zvalue name = PARSE_OR_REJECT(name);
    MATCH_OR_REJECT(CH_GT);

    return name;
}

/* Documented in spec. */
DEF_PARSE(optYieldDef) {
    zvalue result = PARSE(yieldDef);
    return (result != NULL) ? mapFrom1(STR_yieldDef, result) : EMPTY_MAP;
}

/**
 * Helper for `formal`: Parses `[@"?" @"*" @"+"]?`. Returns the parsed token,
 * or `NULL` to indicate that no alternate matched.
 */
DEF_PARSE(formal1) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(CH_QMARK); }
    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_PLUS); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(formal) {
    MARK();

    zvalue name = PARSE(name);

    if (name == NULL) {
        // If there was no name, then the only valid form for a formal
        // is if this is an unnamed / unused argument.
        MATCH_OR_REJECT(CH_DOT);
    }

    zvalue repeat = PARSE(formal1); // Okay for it to be `NULL`.
    if (repeat != NULL) {
        repeat = typeName(get_type(repeat));
    }

    return mapFrom2(STR_name, name, STR_repeat, repeat);
}

/* Documented in spec. */
DEF_PARSE(formalsList) {
    return PARSE_COMMA_SEQ(formal);
}

/**
 * Helper for `closureDeclarations`: Parses an optional name.
 */
DEF_PARSE(closureDeclarations1) {
    zvalue n = PARSE(name);
    return (n == NULL) ? EMPTY_MAP : mapFrom1(STR_name, n);
}

/**
 * Helper for `closureDeclarations`: Parses the `rest` construct in the
 * original spec.
 */
DEF_PARSE(closureDeclarations2) {
    MARK();

    zvalue name = PARSE(closureDeclarations1);  // This never fails.

    if (MATCH(CH_OPAREN) != NULL) {
        zvalue formals = PARSE(formalsList);    // This never fails.
        MATCH_OR_REJECT(CH_CPAREN);
        return GFN_CALL(cat, name, mapFrom1(STR_formals, formals));
    } else {
        RESET();
        zvalue formals = PARSE(formalsList);    // This never fails.
        return mapFrom1(STR_formals, formals);
    }
}

/**
 * Helper for `closureDeclarations`: Parses the main part of the syntax.
 */
DEF_PARSE(closureDeclarations3) {
    MARK();

    // Both of these are always maps (possibly empty).
    zvalue yieldDef = PARSE(optYieldDef);
    zvalue rest = PARSE(closureDeclarations2);

    if (PEEK(CH_DIAMOND) == NULL) {
        MATCH_OR_REJECT(CH_RARROW);
    }

    return GFN_CALL(cat, yieldDef, rest);
}

/* Documented in spec. */
DEF_PARSE(closureDeclarations) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(closureDeclarations3); }
    if (result == NULL) { result = mapFrom1(STR_formals, EMPTY_LIST); }

    return result;
}

/* Documented in spec. */
DEF_PARSE(functionCommon) {
    MARK();

    zvalue name = PARSE_OR_REJECT(name);
    MATCH_OR_REJECT(CH_OPAREN);
    zvalue formals = PARSE(formalsList);  // This never fails.
    MATCH_OR_REJECT(CH_CPAREN);
    zvalue code = PARSE_OR_REJECT(nullaryClosure);

    zvalue yieldDef = GET(yieldDef, code);
    zvalue returnDef = (yieldDef == NULL)
        ? EMPTY_LIST
        : listFrom1(makeVarDef(yieldDef, REFS(return)));

    zvalue closureMap = GFN_CALL(cat,
        dataOf(code),
        mapFrom4(
            STR_formals,    formals,
            STR_name,       name,
            STR_yieldDef,   STR_return,
            STR_statements, GFN_CALL(cat, returnDef, GET(statements, code))));
    return makeValue(TYPE_closure, closureMap, NULL);
}

/* Documented in spec. */
DEF_PARSE(functionDef) {
    MARK();

    MATCH_OR_REJECT(fn);
    zvalue closure = PARSE_OR_REJECT(functionCommon);

    return withTop(makeVarDef(GET(name, closure), closure));
}

/* Documented in spec. */
DEF_PARSE(genericBind) {
    MARK();

    MATCH_OR_REJECT(fn);

    // `bind = (parVarRef | parType)`
    zvalue bind = PARSE(varRef);
    if (bind == NULL) { bind = PARSE_OR_REJECT(type); }

    MATCH_OR_REJECT(CH_DOT);
    zvalue closure = PARSE_OR_REJECT(functionCommon);

    zvalue formals = GET(formals, closure);
    zvalue name = GET(name, closure);
    zvalue fullClosure = withFormals(closure,
        GFN_CALL(cat,
            listFrom1(mapFrom1(STR_name, STR_this)),
            formals));
    return makeCall(REFS(genericBind),
        listFrom3(makeVarRef(name), bind, fullClosure));
}

/* Documented in spec. */
DEF_PARSE(genericDef) {
    MARK();

    MATCH_OR_REJECT(fn);
    zvalue optStar = MATCH(CH_STAR);      // Okay if this fails.
    MATCH_OR_REJECT(CH_DOT);
    zvalue name = PARSE_OR_REJECT(name);
    MATCH_OR_REJECT(CH_OPAREN);
    zvalue formals = PARSE(formalsList);  // This never fails.
    MATCH_OR_REJECT(CH_CPAREN);

    zvalue fullFormals = GFN_CALL(cat, listFrom1(EMPTY_MAP), formals);
    zvalue func = (optStar == NULL)
        ? REFS(makeRegularGeneric)
        : REFS(makeUnitypeGeneric);
    zvalue call = makeCall(
        func,
        listFrom3(
            makeLiteral(name),
            makeLiteral(formalsMinArgs(fullFormals)),
            makeLiteral(formalsMaxArgs(fullFormals))));

    return withTop(makeVarDef(name, call));
}

/* Documented in spec. */
DEF_PARSE(exportableStatement) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(functionDef); }
    if (result == NULL) { result = PARSE(genericDef);  }
    if (result == NULL) { result = PARSE(varDef);      }

    return result;
}

/* Documented in spec. */
DEF_PARSE(statement) {
    zstackPointer save = datFrameStart();
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(exportableStatement); }
    if (result == NULL) { result = PARSE(genericBind);         }
    if (result == NULL) { result = PARSE(varDefMutable);       }
    if (result == NULL) { result = PARSE(expression);          }

    datFrameReturn(save, result);
    return result;
}

/* Documented in spec. */
DEF_PARSE(closureBody) {
    zvalue statements = EMPTY_LIST;
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

        PARSE(optSemicolons);
        statements = listAppend(statements, statement);
    }

    zvalue statement = PARSE(statement);

    if (statement == NULL) {
        statement = PARSE(nonlocalExit);
    }

    if (statement != NULL) {
        statements = listAppend(statements, statement);
    } else {
        yield = PARSE(yield);
    }

    PARSE(optSemicolons);

    return mapFrom2(STR_statements, statements, STR_yield, yield);
}

/* Documented in spec. */
DEF_PARSE(closure) {
    MARK();

    MATCH_OR_REJECT(CH_OCURLY);

    zvalue decls = PARSE(closureDeclarations);  // This never fails.
    zvalue body = PARSE(closureBody);           // This never fails.

    MATCH_OR_REJECT(CH_CCURLY);

    zvalue closure = makeValue(TYPE_closure,
        GFN_CALL(cat, decls, body),
        NULL);
    return withSimpleDefs(closure);
}

/* Documented in spec. */
DEF_PARSE(program) {
    zvalue body = PARSE(closureBody);  // This never fails.

    zvalue closure = makeValue(TYPE_closure,
        GFN_CALL(cat, mapFrom1(STR_formals, EMPTY_LIST), body),
        NULL);
    return withSimpleDefs(closure);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue langParseExpression0(zvalue expression) {
    zvalue tokens;

    if (hasType(expression, TYPE_String)) {
        tokens = langTokenize0(expression);
    } else {
        tokens = expression;
    }

    ParseState state = { tokens, get_size(tokens), 0 };
    zvalue result = parse_expression(&state);

    if (!isEof(&state)) {
        dumpState(&state);
        die("Extra tokens at end of expression.");
    }

    return result;
}

/* Documented in header. */
zvalue langParseProgram0(zvalue program) {
    zvalue tokens;

    if (hasType(program, TYPE_String)) {
        tokens = langTokenize0(program);
    } else {
        tokens = program;
    }

    ParseState state = { tokens, get_size(tokens), 0 };
    zvalue result = parse_program(&state);

    if (!isEof(&state)) {
        dumpState(&state);
        die("Extra tokens at end of program.");
    }

    return result;
}
