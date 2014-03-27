/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "io.h"
#include "impl.h"


/*
 * Exported Definitions
 */

/* Documented in spec. */
FUN_IMPL_DECL(Io0_flatCwd) {
    return ioCwd();
}

/* Documented in spec. */
FUN_IMPL_DECL(Io0_flatFileExists) {
    zvalue path = args[0];

    return ioFileExists(path) ? path : NULL;
}

/* Documented in spec. */
FUN_IMPL_DECL(Io0_flatReadFileUtf8) {
    return ioReadFileUtf8(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(Io0_flatReadLink) {
    return ioReadLink(args[0]);
}

/* Documented in spec. */
FUN_IMPL_DECL(Io0_flatWriteFileUtf8) {
    ioWriteFileUtf8(args[0], args[1]);
    return NULL;
}
