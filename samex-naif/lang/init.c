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
zevalType langTypeMap[DAT_MAX_CLASSES];

// This provides the non-inline version of this function.
extern zevalType get_evalType(zvalue node);

/** Initializes the module. */
MOD_INIT(lang) {
    MOD_USE(const);
    MOD_USE(Bool);
    MOD_USE(Box);
    MOD_USE(Closure);
    MOD_USE(Generator);
    MOD_USE(Map);

    memset(langTypeMap, 0, sizeof(langTypeMap));
    langTypeMap[classIndex(CLS_apply)]                 = EVAL_apply;
    langTypeMap[classIndex(CLS_call)]                  = EVAL_call;
    langTypeMap[classIndex(CLS_closure)]               = EVAL_closure;
    langTypeMap[classIndex(CLS_fetch)]                 = EVAL_fetch;
    langTypeMap[classIndex(CLS_importModule)]          = EVAL_importModule;
    langTypeMap[classIndex(CLS_importModuleSelection)] = EVAL_importModuleSelection;
    langTypeMap[classIndex(CLS_importResource)]        = EVAL_importResource;
    langTypeMap[classIndex(CLS_literal)]               = EVAL_literal;
    langTypeMap[classIndex(CLS_maybe)]                 = EVAL_maybe;
    langTypeMap[classIndex(CLS_noYield)]               = EVAL_noYield;
    langTypeMap[classIndex(CLS_store)]                 = EVAL_store;
    langTypeMap[classIndex(CLS_varRef)]                = EVAL_varRef;
    langTypeMap[classIndex(CLS_varDef)]                = EVAL_varDef;
    langTypeMap[classIndex(CLS_varDefMutable)]         = EVAL_varDefMutable;
    langTypeMap[classIndex(CLS_void)]                  = EVAL_void;
}
