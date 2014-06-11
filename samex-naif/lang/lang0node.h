/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Node constructors and related helpers. These functions are
 * (near-)transliterations of the same-named functions in the module
 * `core.Lang0Node`.
 */

#ifndef _LANG0NODE_H_
#define _LANG0NODE_H_

#include "type/Value.h"

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
bool isExpression(zvalue node);

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
