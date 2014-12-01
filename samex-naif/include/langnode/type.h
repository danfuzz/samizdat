// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Type definitions for `langnode.`
//

#ifndef _LANGNODE_TYPE_H_
#define _LANGNODE_TYPE_H_

#include <stdbool.h>

#include "type/Record.h"
#include "type/Symbol.h"


/** Simple enumeration for all the evaluable node classes and symbols. */
typedef enum {
    EVAL_apply = 1,  // 1, so that it won't be a "sneaky default."
    EVAL_call,
    EVAL_cell,
    EVAL_closure,
    EVAL_directive,
    EVAL_export,
    EVAL_exportSelection,
    EVAL_external,
    EVAL_fetch,
    EVAL_importModule,
    EVAL_importModuleSelection,
    EVAL_importResource,
    EVAL_internal,
    EVAL_lazy,
    EVAL_literal,
    EVAL_mapping,
    EVAL_maybe,
    EVAL_module,
    EVAL_noYield,
    EVAL_nonlocalExit,
    EVAL_promise,
    EVAL_result,
    EVAL_store,
    EVAL_varRef,
    EVAL_varDef,
    EVAL_void,
    EVAL_yield,
    EVAL_CH_PLUS,   // For formal argument repetition.
    EVAL_CH_QMARK,  // For formal argument repetition.
    EVAL_CH_STAR    // For formal argument repetition.
} zevalType;

/** Mapping from `Symbol` index to corresponding `zevalType`. */
extern zevalType langSymbolMap[DAT_MAX_SYMBOLS];

/**
 * Gets the evaluation type (enumerated value) of the given record.
 */
inline zevalType recordEvalType(zvalue record) {
    return langSymbolMap[recNameIndex(record)];
}

/**
 * Returns whether the evaluation type of the given record is as given.
 */
inline bool recordEvalTypeIs(zvalue record, zevalType type) {
    return recordEvalType(record) == type;
}

/**
 * Gets the evaluation type (enumerated value) of the given symbol.
 */
inline zevalType symbolEvalType(zvalue symbol) {
    return langSymbolMap[symbolIndex(symbol)];
}

#endif
