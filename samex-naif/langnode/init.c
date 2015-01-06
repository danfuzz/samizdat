// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Initialization
//

#include "langnode.h"
#include "type/Class.h"


//
// Module Definitions
//

// Documented in header.
znodeType nodeSymbolMap[DAT_MAX_SYMBOLS];

// This provides the non-inline version of this function.
extern znodeType nodeRecType(zvalue record);

// This provides the non-inline version of this function.
extern bool nodeRecTypeIs(zvalue record, znodeType type);

// This provides the non-inline version of this function.
extern znodeType nodeSymbolType(zvalue symbol);

/** Initializes the module. */
MOD_INIT(langnode) {
    MOD_USE(cls);
    MOD_USE(lang_consts);

    utilZero(nodeSymbolMap);

    #define SYM_MAP(name) nodeSymbolMap[SYMIDX(name)] = NODE_##name;

    SYM_MAP(CH_PLUS);
    SYM_MAP(CH_QMARK);
    SYM_MAP(CH_STAR);
    SYM_MAP(apply);
    SYM_MAP(call);
    SYM_MAP(cell);
    SYM_MAP(closure);
    SYM_MAP(directive);
    SYM_MAP(export);
    SYM_MAP(exportSelection);
    SYM_MAP(external);
    SYM_MAP(fetch);
    SYM_MAP(importModule);
    SYM_MAP(importModuleSelection);
    SYM_MAP(importResource);
    SYM_MAP(internal);
    SYM_MAP(lazy);
    SYM_MAP(literal);
    SYM_MAP(mapping);
    SYM_MAP(maybe);
    SYM_MAP(module);
    SYM_MAP(noYield);
    SYM_MAP(nonlocalExit);
    SYM_MAP(promise);
    SYM_MAP(result);
    SYM_MAP(store);
    SYM_MAP(varRef);
    SYM_MAP(varDef);
    SYM_MAP(void);
    SYM_MAP(yield);
}
