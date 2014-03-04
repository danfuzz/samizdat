/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Initialization
 */

#include "const.h"
#include "impl.h"
#include "type/Type.h"

#include <stddef.h>
#include <string.h>



/*
 * Module Definitions
 */

/* Documented in header. */
zevalType langTypeMap[DAT_MAX_TYPES];

/* Documented in header. */
extern zevalType evalTypeOf(zvalue node);

/** Initializes the module. */
MOD_INIT(lang) {
    MOD_USE(const);
    MOD_USE(Box);
    MOD_USE(Closure);
    MOD_USE(Generator);
    MOD_USE(Map);

    memset(langTypeMap, 0, sizeof(langTypeMap));
    langTypeMap[typeIndex(STR_apply)]         = EVAL_apply;
    langTypeMap[typeIndex(STR_call)]          = EVAL_call;
    langTypeMap[typeIndex(STR_closure)]       = EVAL_closure;
    langTypeMap[typeIndex(STR_expression)]    = EVAL_expression;
    langTypeMap[typeIndex(STR_interpolate)]   = EVAL_interpolate;
    langTypeMap[typeIndex(STR_jump)]          = EVAL_jump;
    langTypeMap[typeIndex(STR_literal)]       = EVAL_literal;
    langTypeMap[typeIndex(STR_varBind)]       = EVAL_varBind;
    langTypeMap[typeIndex(STR_varDef)]        = EVAL_varDef;
    langTypeMap[typeIndex(STR_varDefMutable)] = EVAL_varDefMutable;
    langTypeMap[typeIndex(STR_varRef)]        = EVAL_varRef;
}
