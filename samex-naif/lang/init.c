// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Initialization
//

#include <stddef.h>
#include <string.h>

#include "const.h"
#include "type/Class.h"

#include "impl.h"


//
// Module Definitions
//

// Documented in header.
zevalType langTypeMap[DAT_MAX_TYPES];

// This provides the non-inline version of this function.
extern zevalType get_evalType(zvalue node);

/** Initializes the module. */
MOD_INIT(lang) {
    MOD_USE(const);
    MOD_USE(Box);
    MOD_USE(Closure);
    MOD_USE(Generator);
    MOD_USE(Map);

    memset(langTypeMap, 0, sizeof(langTypeMap));
    langTypeMap[classIndex(TYPE_apply)]                 = EVAL_apply;
    langTypeMap[classIndex(TYPE_call)]                  = EVAL_call;
    langTypeMap[classIndex(TYPE_closure)]               = EVAL_closure;
    langTypeMap[classIndex(TYPE_fetch)]                 = EVAL_fetch;
    langTypeMap[classIndex(TYPE_importModule)]          = EVAL_importModule;
    langTypeMap[classIndex(TYPE_importModuleSelection)] = EVAL_importModuleSelection;
    langTypeMap[classIndex(TYPE_importResource)]        = EVAL_importResource;
    langTypeMap[classIndex(TYPE_literal)]               = EVAL_literal;
    langTypeMap[classIndex(TYPE_maybe)]                 = EVAL_maybe;
    langTypeMap[classIndex(TYPE_noYield)]               = EVAL_noYield;
    langTypeMap[classIndex(TYPE_store)]                 = EVAL_store;
    langTypeMap[classIndex(TYPE_varRef)]                = EVAL_varRef;
    langTypeMap[classIndex(TYPE_varDef)]                = EVAL_varDef;
    langTypeMap[classIndex(TYPE_varDefMutable)]         = EVAL_varDefMutable;
    langTypeMap[classIndex(TYPE_void)]                  = EVAL_void;
}
