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
zevalType langClassMap[DAT_MAX_CLASSES];

// Documented in header.
zevalType langSymbolMap[DAT_MAX_SYMBOLS];

// This provides the non-inline version of this function.
extern zevalType classEvalType(zvalue node);

// This provides the non-inline version of this function.
extern zevalType symbolEvalType(zvalue symbol);

/** Initializes the module. */
MOD_INIT(lang) {
    MOD_USE(lang_const);
    MOD_USE(Bool);
    MOD_USE(Box);
    MOD_USE(Closure);
    MOD_USE(Generator);
    MOD_USE(Map);

    memset(langClassMap, 0, sizeof(langClassMap));
    langClassMap[classIndex(CLS_apply)]                 = EVAL_apply;
    langClassMap[classIndex(CLS_call)]                  = EVAL_call;
    langClassMap[classIndex(CLS_closure)]               = EVAL_closure;
    langClassMap[classIndex(CLS_fetch)]                 = EVAL_fetch;
    langClassMap[classIndex(CLS_importModule)]          = EVAL_importModule;
    langClassMap[classIndex(CLS_importModuleSelection)] = EVAL_importModuleSelection;
    langClassMap[classIndex(CLS_importResource)]        = EVAL_importResource;
    langClassMap[classIndex(CLS_literal)]               = EVAL_literal;
    langClassMap[classIndex(CLS_maybe)]                 = EVAL_maybe;
    langClassMap[classIndex(CLS_noYield)]               = EVAL_noYield;
    langClassMap[classIndex(CLS_store)]                 = EVAL_store;
    langClassMap[classIndex(CLS_varRef)]                = EVAL_varRef;
    langClassMap[classIndex(CLS_varDef)]                = EVAL_varDef;
    langClassMap[classIndex(CLS_varDefMutable)]         = EVAL_varDefMutable;
    langClassMap[classIndex(CLS_void)]                  = EVAL_void;

    memset(langSymbolMap, 0, sizeof(langSymbolMap));
    langSymbolMap[symbolIndex(SYM_CH_PLUS)]  = EVAL_CH_PLUS;
    langSymbolMap[symbolIndex(SYM_CH_QMARK)] = EVAL_CH_QMARK;
    langSymbolMap[symbolIndex(SYM_CH_STAR)]  = EVAL_CH_STAR;
}
