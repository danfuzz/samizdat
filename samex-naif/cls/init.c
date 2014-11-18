// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "impl.h"


//
// Module init
//

/** Initializes the module. */
MOD_INIT(cls) {
    MOD_USE(Bool);
    MOD_USE(Map);
    MOD_USE(Null);
}
