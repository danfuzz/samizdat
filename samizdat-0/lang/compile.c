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
    // TODO: Remove this file dump.
    note("File contents:");
    zint size = datStringletUtf8Size(programText);
    char utf[size + 1];
    datStringletEncodeUtf8(programText, utf);
    utf[size] = '\0';
    note("%s", utf);
    note("[fin]");

    zvalue tokens = tokenize(programText);

    // TODO: Remove this file dump.
    zint tokensSize = datSize(tokens);
    for (zint i = 0; i < tokensSize; i++) {
        zvalue one = datListletGet(tokens, i);
        zvalue type = datMapletGet(one, STR_TYPE);
        char value[200] = "";
        size = datStringletUtf8Size(type);
        datStringletEncodeUtf8(type, utf);
        utf[size] = '\0';
        one = datMapletGet(one, STR_VALUE);
        if ((one != NULL) && (datType(one) == DAT_LISTLET)) {
            size = datStringletUtf8Size(one);
            datStringletEncodeUtf8(one, value);
            value[size] = '\0';
        }
        note("token %s %s", utf, value);
    }
    note("[fin]");

    return parse(tokens);
}
