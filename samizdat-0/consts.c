/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"

#include <stddef.h>

zvalue STR_CH_AT = NULL;
zvalue STR_CH_CARET = NULL;
zvalue STR_CH_CL_CURLY = NULL;
zvalue STR_CH_CL_PAREN = NULL;
zvalue STR_CH_CL_SQUARE = NULL;
zvalue STR_CH_COLON = NULL;
zvalue STR_CH_EQUAL = NULL;
zvalue STR_CH_OP_CURLY = NULL;
zvalue STR_CH_OP_PAREN = NULL;
zvalue STR_CH_OP_SQUARE = NULL;
zvalue STR_CH_SEMICOLON = NULL;
zvalue STR_ARGUMENT_SPECS = NULL;
zvalue STR_BINDING = NULL;
zvalue STR_CALL = NULL;
zvalue STR_EXPRESSION_LIST = NULL;
zvalue STR_FUNCTION = NULL;
zvalue STR_IDENTIFIER = NULL;
zvalue STR_INTEGER = NULL;
zvalue STR_LISTLET = NULL;
zvalue STR_LITERAL = NULL;
zvalue STR_MAPLET = NULL;
zvalue STR_PROGRAM = NULL;
zvalue STR_RETURN = NULL;
zvalue STR_STRING = NULL;
zvalue STR_TYPE = NULL;
zvalue STR_UNIQLET = NULL;
zvalue STR_VALUE = NULL;
zvalue STR_VAR_DEF = NULL;
zvalue STR_VAR_REF = NULL;

/* Prefab tokens / nodes. Lazily initialized. */
zvalue TOK_CH_AT = NULL;
zvalue TOK_CH_CARET = NULL;
zvalue TOK_CH_CL_CURLY = NULL;
zvalue TOK_CH_CL_PAREN = NULL;
zvalue TOK_CH_CL_SQUARE = NULL;
zvalue TOK_CH_COLON = NULL;
zvalue TOK_CH_EQUAL = NULL;
zvalue TOK_CH_OP_CURLY = NULL;
zvalue TOK_CH_OP_PAREN = NULL;
zvalue TOK_CH_OP_SQUARE = NULL;
zvalue TOK_CH_SEMICOLON = NULL;
zvalue TOK_ARGUMENT_SPECS = NULL;
zvalue TOK_BINDING = NULL;
zvalue TOK_CALL = NULL;
zvalue TOK_EXPRESSION_LIST = NULL;
zvalue TOK_FUNCTION = NULL;
zvalue TOK_IDENTIFIER = NULL;
zvalue TOK_INTEGER = NULL;
zvalue TOK_LISTLET = NULL;
zvalue TOK_LITERAL = NULL;
zvalue TOK_MAPLET = NULL;
zvalue TOK_PROGRAM = NULL;
zvalue TOK_RETURN = NULL;
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
    STR_CH_CARET        = samStringletFromUtf8String("^", -1);
    STR_CH_CL_CURLY     = samStringletFromUtf8String("}", -1);
    STR_CH_CL_PAREN     = samStringletFromUtf8String(")", -1);
    STR_CH_CL_SQUARE    = samStringletFromUtf8String("]", -1);
    STR_CH_COLON        = samStringletFromUtf8String(":", -1);
    STR_CH_EQUAL        = samStringletFromUtf8String("=", -1);
    STR_CH_OP_CURLY     = samStringletFromUtf8String("{", -1);
    STR_CH_OP_PAREN     = samStringletFromUtf8String("(", -1);
    STR_CH_OP_SQUARE    = samStringletFromUtf8String("[", -1);
    STR_CH_SEMICOLON    = samStringletFromUtf8String(";", -1);
    STR_ARGUMENT_SPECS  = samStringletFromUtf8String("argumentSpecs", -1);
    STR_BINDING         = samStringletFromUtf8String("binding", -1);
    STR_CALL            = samStringletFromUtf8String("call", -1);
    STR_EXPRESSION_LIST = samStringletFromUtf8String("expressionList", -1);
    STR_FUNCTION        = samStringletFromUtf8String("function", -1);
    STR_IDENTIFIER      = samStringletFromUtf8String("identifier", -1);
    STR_IDENTIFIER      = samStringletFromUtf8String("identifier", -1);
    STR_INTEGER         = samStringletFromUtf8String("integer", -1);
    STR_LISTLET         = samStringletFromUtf8String("listlet", -1);
    STR_LITERAL         = samStringletFromUtf8String("literal", -1);
    STR_MAPLET          = samStringletFromUtf8String("maplet", -1);
    STR_PROGRAM         = samStringletFromUtf8String("program", -1);
    STR_RETURN          = samStringletFromUtf8String("return", -1);
    STR_STRING          = samStringletFromUtf8String("string", -1);
    STR_TYPE            = samStringletFromUtf8String("type", -1);
    STR_UNIQLET         = samStringletFromUtf8String("uniqlet", -1);
    STR_VALUE           = samStringletFromUtf8String("value", -1);
    STR_VAR_DEF         = samStringletFromUtf8String("varDef", -1);
    STR_VAR_REF         = samStringletFromUtf8String("varRef", -1);

    zvalue empty = samMapletEmpty();
    TOK_CH_AT           = samMapletPut(empty, STR_TYPE, STR_CH_AT);
    TOK_CH_CARET        = samMapletPut(empty, STR_TYPE, STR_CH_CARET);
    TOK_CH_CL_CURLY     = samMapletPut(empty, STR_TYPE, STR_CH_CL_CURLY);
    TOK_CH_CL_PAREN     = samMapletPut(empty, STR_TYPE, STR_CH_CL_PAREN);
    TOK_CH_CL_SQUARE    = samMapletPut(empty, STR_TYPE, STR_CH_CL_SQUARE);
    TOK_CH_COLON        = samMapletPut(empty, STR_TYPE, STR_CH_COLON);
    TOK_CH_EQUAL        = samMapletPut(empty, STR_TYPE, STR_CH_EQUAL);
    TOK_CH_OP_CURLY     = samMapletPut(empty, STR_TYPE, STR_CH_OP_CURLY);
    TOK_CH_OP_PAREN     = samMapletPut(empty, STR_TYPE, STR_CH_OP_PAREN);
    TOK_CH_OP_SQUARE    = samMapletPut(empty, STR_TYPE, STR_CH_OP_SQUARE);
    TOK_CH_SEMICOLON    = samMapletPut(empty, STR_TYPE, STR_CH_SEMICOLON);
    TOK_ARGUMENT_SPECS  = samMapletPut(empty, STR_TYPE, STR_ARGUMENT_SPECS);
    TOK_BINDING         = samMapletPut(empty, STR_TYPE, STR_BINDING);
    TOK_CALL            = samMapletPut(empty, STR_TYPE, STR_CALL);
    TOK_EXPRESSION_LIST = samMapletPut(empty, STR_TYPE, STR_EXPRESSION_LIST);
    TOK_FUNCTION        = samMapletPut(empty, STR_TYPE, STR_FUNCTION);
    TOK_IDENTIFIER      = samMapletPut(empty, STR_TYPE, STR_IDENTIFIER);
    TOK_INTEGER         = samMapletPut(empty, STR_TYPE, STR_INTEGER);
    TOK_LISTLET         = samMapletPut(empty, STR_TYPE, STR_LISTLET);
    TOK_LITERAL         = samMapletPut(empty, STR_TYPE, STR_LITERAL);
    TOK_MAPLET          = samMapletPut(empty, STR_TYPE, STR_MAPLET);
    TOK_PROGRAM         = samMapletPut(empty, STR_TYPE, STR_PROGRAM);
    TOK_RETURN          = samMapletPut(empty, STR_TYPE, STR_RETURN);
    TOK_STRING          = samMapletPut(empty, STR_TYPE, STR_STRING);
    TOK_UNIQLET         = samMapletPut(empty, STR_TYPE, STR_UNIQLET);
    TOK_VAR_DEF         = samMapletPut(empty, STR_TYPE, STR_VAR_DEF);
    TOK_VAR_REF         = samMapletPut(empty, STR_TYPE, STR_VAR_REF);
}
