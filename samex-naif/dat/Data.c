/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Data` data type
 */

#include "type/Data.h"

#include "impl.h"


/*
 * Type Definition
 */

/** Initializes the module. */
MOD_INIT(Data) {
    MOD_USE(Value);

    // Note: The `typeSystem` module initializes `TYPE_Data`.
    // So, there's nothing to do here, at least for now.
}

/* Documented in header. */
zvalue TYPE_Data = NULL;
