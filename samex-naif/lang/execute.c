// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Node evaluation / execution
//

#include "type/Box.h"
#include "type/SymbolTable.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in header.
zvalue langEval0(zvalue env, zvalue node) {
    zint size = get_size(env);
    zmapping mappings[size];

    arrayFromSymtab(mappings, env);
    for (zint i = 0; i < size; i++) {
        mappings[i].value = cm_new(Result, mappings[i].value);
    }
    env = symtabFromZassoc((zassoc) {size, mappings});

    Frame frame;
    frameInit(&frame, NULL, NULL, env);

    exnoConvert(&node);
    return exnoExecute(node, &frame);
}
