/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#define UNARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        requireExactly(argCount, 1); \
        return datIntletFromInt(op datIntFromIntlet(args[0])); \
    } \
    extern int semicolonRequiredHere

#define BINARY_PRIM(name, op) \
    PRIM_IMPL(name) { \
        requireExactly(argCount, 2); \
        return datIntletFromInt( \
            datIntFromIntlet(args[0]) op datIntFromIntlet(args[1])); \
    } \
    extern int semicolonRequiredHere

/* These are all documented in Samizdat Layer 0 spec. */

UNARY_PRIM(ineg, -);
UNARY_PRIM(inot, ~);

BINARY_PRIM(iadd, +);
BINARY_PRIM(iand, &);
BINARY_PRIM(idiv, /);
BINARY_PRIM(imod, %);
BINARY_PRIM(imul, *);
BINARY_PRIM(ior,  |);
BINARY_PRIM(isub, -);
BINARY_PRIM(ixor, ^);
