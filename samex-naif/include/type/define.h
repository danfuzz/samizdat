// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Definitions for defining new classes and functions.
//
// This is meant to be `#include`d from non-header source files that define
// classes and/or functions.

#ifndef _TYPE_DEFINE_H_
#define _TYPE_DEFINE_H_

#include "type/Class.h"
#include "type/Function.h"
#include "type/Generic.h"
#include "type/Selector.h"
#include "type/String.h"

/** Performs binding of the indicated method. */
#define METH_BIND(cls, name) \
    do { \
        genericBindPrim(GFN_##name, CLS_##cls, METH_NAME(cls, name), \
            #cls ":" #name); \
    } while(0)

#endif
