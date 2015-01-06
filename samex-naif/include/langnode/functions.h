// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Node constructors and accessors. These functions are transliterations or
// near-transliterations of the same-named functions in the module
// `core.LangNode`.
//

#ifndef _LANGNODE_FUNCTIONS_H_
#define _LANGNODE_FUNCTIONS_H_


//
// Parallel to `constructors1.sam`:
//

// These are all documented in spec.
zvalue makeApply(zvalue target, zvalue name, zvalue values);
zvalue makeBasicClosure(zvalue map);
zvalue makeCall(zvalue target, zvalue name, zvalue values);
zvalue makeExport(zvalue node);
zvalue makeExportSelection(zvalue names);
zvalue makeFunCall(zvalue function, zvalue values);
zvalue makeMaybe(zvalue value);
zvalue makeNoYield(zvalue value);
zvalue makeNonlocalExit(zvalue function, zvalue optValue);
zvalue makeVarDef(zvalue name, zvalue box, zvalue optValue);
zvalue makeVarFetch(zvalue name);
zvalue makeVarFetchGeneral(zvalue name);
zvalue makeVarRef(zvalue name);
zvalue makeVarStore(zvalue name, zvalue value);
zvalue withFormals(zvalue node, zvalue formals);
zvalue withName(zvalue node, zvalue name);
zvalue withYieldDef(zvalue node, zvalue name);
zvalue withoutIntermediates(zvalue node);


//
// Parallel to `constructors2.sam`:
//

// These are all documented in spec.
zvalue makeAssignmentIfPossible(zvalue target, zvalue value);
zvalue makeCallGeneral(zvalue target, zvalue name, zvalue values);
zvalue makeClassDef(zvalue name, zvalue attributes, zvalue methods);
zvalue makeDynamicImport(zvalue node);
zvalue makeFullClosure(zvalue base);
zvalue makeFunCallGeneral(zvalue function, zvalue values);
zvalue makeImport(zvalue baseData);
zvalue makeInfoTable(zvalue node);
zvalue makeInterpolate(zvalue node);
zvalue makeMapExpression(zvalue mappings);
zvalue makeMaybeValue(zvalue expression);
zvalue makeRecordExpression(zvalue name, zvalue data);
zvalue makeSymbolTableExpression(zvalue mappings);
zvalue makeThunk(zvalue expression);
zvalue withModuleDefs(zvalue node);
zvalue withTop(zvalue node);
zvalue withoutTops(zvalue node);


//
// Parallel to `getters.sam`:
//

// These are all documented in spec.
zvalue get_baseName(zvalue source);
zvalue get_definedNames(zvalue node);


//
// Parallel to `literals.sam`:
//

/** Equivalent to `LITS::<name>` in the spec. */
#define LITS(name) (LIT_##name)

/** Equivalent to `METHODS::<class>_<name>` in the spec. */
#define METHODS(cls, methodName) \
    (cm_new_Record(SYM(methodId), \
        SYM(class), LITS(cls), \
        SYM(name),  SYMS(methodName)))

/** Equivalent to `SYMS::<name>` in the spec. */
#define SYMS(name) (makeLiteral(SYM(name)))

// These are all documented in spec.
zvalue extractLiteral(zvalue node);
zvalue makeLiteral(zvalue value);


//
// Parallel to `misc.sam`:
//

// These are all documented in spec.
bool canYieldVoid(zvalue node);
zvalue formalsMaxArgs(zvalue formals);
zvalue formalsMinArgs(zvalue formals);
bool isExpression(zvalue node);


//
// Parallel to `refs.sam`:
//

/** Equivalent to `REFS::<name>` in the spec. */
#define REFS(name) (makeVarFetch(SYM(name)))


//
// Parallel to `resolve.sam`:
//

// These are all documented in spec.
zvalue resolveImport(zvalue node, zvalue resolveFn);
zvalue withResolvedImports(zvalue node, zvalue resolveFn);

#endif
