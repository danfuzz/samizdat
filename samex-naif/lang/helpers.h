/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Node constructors and related helpers. Other than the collection
 * constructors, these functions are (near-)transliterations of the same-named
 * functions in the module `core.Lang0Node`.
 */

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include "type/Value.h"


/**
 * Makes a 0-1 mapping map.
 */
zvalue mapFrom1(zvalue k1, zvalue v1);

/**
 * Makes a 0-2 mapping map.
 */
zvalue mapFrom2(zvalue k1, zvalue v1, zvalue k2, zvalue v2);

/**
 * Makes a 0-3 mapping map.
 */
zvalue mapFrom3(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3);

/**
 * Makes a 0-4 mapping map. Values are allowed to be `NULL`, in
 * which case the corresponding key isn't included in the result.
 */
zvalue mapFrom4(zvalue k1, zvalue v1, zvalue k2, zvalue v2,
        zvalue k3, zvalue v3, zvalue k4, zvalue v4);

/**
 * Makes a 1 element list.
 */
zvalue listFrom1(zvalue e1);

/**
 * Makes a 2 element list.
 */
zvalue listFrom2(zvalue e1, zvalue e2);

/**
 * Makes a 3 element list.
 */
zvalue listFrom3(zvalue e1, zvalue e2, zvalue e3);

/**
 * Appends an element to a list.
 */
zvalue listAppend(zvalue list, zvalue elem);

/** Equivalent to `REFS::<name>` in the spec. */
#define REFS(name) (makeVarRef(STR_##name))

/** Equivalent to `get_<name>(node)` in the spec. */
#define GET(name, node) (get((node), STR_##name))

/* Documented in spec. */
zvalue formalsMaxArgs(zvalue formals);

/* Documented in spec. */
zvalue formalsMinArgs(zvalue formals);

/* Documented in spec. */
zvalue get_baseName(zvalue source);

/* Documented in spec. */
zvalue get_definedNames(zvalue node);

/* Documented in spec. */
zvalue makeApply(zvalue function, zvalue actuals);

/* Documented in spec. */
zvalue makeBasicClosure(zvalue map);

/* Documented in spec. */
zvalue makeCall(zvalue function, zvalue actuals);

/* Documented in spec. */
zvalue makeCallOrApply(zvalue function, zvalue actuals);

/* Documented in spec. */
zvalue makeDynamicImport(zvalue node);

/* Documented in spec. */
zvalue makeExport(zvalue node);

/* Documented in spec. */
zvalue makeExportSelection(zvalue names);

/* Documented in spec. */
zvalue makeFullClosure(zvalue map);

/* Documented in spec. */
zvalue makeImport(zvalue baseData);

/* Documented in spec. */
zvalue makeInfoMap(zvalue node);

/* Documented in spec. */
zvalue makeInterpolate(zvalue node);

/* Documented in spec. */
zvalue makeLiteral(zvalue value);

/* Documented in spec. */
zvalue makeMaybe(zvalue value);

/* Documented in spec. */
zvalue makeMaybeValue(zvalue expression);

/* Documented in spec. */
zvalue makeNoYield(zvalue value);

/** Documented in spec. */
zvalue makeNonlocalExit(zvalue function, zvalue optValue);

/* Documented in spec. */
zvalue makeThunk(zvalue expression);

/* Documented in spec. */
zvalue makeVarBind(zvalue name, zvalue value);

/* Documented in spec. */
zvalue makeVarDef(zvalue name, zvalue value);

/* Documented in spec. */
zvalue makeVarDefMutable(zvalue name, zvalue value);

/* Documented in spec. */
zvalue makeVarRef(zvalue name);

/* Documented in spec. */
zvalue resolveImport(zvalue node, zvalue resolveFn);

/* Documented in spec. */
zvalue withFormals(zvalue node, zvalue formals);

/* Documented in spec. */
zvalue withModuleDefs(zvalue node);

/* Documented in spec. */
zvalue withResolvedImports(zvalue node, zvalue resolveFn);

/* Documented in spec. */
zvalue withTop(zvalue node);

/* Documented in spec. */
zvalue withoutInterpolate(zvalue node);

/* Documented in spec. */
zvalue withoutTops(zvalue node);

#endif
