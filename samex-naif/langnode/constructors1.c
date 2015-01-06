// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Bool.h"
#include "type/Class.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/Record.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "type/String.h"
#include "util.h"

#include "langnode.h"


//
// Exported functions
//

// Documented in spec.
zvalue makeApply(zvalue target, zvalue name, zvalue values) {
    if (values == NULL) {
        values = TOK_void;
    }

    return cm_new_Record(SYM(apply),
        SYM(name),   name,
        SYM(target), target,
        SYM(values), values);
}

// Documented in spec.
zvalue makeBasicClosure(zvalue table) {
    return cm_new(Record, SYM(closure),
        cm_cat(
            cm_new_SymbolTable(
                SYM(formals), EMPTY_LIST,
                SYM(statements), EMPTY_LIST),
            table));
}

// Documented in spec.
zvalue makeCall(zvalue target, zvalue name, zvalue values) {
    if (values == NULL) {
        values = EMPTY_LIST;
    }

    return cm_new_Record(SYM(call),
        SYM(name),   name,
        SYM(target), target,
        SYM(values), values);
}

// Documented in spec.
zvalue makeExport(zvalue node) {
    return cm_new_Record(SYM(export), SYM(value), node);
}

// Documented in spec.
zvalue makeExportSelection(zvalue names) {
    return cm_new_Record(SYM(exportSelection), SYM(select), names);
}

// Documented in spec.
zvalue makeFunCall(zvalue function, zvalue values) {
    if (recHasName(function, SYM(methodId))) {
        zvalue name = cm_get(function, SYM(name));
        zvalue cls = cm_get(function, SYM(class));

        if (cls != NULL) {
            return makeCall(cls, name, values);
        } else {
            zvalue first = cm_nth(values, 0);
            zvalue rest = METH_CALL(values, sliceInclusive, INT_1);
            return makeCall(first, name, rest);
        }
    } else {
        return makeCall(function, SYMS(call), values);
    }
}

// Documented in spec.
zvalue makeMaybe(zvalue value) {
    return cm_new_Record(SYM(maybe), SYM(value), value);
}

// Documented in spec.
zvalue makeNoYield(zvalue value) {
    return cm_new_Record(SYM(noYield), SYM(value), value);
}

// Documented in spec.
zvalue makeNonlocalExit(zvalue function, zvalue optValue) {
    zvalue value = (optValue == NULL) ? TOK_void : optValue;
    return cm_new_Record(SYM(nonlocalExit),
        SYM(function), function, SYM(value), value);
}

// Documented in spec.
zvalue makeVarDef(zvalue name, zvalue box, zvalue optValue) {
    zvalue value = (optValue == NULL) ? TOK_void : optValue;
    return cm_new_Record(SYM(varDef),
        SYM(name),  name,
        SYM(box),   box,
        SYM(value), value);
}

// Documented in spec.
zvalue makeVarFetch(zvalue name) {
    return cm_new_Record(SYM(fetch), SYM(target), makeVarRef(name));
}

// Documented in spec.
zvalue makeVarFetchGeneral(zvalue name) {
    // See discussion in `makeAssignmentIfPossible` above, for details about
    // `lvalue`.
    zvalue ref = makeVarRef(name);
    return cm_new_Record(SYM(fetch),
        SYM(box),    ref,
        SYM(lvalue), EMPTY_LIST,
        SYM(target), ref);
}

// Documented in spec.
zvalue makeVarRef(zvalue name) {
    return cm_new_Record(SYM(varRef), SYM(name), name);
}

// Documented in spec.
zvalue makeVarStore(zvalue name, zvalue value) {
    return cm_new_Record(SYM(store),
        SYM(target), makeVarRef(name), SYM(value), value);
}

// Documented in spec.
zvalue withFormals(zvalue node, zvalue formals) {
    return cm_cat(node, cm_new_SymbolTable(SYM(formals), formals));
}

// Documented in spec.
zvalue withName(zvalue node, zvalue name) {
    return cm_cat(node, cm_new_SymbolTable(SYM(name), name));
}

// Documented in spec.
zvalue withYieldDef(zvalue node, zvalue name) {
    zvalue yieldDef = cm_get(node, SYM(yieldDef));
    zvalue newBindings;

    if (yieldDef != NULL) {
        zvalue defStat = makeVarDef(name, SYM(result), makeVarFetch(yieldDef));
        newBindings = cm_new_SymbolTable(
            SYM(statements),
            listPrepend(defStat, cm_get(node, SYM(statements))));
    } else {
        newBindings = cm_new_SymbolTable(SYM(yieldDef), name);
    }

    return cm_cat(node, newBindings);
};

// Documented in spec.
zvalue withoutIntermediates(zvalue node) {
    return METH_CALL(node, del, SYM(box), SYM(interpolate), SYM(lvalue));
}
