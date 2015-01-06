// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "io.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(Io0_cwd) {
    return ioCwd();
}

// Documented in spec.
FUN_IMPL_DECL(Io0_fileType) {
    zvalue path = args.elems[0];
    return ioFileType(path, true);
}

// Documented in spec.
FUN_IMPL_DECL(Io0_readDirectory) {
    return ioReadDirectory(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(Io0_readFileUtf8) {
    return ioReadFileUtf8(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(Io0_readLink) {
    return ioReadLink(args.elems[0]);
}

// Documented in spec.
FUN_IMPL_DECL(Io0_writeFileUtf8) {
    ioWriteFileUtf8(args.elems[0], args.elems[1]);
    return NULL;
}
