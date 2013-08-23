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
zevalType langTypeMap[PB_MAX_TYPES];

/* Documented in header. */
void langInit(void) {
    if (TYPE_NonlocalExit != NULL) {
        return;
    }

    constInit();
    langBindClosure();
    langBindNonlocalExit();

    memset(langTypeMap, 0, sizeof(langTypeMap));
    langTypeMap[EVAL_call]        = typeIndex(STR_call);
    langTypeMap[EVAL_closure]     = typeIndex(STR_closure);
    langTypeMap[EVAL_expression]  = typeIndex(STR_expression);
    langTypeMap[EVAL_interpolate] = typeIndex(STR_interpolate);
    langTypeMap[EVAL_literal]     = typeIndex(STR_literal);
    langTypeMap[EVAL_fnDef]       = typeIndex(STR_fnDef);
    langTypeMap[EVAL_varDef]      = typeIndex(STR_varDef);
    langTypeMap[EVAL_varRef]      = typeIndex(STR_varRef);
}
