/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * `Generic` data type
 */

#ifndef _TYPE_GENERIC_H_
#define _TYPE_GENERIC_H_

#include "pb.h"
#include "type/Function.h"

/** Flags passed to `makeGeneric`. */
typedef enum {
    /** No flags. */
    GFN_NONE = 0,

    /**
     * Indicates that all arguments to the function must be of the same type.
     */
    GFN_SAME_TYPE = 1
} zgenericFlags;

/** C function name for a method on the given type with the given name. */
#define METH_NAME(type, name) type##_##name

/** Declaration for a method on the given type with the given name. */
#define METH_IMPL(type, name) \
    static zvalue METH_NAME(type, name)(zint argCount, const zvalue *args)

/** Performs binding of the indicated method. */
#define METH_BIND(type, name) \
    do { \
        genericBindPrim(GFN_##name, TYPE_##type, METH_NAME(type, name), \
            #type ":" #name); \
    } while(0)

/** Type value for in-model type `Generic`. */
extern zvalue TYPE_Generic;

/**
 * Adds a type-to-function binding to the given generic. `generic` must be
 * a generic function, `typeOrName` is the usual representation of a type,
 * and `function` must be a valid `Fallable`. The type must not have already
 * been bound in the given generic, and the generic must not be sealed.
 */
void genericBind(zvalue generic, zvalue typeOrName, zvalue function);

/**
 * Adds a type-to-C-function binding to the given generic. `generic` must
 * be a generic function, `typeOrName` is the usual representation of a type,
 * and `function` must be a valid `zfunction`. The type must not have already
 * been bound in the given generic, and the generic must not be sealed.
 * An optional `builtinName` becomes the name of the bound builtin.
 */
void genericBindPrim(zvalue generic, zvalue typeOrName, zfunction function,
    const char *builtinName);

/**
 * Seal the given generic. This prevents it from gaining any new bindings.
 * `generic` must be a generic function.
 */
void genericSeal(zvalue generic);

/**
 * Constructs and returns a generic function with the given argument
 * restrictions and optional name (used when producing stack traces). It is
 * initially unsealed and without any bindings. `minArgs` must be at least
 * `1`, and `maxArgs` must be either greater than `minArgs` or `-1` to indicate
 * that there is no limit.
 */
zvalue makeGeneric(zint minArgs, zint maxArgs, zgenericFlags flags,
        zvalue name);

#endif
