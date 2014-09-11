// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "const.h"
#include "type/Bool.h"
#include "type/Class.h"
#include "type/DerivedData.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Null.h"
#include "type/Number.h"
#include "type/Symbol.h"
#include "type/String.h"
#include "util.h"

#include "helpers.h"
#include "impl.h"
#include "langnode.h"


//
// ParseState definition and functions
//

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
 * Reads the next token if its class matches the given class.
 */
static zvalue readMatch(ParseState *state, zvalue cls) {
    if (isEof(state)) {
        return NULL;
    }

    zvalue result = nth(state->tokens, state->at);

    if (!hasClass(result, cls)) {
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
 * Peeks at the next token, checking against the given class.
 */
static zvalue peekMatch(ParseState *state, zvalue cls) {
    zint mark = cursor(state);
    zvalue result = readMatch(state, cls);

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


//
// Parsing helper functions
//

// Definitions to help avoid boilerplate in the parser functions.
#define RULE(name) parse_##name
#define TOKEN(cls) CLS_##cls
#define DEF_PARSE(name) static zvalue RULE(name)(ParseState *state)
#define PARSE(name) RULE(name)(state)
#define PARSE_OPT(name) parseOpt(RULE(name), state)
#define PARSE_STAR(name) parseStar(RULE(name), state)
#define PARSE_PLUS(name) parsePlus(RULE(name), state)
#define PARSE_DELIMITED_SEQ(name, type) \
    parseDelimitedSequence(RULE(name), TOKEN(type), state)
#define PARSE_LOOKAHEAD(name) parseLookahead(RULE(name), state)
#define MATCH(cls) readMatch(state, (TOKEN(cls)))
#define PEEK(cls) peekMatch(state, (TOKEN(cls)))
#define MARK() zint mark = cursor(state); zvalue tempResult
#define RESET() do { reset(state, mark); } while (0)
#define REJECT() do { RESET(); return NULL; } while (0)
#define REJECT_IF(condition) \
    do { if ((condition)) REJECT(); } while (0)
#define MATCH_OR_REJECT(cls) \
    tempResult = MATCH(cls); \
    REJECT_IF(tempResult == NULL)
#define PARSE_OR_REJECT(name) \
    tempResult = PARSE(name); \
    REJECT_IF(tempResult == NULL)
#define PARSE_PLUS_OR_REJECT(name) \
    tempResult = PARSE_PLUS(name); \
    REJECT_IF(tempResult == NULL)

/** Function prototype for all parser functions. */
typedef zvalue (*parserFunction)(ParseState *);

/**
 * Parses `x?` for an arbitrary rule `x`. Returns a list of parsed `x` results
 * (of size 0 or 1).
 */
static zvalue parseOpt(parserFunction rule, ParseState *state) {
    zvalue one = rule(state);

    return (one == NULL) ? EMPTY_LIST : listFrom1(one);
}

/**
 * Parses `x*` for an arbitrary rule `x`. Returns a list of parsed `x` results.
 */
static zvalue parseStar(parserFunction rule, ParseState *state) {
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
static zvalue parsePlus(parserFunction rule, ParseState *state) {
    MARK();

    zvalue result = parseStar(rule, state);
    REJECT_IF(get_size(result) == 0);

    return result;
}

/**
 * Parses `(x (@y x)*)?` for an arbitrary rule `x` and token type `y`. Returns
 * a list of parsed `x` results.
 */
static zvalue parseDelimitedSequence(parserFunction rule, zvalue tokenType,
        ParseState *state) {
    zvalue item = rule(state);

    if (item == NULL) {
        return EMPTY_LIST;
    }

    zvalue result = listFrom1(item);

    for (;;) {
        MARK();

        if (! readMatch(state, tokenType)) {
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
 * Parses `&x` for an arbitrary rule `x`. Returns the parsed result or
 * `NULL` if it failed.
 */
static zvalue parseLookahead(parserFunction rule, ParseState *state) {
    MARK();
    zvalue result = rule(state);
    RESET();

    return result;
}

/**
 * Parses zero or more semicolons. Always returns `NULL`.
 */
DEF_PARSE(optSemicolons) {
    while (MATCH(CH_SEMICOLON) != NULL) /* empty */ ;
    return NULL;
}


//
// Samizdat 0 Tree Grammar
//
// The following is a near-direct transliterations of the tree syntax
// rules from the spec.
//

// Documented in spec.
DEF_PARSE(assignExpression);
DEF_PARSE(opExpression);
DEF_PARSE(rawClosure);

// Documented in spec.
DEF_PARSE(expression) {
    zstackPointer save = datFrameStart();
    zvalue result = PARSE(assignExpression);

    datFrameReturn(save, result);
    return result;
}

// Documented in spec.
DEF_PARSE(parenExpression) {
    MARK();

    MATCH_OR_REJECT(CH_OPAREN);
    zvalue expression = PARSE_OR_REJECT(expression);
    MATCH_OR_REJECT(CH_CPAREN);

    return withoutInterpolate(expression);
}

// Documented in spec.
DEF_PARSE(nameSymbol) {
    MARK();

    zvalue nameIdent = MATCH_OR_REJECT(identifier);
    return get(nameIdent, SYM_value);
}

// Documented in spec.
DEF_PARSE(nameSymbolList) {
    MARK();

    zvalue result = PARSE_DELIMITED_SEQ(nameSymbol, CH_COMMA);
    REJECT_IF(get_size(result) == 0);

    return result;
}

// Documented in spec.
DEF_PARSE(varLvalue) {
    MARK();

    zvalue name = PARSE_OR_REJECT(nameSymbol);
    return makeVarFetchLvalue(name);
}

// Documented in spec.
DEF_PARSE(varRef) {
    MARK();

    MATCH_OR_REJECT(var);
    zvalue name = PARSE_OR_REJECT(nameSymbol);
    return makeVarRef(name);
}

// Documented in spec.
DEF_PARSE(identifierSymbol) {
    MARK();

    zvalue s = MATCH(string);
    if (s != NULL) { return makeSymbolLiteral(get(s, SYM_value)); }

    zvalue name = PARSE(nameSymbol);
    if (name != NULL) { return makeLiteral(name); }

    // Equivalent to matching `.` in a pex.
    zvalue token = MATCH_OR_REJECT(Value);

    // We reject tokens that either/both have a `value` binding or have a
    // non-alphabetic name, instead of looking up in `KEYWORDS`: `KEYWORDS`
    // isn't defined in layer 0.

    REJECT_IF(get(token, SYM_value) != NULL);

    name = className(get_class(token));
    zchar firstCh = zcharFromString(nth(valToString(name), 0));

    REJECT_IF((firstCh < 'a') || (firstCh > 'z'));

    return makeLiteral(name);
}

// Documented in spec.
DEF_PARSE(keyLiteral) {
    MARK();

    zvalue s = MATCH(string);
    if (s != NULL) {
        return makeLiteral(get(s, SYM_value));
    }

    return PARSE_OR_REJECT(identifierSymbol);
}

/**
 * Helper for `literal`: Parses `[@"(" @"{"]`.
 */
DEF_PARSE(literal1) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(CH_OPAREN); }
    if (result == NULL) { result = MATCH(CH_OCURLY); }

    return result;
}

// Documented in spec.
DEF_PARSE(literal) {
    MARK();

    zvalue token;

    if (MATCH(CH_MINUS)) {
        token = MATCH_OR_REJECT(int);
        return makeLiteral(METH_CALL(neg, get(token, SYM_value)));
    } else if ((token = MATCH(int))) {
        return makeLiteral(get(token, SYM_value));
    } else if ((token = MATCH(string))) {
        return makeLiteral(get(token, SYM_value));
    } else if (MATCH(zfalse)) {
        return makeLiteral(BOOL_FALSE);
    } else if (MATCH(ztrue)) {
        return makeLiteral(BOOL_TRUE);
    } else if (MATCH(null)) {
        return makeLiteral(THE_NULL);
    } else if (MATCH(CH_AT)) {
        zvalue symbol = PARSE_OR_REJECT(identifierSymbol);
        REJECT_IF(PARSE_LOOKAHEAD(literal1));
        return symbol;
    }

    return NULL;
}

/**
 * Helper for `key`: Parses `identifierSymbol @":"`.
 */
DEF_PARSE(key1) {
    MARK();

    zvalue result = PARSE_OR_REJECT(keyLiteral);
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

// Documented in spec.
DEF_PARSE(key) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(key1); }
    if (result == NULL) { result = PARSE(key2); }

    return result;
}

/**
 * Helper for `mapping`: Parses the first alternate.
 */
DEF_PARSE(mapping1) {
    MARK();

    zvalue keys = PARSE_PLUS_OR_REJECT(key);
    zvalue value = PARSE_OR_REJECT(expression);

    return makeData(CLS_mapping, mapFrom2(SYM_keys, keys, SYM_value, value));
}

/**
 * Helper for `mapping`: Parses the second alternate.
 */
DEF_PARSE(mapping2) {
    MARK();

    zvalue value = PARSE_OR_REJECT(expression);

    zvalue result = get(value, SYM_interpolate);
    REJECT_IF(result == NULL);

    return result;
}

/**
 * Helper for `mapping`: Parses the third alternate.
 */
DEF_PARSE(mapping3) {
    MARK();

    zvalue name = PARSE_OR_REJECT(nameSymbol);

    return makeData(CLS_mapping,
        mapFrom2(
            SYM_keys,  listFrom1(makeLiteral(name)),
            SYM_value, makeVarFetch(name)));
}

// Documented in spec.
DEF_PARSE(mapping) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(mapping1); }
    if (result == NULL) { result = PARSE(mapping2); }
    if (result == NULL) { result = PARSE(mapping3); }

    return result;
}

// Documented in spec.
DEF_PARSE(mappings) {
    return PARSE_DELIMITED_SEQ(mapping, CH_COMMA);
}

// Documented in spec.
DEF_PARSE(map) {
    MARK();

    MATCH_OR_REJECT(CH_OCURLY);
    zvalue mappings = PARSE(mappings);
    MATCH_OR_REJECT(CH_CCURLY);

    return makeMapExpression(mappings);
}

// Documented in spec.
DEF_PARSE(symbolTable) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    MATCH_OR_REJECT(CH_OCURLY);
    zvalue mappings = PARSE(mappings);
    MATCH_OR_REJECT(CH_CCURLY);

    return makeSymbolTableExpression(mappings);
}

// Documented in spec.
DEF_PARSE(listItem) {
    MARK();

    zvalue expr = PARSE(expression);

    if (MATCH(CH_COLON)) {
        die("Mapping syntax not valid as a list item or call argument.");
    }

    return expr;
}

// Documented in spec.
DEF_PARSE(unadornedList) {
    return PARSE_DELIMITED_SEQ(listItem, CH_COMMA);
}

// Documented in spec.
DEF_PARSE(list) {
    MARK();

    MATCH_OR_REJECT(CH_OSQUARE);
    zvalue expressions = PARSE(unadornedList);
    MATCH_OR_REJECT(CH_CSQUARE);

    return (get_size(expressions) == 0)
        ? makeLiteral(EMPTY_LIST)
        : makeCallOrApply(REFS(makeList), expressions);
}

// Documented in spec.
DEF_PARSE(type) {
    MARK();

    MATCH_OR_REJECT(CH_ATAT);

    zvalue name = PARSE(identifierSymbol);
    if (name != NULL) {
        return makeLiteral(makeDerivedDataClass(get(name, SYM_value)));
    }

    name = PARSE_OR_REJECT(parenExpression);
    return makeCall(REFS(makeDerivedDataClass), listFrom1(name));
}

// Documented in spec.
DEF_PARSE(deriv) {
    MARK();

    MATCH_OR_REJECT(CH_AT);

    zvalue cls;
    zvalue name = PARSE(identifierSymbol);
    if (name != NULL) {
        cls = makeLiteral(makeDerivedDataClass(get(name, SYM_value)));
    } else {
        cls = PARSE_OR_REJECT(parenExpression);
    }

    // Value is mandatory, so the last one is `PARSE_OR_REJECT`.
    zvalue value = PARSE(parenExpression);
    if (value == NULL) value = PARSE_OR_REJECT(map);

    return makeCall(REFS(makeData), listFrom2(cls, value));
}

// Documented in spec.
DEF_PARSE(fullClosure) {
    MARK();

    if (PEEK(CH_OCURLY) == NULL) {
        return NULL;
    }

    zvalue raw = PARSE_OR_REJECT(rawClosure);

    zvalue closure = makeFullClosure(raw);
    return withoutTops(closure);
}

// Documented in spec.
DEF_PARSE(basicClosure) {
    MARK();

    if (PEEK(CH_OCURLY) == NULL) {
        return NULL;
    }

    zvalue raw = PARSE_OR_REJECT(rawClosure);

    zvalue closure = makeBasicClosure(raw);
    return withoutTops(closure);
}

// Documented in spec.
DEF_PARSE(nullaryClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(fullClosure);

    zvalue formals = get(c, SYM_formals);
    if (!valEq(formals, EMPTY_LIST)) {
        die("Invalid formal argument in code block.");
    }

    return c;
}

// Documented in spec.
DEF_PARSE(basicNullaryClosure) {
    MARK();

    zvalue c = PARSE_OR_REJECT(basicClosure);

    zvalue formals = get(c, SYM_formals);
    if (!valEq(formals, EMPTY_LIST)) {
        die("Invalid formal argument in code block.");
    }

    return c;
}

// Documented in spec.
DEF_PARSE(term) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(varLvalue);       }
    if (result == NULL) { result = PARSE(varRef);          }
    if (result == NULL) { result = PARSE(literal);         }
    if (result == NULL) { result = PARSE(symbolTable);     }
    if (result == NULL) { result = PARSE(map);             }
    if (result == NULL) { result = PARSE(list);            }
    if (result == NULL) { result = PARSE(deriv);           }
    if (result == NULL) { result = PARSE(type);            }
    if (result == NULL) { result = PARSE(fullClosure);     }
    if (result == NULL) { result = PARSE(parenExpression); }

    return result;
}

// Documented in spec.
DEF_PARSE(actualsList) {
    MARK();

    if (MATCH(CH_OPAREN)) {
        zvalue normalActuals = PARSE(unadornedList);  // This never fails.
        MATCH_OR_REJECT(CH_CPAREN);
        zvalue closureActuals = PARSE_STAR(fullClosure);
        return METH_CALL(cat, closureActuals, normalActuals);
    }

    return PARSE_PLUS(fullClosure);
}

// Documented in spec.
DEF_PARSE(postfixOperator) {
    // We differ from the spec here, returning payloads that are directly
    // inspected by the `unaryExpression` code, instead of just being
    // functions to apply.

    MARK();

    zvalue result = NULL;

    if (result == NULL) { result = PARSE(actualsList); }

    if ((result == NULL) && (MATCH(CH_COLONCOLON) != NULL)) {
        result = PARSE_OR_REJECT(keyLiteral);
    }

    if (result == NULL) { result = MATCH(CH_STAR); }
    if (result == NULL) { result = MATCH(CH_QMARK); }

    if (result == NULL) {
        MATCH_OR_REJECT(CH_DOT);
        zvalue name = PARSE_OR_REJECT(nameSymbol);
        zvalue actuals = PARSE_OR_REJECT(actualsList);
        result = makeCall(makeLiteral(name), actuals);
    }

    return result;
}

// Documented in spec.
DEF_PARSE(unaryExpression) {
    MARK();

    zvalue result = PARSE_OR_REJECT(term);
    zvalue postfixes = PARSE_STAR(postfixOperator);

    zint size = get_size(postfixes);
    for (zint i = 0; i < size; i++) {
        zvalue one = nth(postfixes, i);
        if (hasClass(one, CLS_List)) {
            // Regular function call.
            result = makeCallOrApply(result, one);
        } else if (hasClass(one, CLS_call)) {
            // Method call.
            zvalue function = get(one, SYM_function);
            zvalue values = get(one, SYM_values);
            result = makeCallOrApply(function, listPrepend(result, values));
        } else if (valEq(one, TOK_CH_STAR)) {
            result = makeInterpolate(result);
        } else if (valEq(one, TOK_CH_QMARK)) {
            result = makeMaybeValue(result);
        } else if (hasClass(one, CLS_literal)) {
            result = makeCallOrApply(SYMS(get), listFrom2(result, one));
        } else {
            die("Unexpected postfix.");
        }
    }

    return result;
}

// Documented in spec.
DEF_PARSE(opExpression) {
    return PARSE(unaryExpression);
}

// Documented in spec.
DEF_PARSE(assignExpression) {
    zvalue target = PARSE(opExpression);

    if (target == NULL) {
        return NULL;
    }

    MARK();

    if (MATCH(CH_COLONEQUAL) == NULL) {
        return target;
    }

    zvalue value = PARSE(opExpression);
    if (value == NULL) {
        RESET();
        return target;
    }

    zvalue result = makeAssignmentIfPossible(target, value);
    if (result == NULL) {
        RESET();
        return target;
    }

    return result;
}

/**
 * Helper for `yieldOrNonlocal`: Parses the `op` assignment.
 */
DEF_PARSE(yieldOrNonlocal1) {
    zvalue result = NULL;

    if (result == NULL) { result = MATCH(break);    }
    if (result == NULL) { result = MATCH(continue); }
    if (result == NULL) { result = MATCH(return);   }
    if (result == NULL) { result = MATCH(yield);    }

    return result;
}

/**
 * Helper for `yieldOrNonlocal`: Parses the first alternate of the `name`
 * assignment.
 */
DEF_PARSE(yieldOrNonlocal2) {
    MARK();

    if (MATCH(CH_SLASH)) {
        zvalue result = PARSE(varLvalue);
        if (result != NULL) {
            return result;
        }
        RESET();
    }

    return TOK_yield;
}

// Documented in spec.
DEF_PARSE(yieldOrNonlocal) {
    MARK();

    zvalue op = PARSE_OR_REJECT(yieldOrNonlocal1);
    zvalue optQuest = MATCH(CH_QMARK);  // It's okay for this to be `NULL`.

    zvalue name = hasClass(op, CLS_yield)
        ? PARSE(yieldOrNonlocal2)       // It's okay for this to be `NULL`.
        : NULL;
    if (name == NULL) {
        name = makeVarFetch(className(get_class(op)));
    }

    zvalue value = PARSE(expression);   // It's okay for this to be `NULL`.
    if (value != NULL) {
        if (optQuest != NULL) {
            value = makeMaybe(value);
        }
    } else {
        REJECT_IF(optQuest != NULL);
        value = TOK_void;
    }

    return valEq(name, TOK_yield) ? value : makeNonlocalExit(name, value);
}

// Documented in spec.
DEF_PARSE(varDef) {
    MARK();

    MATCH_OR_REJECT(def);
    zvalue name = PARSE_OR_REJECT(nameSymbol);

    zvalue expr;
    if (MATCH(CH_EQUAL)) {
        expr = PARSE_OR_REJECT(expression);
    } else {
        expr = NULL;
    }

    return makeVarDef(name, expr);
}

// Documented in spec.
DEF_PARSE(varDefMutable) {
    MARK();

    MATCH_OR_REJECT(var);
    zvalue name = PARSE_OR_REJECT(nameSymbol);

    zvalue expr;
    if (MATCH(CH_EQUAL)) {
        expr = PARSE_OR_REJECT(expression);
    } else {
        expr = NULL;
    }

    return makeVarDefMutable(name, expr);
}

// Documented in spec.
DEF_PARSE(yieldDef) {
    MARK();

    MATCH_OR_REJECT(CH_SLASH);
    return PARSE_OR_REJECT(nameSymbol);
}

// Documented in spec.
DEF_PARSE(optYieldDef) {
    zvalue result = PARSE(yieldDef);
    return (result != NULL) ? mapFrom1(SYM_yieldDef, result) : EMPTY_MAP;
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

// Documented in spec.
DEF_PARSE(formal) {
    MARK();

    zvalue name = PARSE(nameSymbol);

    if (name == NULL) {
        // If there was no name, then the only valid form for a formal
        // is if this is an unnamed / unused argument.
        MATCH_OR_REJECT(CH_DOT);
    }

    zvalue repeat = PARSE(formal1);  // Okay for it to be `NULL`.
    if (repeat != NULL) {
        repeat = className(get_class(repeat));
    }

    return mapFrom2(SYM_name, name, SYM_repeat, repeat);
}

// Documented in spec.
DEF_PARSE(formalsList) {
    return PARSE_DELIMITED_SEQ(formal, CH_COMMA);
}

/**
 * Helper for `closureDeclarations`: Parses an optional name.
 */
DEF_PARSE(closureDeclarations1) {
    zvalue n = PARSE(nameSymbol);
    return (n == NULL) ? EMPTY_MAP : mapFrom1(SYM_name, n);
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
        return METH_CALL(cat, name, mapFrom1(SYM_formals, formals));
    } else {
        RESET();
        zvalue formals = PARSE(formalsList);    // This never fails.
        return mapFrom1(SYM_formals, formals);
    }
}

/**
 * Helper for `closureDeclarations`: Parses the main part of the syntax.
 */
DEF_PARSE(closureDeclarations3) {
    MARK();

    // Both of these are always maps (possibly empty).
    zvalue most = PARSE(closureDeclarations2);
    zvalue yieldDef = PARSE(optYieldDef);

    MATCH_OR_REJECT(CH_RARROW);

    return METH_CALL(cat, most, yieldDef);
}

// Documented in spec.
DEF_PARSE(closureDeclarations) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(closureDeclarations3); }
    if (result == NULL) { result = mapFrom1(SYM_formals, EMPTY_LIST); }

    return result;
}

// Documented in spec.
DEF_PARSE(functionCommon) {
    MARK();

    zvalue name = PARSE_OR_REJECT(nameSymbol);
    MATCH_OR_REJECT(CH_OPAREN);
    zvalue formals = PARSE(formalsList);  // This never fails.
    MATCH_OR_REJECT(CH_CPAREN);
    zvalue code = PARSE_OR_REJECT(basicNullaryClosure);

    zvalue basic = withName(
        withFormals(
            withYieldDef(code, SYM_return),
            formals),
        name);

    return makeFullClosure(basic);
}

// Documented in spec.
DEF_PARSE(functionDef) {
    MARK();

    MATCH_OR_REJECT(fn);
    zvalue closure = PARSE_OR_REJECT(functionCommon);

    return withTop(makeVarDef(get(closure, SYM_name), closure));
}

// Documented in spec.
DEF_PARSE(methodBind) {
    MARK();

    MATCH_OR_REJECT(fn);
    zvalue bind = PARSE_OR_REJECT(varLvalue);
    MATCH_OR_REJECT(CH_DOT);
    zvalue closure = PARSE_OR_REJECT(functionCommon);

    zvalue formals = get(closure, SYM_formals);
    zvalue name = get(closure, SYM_name);
    zvalue fullClosure = withFormals(closure,
        METH_CALL(cat,
            listFrom1(mapFrom1(SYM_name, SYM_this)),
            formals));
    return makeCall(REFS(classAddMethod),
        listFrom3(bind, makeLiteral(name), fullClosure));
}

/** Helper for `importName`: Parses the first alternate. */
DEF_PARSE(importName1) {
    MARK();

    zvalue name = PARSE_OR_REJECT(nameSymbol);
    zvalue key = MATCH(CH_STAR) ? SYM_prefix : SYM_name;
    MATCH_OR_REJECT(CH_EQUAL);

    return mapFrom1(key, name);
}

// Documented in spec.
DEF_PARSE(importName) {
    zvalue result = PARSE(importName1);
    return (result == NULL) ? EMPTY_MAP : result;
}

/** Helper for `importFormat`: Parses the first alternate. */
DEF_PARSE(importFormat1) {
    MARK();

    MATCH_OR_REJECT(CH_AT);
    zvalue f = PARSE_OR_REJECT(identifierSymbol);
    return mapFrom1(SYM_format, get(f, SYM_value));
}

// Documented in spec.
DEF_PARSE(importFormat) {
    zvalue result = PARSE(importFormat1);
    return (result == NULL) ? EMPTY_MAP : result;
}

/**
 * Helper for `importSource`: Parses `@"." name`, returning a combined
 * string.
 */
DEF_PARSE(importSourceDotName) {
    MARK();

    MATCH_OR_REJECT(CH_DOT);
    zvalue name = PARSE_OR_REJECT(nameSymbol);

    return METH_CALL(cat, STR_CH_DOT, name);
}

/**
 * Helper for `importSource`: Parses `@"/" name`, returning a combined
 * string.
 */
DEF_PARSE(importSourceSlashName) {
    MARK();

    MATCH_OR_REJECT(CH_SLASH);
    zvalue name = PARSE_OR_REJECT(nameSymbol);

    return METH_CALL(cat, STR_CH_SLASH, name);
}

/** Helper for `importSource`: Parses the first alternate. */
DEF_PARSE(importSource1) {
    MARK();

    MATCH_OR_REJECT(CH_DOT);
    MATCH_OR_REJECT(CH_SLASH);
    zvalue first = PARSE_OR_REJECT(nameSymbol);
    zvalue rest = PARSE_STAR(importSourceSlashName);
    zvalue optSuffix = PARSE_OPT(importSourceDotName);

    zvalue name = METH_APPLY(cat,
        METH_CALL(cat, listFrom2(EMPTY_STRING, first), rest, optSuffix));
    return makeData(CLS_internal, mapFrom1(SYM_name, name));
}

/** Helper for `importSource`: Parses the second alternate. */
DEF_PARSE(importSource2) {
    MARK();

    zvalue first = PARSE_OR_REJECT(nameSymbol);
    zvalue rest = PARSE_STAR(importSourceDotName);

    zvalue name = METH_APPLY(cat,
        METH_CALL(cat, listFrom2(EMPTY_STRING, first), rest));
    return makeData(CLS_external, mapFrom1(SYM_name, name));
}

// Documented in spec.
DEF_PARSE(importSource) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(importSource1); }
    if (result == NULL) { result = PARSE(importSource2); }

    return result;
}

/** Helper for `importSelect`: Parses the first alternate. */
DEF_PARSE(importSelect1) {
    MARK();

    MATCH_OR_REJECT(CH_COLONCOLON);
    zvalue result = MATCH_OR_REJECT(CH_STAR);

    return mapFrom1(SYM_select, SYM_CH_STAR);
}

/** Helper for `importSelect`: Parses the second alternate. */
DEF_PARSE(importSelect2) {
    MARK();

    MATCH_OR_REJECT(CH_COLONCOLON);
    zvalue select = PARSE_OR_REJECT(nameSymbolList);

    return mapFrom1(SYM_select, select);
}

// Documented in spec.
DEF_PARSE(importSelect) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(importSelect1); }
    if (result == NULL) { result = PARSE(importSelect2); }

    return (result == NULL) ? EMPTY_MAP : result;
}

// Documented in spec.
DEF_PARSE(importStatement) {
    MARK();

    zvalue optExport = MATCH(export);         // Okay if this fails.
    MATCH_OR_REJECT(import);
    zvalue nameOrPrefix = PARSE(importName);  // Never fails.
    zvalue format = PARSE(importFormat);      // Never fails.
    zvalue source = PARSE_OR_REJECT(importSource);
    zvalue select = PARSE(importSelect);      // Never fails.

    zvalue data = METH_CALL(cat,
        nameOrPrefix,
        format,
        select,
        mapFrom1(SYM_source, source));

    return (optExport != NULL)
        ? makeExport(makeImport(data))
        : makeImport(data);
}

// Documented in spec.
DEF_PARSE(exportableStatement) {
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(functionDef); }
    if (result == NULL) { result = PARSE(varDef);      }

    return result;
}

// Documented in spec.
DEF_PARSE(statement) {
    zstackPointer save = datFrameStart();
    zvalue result = NULL;

    if (result == NULL) { result = PARSE(exportableStatement); }
    if (result == NULL) { result = PARSE(methodBind);          }
    if (result == NULL) { result = PARSE(varDefMutable);       }
    if (result == NULL) { result = PARSE(expression);          }

    datFrameReturn(save, result);
    return result;
}

// Documented in spec.
DEF_PARSE(programStatement) {
    MARK();

    zvalue result = PARSE(statement);  // Okay for this to fail.
    if (result != NULL) { return result; }

    MATCH_OR_REJECT(export);

    result = PARSE(nameSymbolList);
    if (result != NULL) { return makeExportSelection(result); }

    result = PARSE_OR_REJECT(exportableStatement);
    return makeExport(result);
}

// Documented in spec.
DEF_PARSE(closureBody) {
    zvalue statements = EMPTY_LIST;
    zvalue yieldNode = NULL;

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

    if (statement != NULL) {
        statements = listAppend(statements, statement);
    } else {
        yieldNode = PARSE(yieldOrNonlocal);
    }

    PARSE(optSemicolons);

    return mapFrom2(
        SYM_statements, statements,
        SYM_yield,      yieldNode);
}

// Documented in spec.
DEF_PARSE(rawClosure) {
    MARK();

    MATCH_OR_REJECT(CH_OCURLY);

    zvalue decls = PARSE(closureDeclarations);  // This never fails.
    zvalue body = PARSE(closureBody);           // This never fails.

    MATCH_OR_REJECT(CH_CCURLY);

    return METH_CALL(cat, decls, body);
}

// Documented in spec.
DEF_PARSE(program) {
    // Note: This isn't structured the same way as the spec, as it's a bit
    // more awkward to do that at this layer; but the result should be the
    // same.

    zvalue statements = EMPTY_LIST;
    bool any = false;
    bool importOkay = true;

    for (;;) {
        if (any && (MATCH(CH_SEMICOLON) == NULL)) {
            break;
        }
        PARSE(optSemicolons);

        zvalue one = NULL;

        if (importOkay) {
            one = PARSE(importStatement);
            if (one == NULL) {
                importOkay = false;
            }
        }

        if (one == NULL) {
            one = PARSE(programStatement);
            if (one == NULL) {
                break;
            }
        }

        statements = listAppend(statements, one);
        any = true;
    }

    PARSE(optSemicolons);

    zvalue closure = makeFullClosure(
        mapFrom2(SYM_statements, statements, SYM_yield, TOK_void));
    return withoutTops(closure);
}

//
// Exported Definitions
//

// Documented in header.
zvalue langParseExpression0(zvalue expression) {
    zvalue tokens;

    if (hasClass(expression, CLS_String)) {
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

// Documented in header.
zvalue langParseProgram0(zvalue program) {
    zvalue tokens;

    if (hasClass(program, CLS_String)) {
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

// Documented in header.
zvalue langSimplify0(zvalue node, zvalue resolveFn) {
    if (hasClass(node, CLS_closure)) {
        node = withResolvedImports(node, resolveFn);
        return withModuleDefs(node);
    }

    return node;
}
