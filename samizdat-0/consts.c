/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"

#include <stddef.h>

zvalue STR_CH_AT = NULL;
zvalue STR_CH_ATAT = NULL;
zvalue STR_CH_CARET = NULL;
zvalue STR_CH_CL_CURLY = NULL;
zvalue STR_CH_CL_PAREN = NULL;
zvalue STR_CH_CL_SQUARE = NULL;
zvalue STR_CH_COLON = NULL;
zvalue STR_CH_COLONCOLON = NULL;
zvalue STR_CH_EQUAL = NULL;
zvalue STR_CH_OP_CURLY = NULL;
zvalue STR_CH_OP_PAREN = NULL;
zvalue STR_CH_OP_SQUARE = NULL;
zvalue STR_CH_SEMICOLON = NULL;
zvalue STR_ACTUALS = NULL;
zvalue STR_BINDING = NULL;
zvalue STR_CALL = NULL;
zvalue STR_EXPRESSION = NULL;
zvalue STR_FORMALS = NULL;
zvalue STR_FUNCTION = NULL;
zvalue STR_IDENTIFIER = NULL;
zvalue STR_INTEGER = NULL;
zvalue STR_KEY = NULL;
zvalue STR_LISTLET = NULL;
zvalue STR_LITERAL = NULL;
zvalue STR_MAPLET = NULL;
zvalue STR_NAME = NULL;
zvalue STR_RETURN = NULL;
zvalue STR_STATEMENT = NULL;
zvalue STR_STATEMENTS = NULL;
zvalue STR_STRING = NULL;
zvalue STR_TYPE = NULL;
zvalue STR_UNIQLET = NULL;
zvalue STR_VALUE = NULL;
zvalue STR_VAR_DEF = NULL;
zvalue STR_VAR_REF = NULL;

/* Prefab tokens / nodes. Lazily initialized. */
zvalue TOK_CH_AT = NULL;
zvalue TOK_CH_ATAT = NULL;
zvalue TOK_CH_CARET = NULL;
zvalue TOK_CH_CL_CURLY = NULL;
zvalue TOK_CH_CL_PAREN = NULL;
zvalue TOK_CH_CL_SQUARE = NULL;
zvalue TOK_CH_COLON = NULL;
zvalue TOK_CH_COLONCOLON = NULL;
zvalue TOK_CH_EQUAL = NULL;
zvalue TOK_CH_OP_CURLY = NULL;
zvalue TOK_CH_OP_PAREN = NULL;
zvalue TOK_CH_OP_SQUARE = NULL;
zvalue TOK_CH_SEMICOLON = NULL;
zvalue TOK_ACTUALS = NULL;
zvalue TOK_BINDING = NULL;
zvalue TOK_CALL = NULL;
zvalue TOK_EXPRESSION = NULL;
zvalue TOK_FORMALS = NULL;
zvalue TOK_FUNCTION = NULL;
zvalue TOK_IDENTIFIER = NULL;
zvalue TOK_INTEGER = NULL;
zvalue TOK_LISTLET = NULL;
zvalue TOK_LITERAL = NULL;
zvalue TOK_MAPLET = NULL;
zvalue TOK_RETURN = NULL;
zvalue TOK_STATEMENT = NULL;
zvalue TOK_STATEMENTS = NULL;
zvalue TOK_STRING = NULL;
zvalue TOK_UNIQLET = NULL;
zvalue TOK_VAR_DEF = NULL;
zvalue TOK_VAR_REF = NULL;


/*
 * Exported functions
 */

/** Documented in `consts.h` */
void constsInit(void) {
    if (STR_CH_AT != NULL) {
        return;
    }

    STR_CH_AT           = samStringletFromUtf8String("@", -1);
    STR_CH_ATAT         = samStringletFromUtf8String("@@", -1);
    STR_CH_CARET        = samStringletFromUtf8String("^", -1);
    STR_CH_CL_CURLY     = samStringletFromUtf8String("}", -1);
    STR_CH_CL_PAREN     = samStringletFromUtf8String(")", -1);
    STR_CH_CL_SQUARE    = samStringletFromUtf8String("]", -1);
    STR_CH_COLON        = samStringletFromUtf8String(":", -1);
    STR_CH_COLONCOLON   = samStringletFromUtf8String("::", -1);
    STR_CH_EQUAL        = samStringletFromUtf8String("=", -1);
    STR_CH_OP_CURLY     = samStringletFromUtf8String("{", -1);
    STR_CH_OP_PAREN     = samStringletFromUtf8String("(", -1);
    STR_CH_OP_SQUARE    = samStringletFromUtf8String("[", -1);
    STR_CH_SEMICOLON    = samStringletFromUtf8String(";", -1);
    STR_ACTUALS         = samStringletFromUtf8String("actuals", -1);
    STR_BINDING         = samStringletFromUtf8String("binding", -1);
    STR_CALL            = samStringletFromUtf8String("call", -1);
    STR_EXPRESSION      = samStringletFromUtf8String("expression", -1);
    STR_FORMALS         = samStringletFromUtf8String("formals", -1);
    STR_FUNCTION        = samStringletFromUtf8String("function", -1);
    STR_IDENTIFIER      = samStringletFromUtf8String("identifier", -1);
    STR_INTEGER         = samStringletFromUtf8String("integer", -1);
    STR_KEY             = samStringletFromUtf8String("key", -1);
    STR_LISTLET         = samStringletFromUtf8String("listlet", -1);
    STR_LITERAL         = samStringletFromUtf8String("literal", -1);
    STR_MAPLET          = samStringletFromUtf8String("maplet", -1);
    STR_NAME            = samStringletFromUtf8String("name", -1);
    STR_RETURN          = samStringletFromUtf8String("return", -1);
    STR_STATEMENT       = samStringletFromUtf8String("statement", -1);
    STR_STATEMENTS      = samStringletFromUtf8String("statements", -1);
    STR_STRING          = samStringletFromUtf8String("string", -1);
    STR_TYPE            = samStringletFromUtf8String("type", -1);
    STR_UNIQLET         = samStringletFromUtf8String("uniqlet", -1);
    STR_VALUE           = samStringletFromUtf8String("value", -1);
    STR_VAR_DEF         = samStringletFromUtf8String("varDef", -1);
    STR_VAR_REF         = samStringletFromUtf8String("varRef", -1);

    zvalue empty = samMapletEmpty();
    TOK_CH_AT           = samMapletPut(empty, STR_TYPE, STR_CH_AT);
    TOK_CH_ATAT         = samMapletPut(empty, STR_TYPE, STR_CH_ATAT);
    TOK_CH_CARET        = samMapletPut(empty, STR_TYPE, STR_CH_CARET);
    TOK_CH_CL_CURLY     = samMapletPut(empty, STR_TYPE, STR_CH_CL_CURLY);
    TOK_CH_CL_PAREN     = samMapletPut(empty, STR_TYPE, STR_CH_CL_PAREN);
    TOK_CH_CL_SQUARE    = samMapletPut(empty, STR_TYPE, STR_CH_CL_SQUARE);
    TOK_CH_COLON        = samMapletPut(empty, STR_TYPE, STR_CH_COLON);
    TOK_CH_COLONCOLON   = samMapletPut(empty, STR_TYPE, STR_CH_COLONCOLON);
    TOK_CH_EQUAL        = samMapletPut(empty, STR_TYPE, STR_CH_EQUAL);
    TOK_CH_OP_CURLY     = samMapletPut(empty, STR_TYPE, STR_CH_OP_CURLY);
    TOK_CH_OP_PAREN     = samMapletPut(empty, STR_TYPE, STR_CH_OP_PAREN);
    TOK_CH_OP_SQUARE    = samMapletPut(empty, STR_TYPE, STR_CH_OP_SQUARE);
    TOK_CH_SEMICOLON    = samMapletPut(empty, STR_TYPE, STR_CH_SEMICOLON);
    TOK_ACTUALS         = samMapletPut(empty, STR_TYPE, STR_ACTUALS);
    TOK_BINDING         = samMapletPut(empty, STR_TYPE, STR_BINDING);
    TOK_CALL            = samMapletPut(empty, STR_TYPE, STR_CALL);
    TOK_EXPRESSION      = samMapletPut(empty, STR_TYPE, STR_EXPRESSION);
    TOK_FORMALS         = samMapletPut(empty, STR_TYPE, STR_FORMALS);
    TOK_FUNCTION        = samMapletPut(empty, STR_TYPE, STR_FUNCTION);
    TOK_IDENTIFIER      = samMapletPut(empty, STR_TYPE, STR_IDENTIFIER);
    TOK_INTEGER         = samMapletPut(empty, STR_TYPE, STR_INTEGER);
    TOK_LISTLET         = samMapletPut(empty, STR_TYPE, STR_LISTLET);
    TOK_LITERAL         = samMapletPut(empty, STR_TYPE, STR_LITERAL);
    TOK_MAPLET          = samMapletPut(empty, STR_TYPE, STR_MAPLET);
    TOK_RETURN          = samMapletPut(empty, STR_TYPE, STR_RETURN);
    TOK_STATEMENT       = samMapletPut(empty, STR_TYPE, STR_STATEMENT);
    TOK_STATEMENTS      = samMapletPut(empty, STR_TYPE, STR_STATEMENTS);
    TOK_STRING          = samMapletPut(empty, STR_TYPE, STR_STRING);
    TOK_UNIQLET         = samMapletPut(empty, STR_TYPE, STR_UNIQLET);
    TOK_VAR_DEF         = samMapletPut(empty, STR_TYPE, STR_VAR_DEF);
    TOK_VAR_REF         = samMapletPut(empty, STR_TYPE, STR_VAR_REF);
}

/** Documented in `consts.h` */
zvalue valueToken(zvalue token, zvalue value) {
    return samMapletPut(token, STR_VALUE, value);
}

/** Documented in `consts.h` */
zvalue highType(zvalue value) {
    return samMapletGet(value, STR_TYPE);
}

/** Documented in `consts.h` */
zvalue highValue(zvalue value) {
    return samMapletGet(value, STR_VALUE);
}

/** Documented in `consts.h` */
bool hasType(zvalue value, zvalue type) {
    return (samCompare(highType(value), type) == 0);
}

/** Documented in `consts.h` */
void assertType(zvalue value, zvalue type) {
    if (!hasType(value, type)) {
        samDie("Type mismatch.");
    }
}
