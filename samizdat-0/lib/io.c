/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "io.h"
#include "impl.h"

#include <stdio.h>
#include <stdlib.h>


/*
 * Helper functions
 */

/**
 * Emits a note.
 */
static void emitNote(zvalue message) {
    zint size = 0;
    const char *str = datStringletEncodeUtf8(message, &size);

    fwrite(str, 1, size, stderr);
    fputc('\n', stderr);
}

/*
 * Primitive implementations (exported via the context)
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0Die) {
    requireRange(argCount, 0, 1);

    zvalue message =
        (argCount == 1) ? args[0] : datStringletFromUtf8String(-1, "Alas");
    emitNote(message);
    exit(1);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0Note) {
    requireExactly(argCount, 1);
    emitNote(args[0]);
    return NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0PathFromStringlet) {
    requireExactly(argCount, 1);
    return ioPathListletFromUtf8(datStringletEncodeUtf8(args[0], NULL));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0ReadFileUtf8) {
    requireExactly(argCount, 1);
    return ioReadFileUtf8(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(io0WriteFileUtf8) {
    requireExactly(argCount, 2);
    ioWriteFileUtf8(args[0], args[1]);
    return NULL;
}
