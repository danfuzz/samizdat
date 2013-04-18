/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"

#include <stddef.h>

zvalue STR_CH_AT = NULL;
zvalue STR_CH_ATAT = NULL;
zvalue STR_CH_CCURLY = NULL;
zvalue STR_CH_CPAREN = NULL;
zvalue STR_CH_CSQUARE = NULL;
zvalue STR_CH_COLONCOLON = NULL;
zvalue STR_CH_DIAMOND = NULL;
zvalue STR_CH_DOT = NULL;
zvalue STR_CH_EQUAL = NULL;
zvalue STR_CH_OCURLY = NULL;
zvalue STR_CH_OPAREN = NULL;
zvalue STR_CH_OSQUARE = NULL;
zvalue STR_CH_SEMICOLON = NULL;
zvalue STR_CH_STAR = NULL;
zvalue STR_ACTUALS = NULL;
zvalue STR_BLOCK = NULL;
zvalue STR_BOOLEAN = NULL;
zvalue STR_CALL = NULL;
zvalue STR_FORMALS = NULL;
zvalue STR_FUNCTION = NULL;
zvalue STR_IDENTIFIER = NULL;
zvalue STR_INTEGER = NULL;
zvalue STR_KEY = NULL;
zvalue STR_LISTLET = NULL;
zvalue STR_LITERAL = NULL;
zvalue STR_MAPLET = NULL;
zvalue STR_NAME = NULL;
zvalue STR_REPEAT = NULL;
zvalue STR_STATEMENTS = NULL;
zvalue STR_STRING = NULL;
zvalue STR_TYPE = NULL;
zvalue STR_UNIQLET = NULL;
zvalue STR_VALUE = NULL;
zvalue STR_VAR_DEF = NULL;
zvalue STR_VAR_REF = NULL;
zvalue STR_YIELD = NULL;

/* Prefab tokens / nodes. Lazily initialized. */
zvalue TOK_CH_AT = NULL;
zvalue TOK_CH_ATAT = NULL;
zvalue TOK_CH_CCURLY = NULL;
zvalue TOK_CH_CPAREN = NULL;
zvalue TOK_CH_CSQUARE = NULL;
zvalue TOK_CH_COLONCOLON = NULL;
zvalue TOK_CH_DIAMOND = NULL;
zvalue TOK_CH_DOT = NULL;
zvalue TOK_CH_EQUAL = NULL;
zvalue TOK_CH_OCURLY = NULL;
zvalue TOK_CH_OPAREN = NULL;
zvalue TOK_CH_OSQUARE = NULL;
zvalue TOK_CH_SEMICOLON = NULL;
zvalue TOK_CH_STAR = NULL;
zvalue TOK_ACTUALS = NULL;
zvalue TOK_BLOCK = NULL;
zvalue TOK_BOOLEAN = NULL;
zvalue TOK_CALL = NULL;
zvalue TOK_FORMALS = NULL;
zvalue TOK_FUNCTION = NULL;
zvalue TOK_IDENTIFIER = NULL;
zvalue TOK_INTEGER = NULL;
zvalue TOK_LISTLET = NULL;
zvalue TOK_LITERAL = NULL;
zvalue TOK_MAPLET = NULL;
zvalue TOK_STRING = NULL;
zvalue TOK_UNIQLET = NULL;
zvalue TOK_VAR_DEF = NULL;
zvalue TOK_VAR_REF = NULL;


/*
 * Module functions
 */

/* Documented in header. */
void constsInit(void) {
    if (STR_CH_AT != NULL) {
        return;
    }

    STR_CH_AT         = datStringletFromUtf8String("@", -1);
    STR_CH_ATAT       = datStringletFromUtf8String("@@", -1);
    STR_CH_CCURLY     = datStringletFromUtf8String("}", -1);
    STR_CH_CPAREN     = datStringletFromUtf8String(")", -1);
    STR_CH_CSQUARE    = datStringletFromUtf8String("]", -1);
    STR_CH_COLONCOLON = datStringletFromUtf8String("::", -1);
    STR_CH_DIAMOND    = datStringletFromUtf8String("<>", -1);
    STR_CH_DOT        = datStringletFromUtf8String(".", -1);
    STR_CH_EQUAL      = datStringletFromUtf8String("=", -1);
    STR_CH_OCURLY     = datStringletFromUtf8String("{", -1);
    STR_CH_OPAREN     = datStringletFromUtf8String("(", -1);
    STR_CH_OSQUARE    = datStringletFromUtf8String("[", -1);
    STR_CH_SEMICOLON  = datStringletFromUtf8String(";", -1);
    STR_CH_STAR       = datStringletFromUtf8String("*", -1);
    STR_ACTUALS       = datStringletFromUtf8String("actuals", -1);
    STR_BLOCK         = datStringletFromUtf8String("block", -1);
    STR_BOOLEAN       = datStringletFromUtf8String("boolean", -1);
    STR_CALL          = datStringletFromUtf8String("call", -1);
    STR_FORMALS       = datStringletFromUtf8String("formals", -1);
    STR_FUNCTION      = datStringletFromUtf8String("function", -1);
    STR_IDENTIFIER    = datStringletFromUtf8String("identifier", -1);
    STR_INTEGER       = datStringletFromUtf8String("integer", -1);
    STR_KEY           = datStringletFromUtf8String("key", -1);
    STR_LISTLET       = datStringletFromUtf8String("listlet", -1);
    STR_LITERAL       = datStringletFromUtf8String("literal", -1);
    STR_MAPLET        = datStringletFromUtf8String("maplet", -1);
    STR_NAME          = datStringletFromUtf8String("name", -1);
    STR_REPEAT        = datStringletFromUtf8String("repeat", -1);
    STR_STATEMENTS    = datStringletFromUtf8String("statements", -1);
    STR_STRING        = datStringletFromUtf8String("string", -1);
    STR_TYPE          = datStringletFromUtf8String("type", -1);
    STR_UNIQLET       = datStringletFromUtf8String("uniqlet", -1);
    STR_VALUE         = datStringletFromUtf8String("value", -1);
    STR_VAR_DEF       = datStringletFromUtf8String("varDef", -1);
    STR_VAR_REF       = datStringletFromUtf8String("varRef", -1);
    STR_YIELD         = datStringletFromUtf8String("yield", -1);

    zvalue empty = datMapletEmpty();
    TOK_CH_AT         = datHighletFromType(STR_CH_AT);
    TOK_CH_ATAT       = datHighletFromType(STR_CH_ATAT);
    TOK_CH_CCURLY     = datHighletFromType(STR_CH_CCURLY);
    TOK_CH_CPAREN     = datHighletFromType(STR_CH_CPAREN);
    TOK_CH_CSQUARE    = datHighletFromType(STR_CH_CSQUARE);
    TOK_CH_COLONCOLON = datHighletFromType(STR_CH_COLONCOLON);
    TOK_CH_DIAMOND    = datHighletFromType(STR_CH_DIAMOND);
    TOK_CH_DOT        = datHighletFromType(STR_CH_DOT);
    TOK_CH_EQUAL      = datHighletFromType(STR_CH_EQUAL);
    TOK_CH_OCURLY     = datHighletFromType(STR_CH_OCURLY);
    TOK_CH_OPAREN     = datHighletFromType(STR_CH_OPAREN);
    TOK_CH_OSQUARE    = datHighletFromType(STR_CH_OSQUARE);
    TOK_CH_SEMICOLON  = datHighletFromType(STR_CH_SEMICOLON);
    TOK_CH_STAR       = datHighletFromType(STR_CH_STAR);
    TOK_ACTUALS       = datHighletFromType(STR_ACTUALS);
    TOK_BLOCK         = datHighletFromType(STR_BLOCK);
    TOK_BOOLEAN       = datHighletFromType(STR_BOOLEAN);
    TOK_CALL          = datHighletFromType(STR_CALL);
    TOK_FORMALS       = datHighletFromType(STR_FORMALS);
    TOK_FUNCTION      = datHighletFromType(STR_FUNCTION);
    TOK_IDENTIFIER    = datHighletFromType(STR_IDENTIFIER);
    TOK_INTEGER       = datHighletFromType(STR_INTEGER);
    TOK_LISTLET       = datHighletFromType(STR_LISTLET);
    TOK_LITERAL       = datHighletFromType(STR_LITERAL);
    TOK_MAPLET        = datHighletFromType(STR_MAPLET);
    TOK_STRING        = datHighletFromType(STR_STRING);
    TOK_UNIQLET       = datHighletFromType(STR_UNIQLET);
    TOK_VAR_DEF       = datHighletFromType(STR_VAR_DEF);
    TOK_VAR_REF       = datHighletFromType(STR_VAR_REF);
}
