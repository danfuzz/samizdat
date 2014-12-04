// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/List.h"
#include "util.h"

#include "impl.h"


//
// Exported Definitions
//

// Documented in spec.
FUN_IMPL_DECL(loop) {
    zvalue function = args.elems[0];
    for (;;) {
        zstackPointer save = datFrameStart();
        FUN_CALL(function);
        datFrameReturn(save, NULL);
    }
}

// Documented in spec.
FUN_IMPL_DECL(maybeValue) {
    zvalue function = args.elems[0];
    zvalue value = FUN_CALL(function);

    return (value == NULL) ? EMPTY_LIST : listFromValue(value);
}
