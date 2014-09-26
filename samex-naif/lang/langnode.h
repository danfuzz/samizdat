// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Node constructors and related helpers. These functions are
// (near-)transliterations of the same-named functions in the module
// `core.LangNode`.
//

#ifndef _LANGNODE_H_
#define _LANGNODE_H_

#include "type/Value.h"


/** Equivalent to `REFS::<name>` in the spec. */
#define REFS(name) (makeVarFetch(SYM(name)))

/** Equivalent to `SYMS::<name>` in the spec. */
#define SYMS(name) (makeLiteral(SYM(name)))

// Documented in spec.
bool canYieldVoid(zvalue node);

// Documented in spec.
zvalue formalsMaxArgs(zvalue formals);

// Documented in spec.
zvalue formalsMinArgs(zvalue formals);

// Documented in spec.
zvalue get_baseName(zvalue source);

// Documented in spec.
zvalue get_definedNames(zvalue node);

// Documented in spec.
bool isExpression(zvalue node);

// Documented in spec.
zvalue makeApply(zvalue function, zvalue values);

// Documented in spec.
zvalue makeAssignmentIfPossible(zvalue target, zvalue value);

// Documented in spec.
zvalue makeBasicClosure(zvalue map);

// Documented in spec.
zvalue makeCall(zvalue function, zvalue values);

// Documented in spec.
zvalue makeCallOrApply(zvalue function, zvalue values);

// Documented in spec.
zvalue makeDynamicImport(zvalue node);

// Documented in spec.
zvalue makeExport(zvalue node);

// Documented in spec.
zvalue makeExportSelection(zvalue names);

// Documented in spec.
zvalue makeFullClosure(zvalue nodeOrMap);

// Documented in spec.
zvalue makeImport(zvalue baseData);

// Documented in spec.
zvalue makeInfoTable(zvalue node);

// Documented in spec.
zvalue makeInterpolate(zvalue node);

// Documented in spec.
zvalue makeLiteral(zvalue value);

// Documented in spec.
zvalue makeMapExpression(zvalue mappings);

// Documented in spec.
zvalue makeMaybe(zvalue value);

// Documented in spec.
zvalue makeMaybeValue(zvalue expression);

// Documented in spec.
zvalue makeNoYield(zvalue value);

/** Documented in spec. */
zvalue makeNonlocalExit(zvalue function, zvalue optValue);

// Documented in spec.
zvalue makeSymbolLiteral(zvalue name);

// Documented in spec.
zvalue makeSymbolTableExpression(zvalue mappings);

// Documented in spec.
zvalue makeThunk(zvalue expression);

// Documented in spec.
zvalue makeVarDef(zvalue name, zvalue value);

// Documented in spec.
zvalue makeVarDefMutable(zvalue name, zvalue value);

// Documented in spec.
zvalue makeVarFetch(zvalue name);

// Documented in spec.
zvalue makeVarFetchLvalue(zvalue name);

// Documented in spec.
zvalue makeVarRef(zvalue name);

// Documented in spec.
zvalue makeVarStore(zvalue name, zvalue value);

// Documented in spec.
zvalue resolveImport(zvalue node, zvalue resolveFn);

// Documented in spec.
zvalue withFormals(zvalue node, zvalue formals);

// Documented in spec.
zvalue withModuleDefs(zvalue node);

// Documented in spec.
zvalue withName(zvalue node, zvalue name);

// Documented in spec.
zvalue withResolvedImports(zvalue node, zvalue resolveFn);

// Documented in spec.
zvalue withTop(zvalue node);

// Documented in spec.
zvalue withYieldDef(zvalue node, zvalue name);

// Documented in spec.
zvalue withoutInterpolate(zvalue node);

// Documented in spec.
zvalue withoutTops(zvalue node);

#endif
