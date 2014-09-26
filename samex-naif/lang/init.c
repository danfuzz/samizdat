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
zevalType langSymbolMap[DAT_MAX_SYMBOLS];

// This provides the non-inline version of this function.
extern zevalType recordEvalType(zvalue record);

// This provides the non-inline version of this function.
extern bool recordEvalTypeIs(zvalue record, zevalType type);

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

    memset(langSymbolMap, 0, sizeof(langSymbolMap));

    #define REC_MAP(name) \
        langSymbolMap[symbolIndex(SYM(name))] = EVAL_##name;
    #define SYM_MAP(name) \
        langSymbolMap[symbolIndex(SYM(name))] = EVAL_##name;

    REC_MAP(apply);
    REC_MAP(call);
    REC_MAP(closure);
    REC_MAP(directive);
    REC_MAP(export);
    REC_MAP(exportSelection);
    REC_MAP(external);
    REC_MAP(fetch);
    REC_MAP(importModule);
    REC_MAP(importModuleSelection);
    REC_MAP(importResource);
    REC_MAP(internal);
    REC_MAP(literal);
    REC_MAP(mapping);
    REC_MAP(maybe);
    REC_MAP(module);
    REC_MAP(noYield);
    REC_MAP(nonlocalExit);
    REC_MAP(store);
    REC_MAP(varRef);
    REC_MAP(varDef);
    REC_MAP(varDefMutable);
    REC_MAP(void);
    REC_MAP(yield);

    SYM_MAP(CH_PLUS);
    SYM_MAP(CH_QMARK);
    SYM_MAP(CH_STAR);
}
