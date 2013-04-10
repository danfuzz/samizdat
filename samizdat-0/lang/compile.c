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
        zvalue value = datMapletGet(one, STR_VALUE);
        const char *typeStr = datStringletEncodeUtf8(type, NULL);
        const char *valueStr = "";

        if ((value != NULL) && (datType(value) == DAT_LISTLET)) {
            valueStr = datStringletEncodeUtf8(value, NULL);
        }

        note("=== %4lld -- %s %s", i, typeStr, valueStr);
    }
    note("[fin]");
#endif

    return parse(tokens);
}
