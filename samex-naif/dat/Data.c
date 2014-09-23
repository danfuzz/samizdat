// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// `Data` class
//

#include "type/Data.h"
#include "type/define.h"

#include "impl.h"


//
// Class Definition
//

/** Initializes the module. */
MOD_INIT(Data) {
    MOD_USE(Value);

    CLS_Data = makeCoreClass("Data", CLS_Value, NULL, NULL);
}

// Documented in header.
zvalue CLS_Data = NULL;
