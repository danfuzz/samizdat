// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Definitions for defining new classes and functions.
//

#ifndef _TYPE_DEFINE_H_
#define _TYPE_DEFINE_H_

#include "type/Class.h"
#include "type/Function.h"
#include "type/Generic.h"
#include "type/Selector.h"
#include "type/String.h"

/** C function name for a method on the given class with the given name. */
#define METH_NAME(cls, name) cls##_##name

/** Declaration for a method on the given class with the given name. */
#define METH_IMPL(cls, name) \
    static zvalue METH_NAME(cls, name)( \
        zvalue thisFunction, zint argCount, const zvalue *args)

/** Performs binding of the indicated method. */
#define METH_BIND(cls, name) \
    do { \
        genericBindPrim(GFN_##name, CLS_##cls, METH_NAME(cls, name), \
            #cls ":" #name); \
    } while(0)

#endif
