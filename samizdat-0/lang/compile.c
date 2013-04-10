/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>

/*
 * Exported functions
 */

/* Documented in header. */
zvalue langCompile(zvalue programText) {
    zvalue tokens = tokenize(programText);

    // TODO: Remove this file dump.
#if 0
    zint tokensSize = datSize(tokens);
    for (zint i = 0; i < tokensSize; i++) {
        zvalue one = datListletGet(tokens, i);
        zvalue type = datMapletGet(one, STR_TYPE);
        char typeStr[200];
        char value[200] = "";
        zint size = datStringletUtf8Size(type);
        datStringletEncodeUtf8(type, typeStr);
        typeStr[size] = '\0';
        one = datMapletGet(one, STR_VALUE);
        if ((one != NULL) && (datType(one) == DAT_LISTLET)) {
            size = datStringletUtf8Size(one);
            datStringletEncodeUtf8(one, value);
            value[size] = '\0';
        }
        note("=== %4lld -- %s %s", i, typeStr, value);
    }
    note("[fin]");
#endif

    return parse(tokens);
}
