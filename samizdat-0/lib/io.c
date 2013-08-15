/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "io.h"
#include "impl.h"
#include "util.h"

#include <stdio.h>


/*
 * Private Definitions
 */

/**
 * Emits a note.
 */
static void emitNote(zvalue message) {
    zint size = utf8SizeFromString(message);
    char str[size + 1];
    utf8FromString(size + 1, str, message);

    fwrite(str, 1, size, stderr);
    fputc('\n', stderr);
}


/*
 * Exported Definitions
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0Die) {
    if (argCount == 1) {
        emitNote(args[0]);
    }

    die("Alas.");
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0Note) {
    emitNote(args[0]);
    return NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0FlatCwd) {
    return ioFlatCwd();
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0FlatReadFileUtf8) {
    return ioFlatReadFileUtf8(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0FlatReadLink) {
    return ioFlatReadLink(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0FlatWriteFileUtf8) {
    ioFlatWriteFileUtf8(args[0], args[1]);
    return NULL;
}
