// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
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
    NODE_apply = 1,  // 1, so that it won't be a "sneaky default."
    NODE_call,
    NODE_cell,
    NODE_closure,
    NODE_directive,
    NODE_export,
    NODE_exportSelection,
    NODE_external,
    NODE_fetch,
    NODE_importModule,
    NODE_importModuleSelection,
    NODE_importResource,
    NODE_internal,
    NODE_lazy,
    NODE_literal,
    NODE_mapping,
    NODE_maybe,
    NODE_module,
    NODE_noYield,
    NODE_nonlocalExit,
    NODE_promise,
    NODE_result,
    NODE_store,
    NODE_varRef,
    NODE_varDef,
    NODE_void,
    NODE_yield,
    NODE_CH_PLUS,   // For formal argument repetition.
    NODE_CH_QMARK,  // For formal argument repetition.
    NODE_CH_STAR    // For formal argument repetition.
} znodeType;

/** Mapping from `Symbol` index to corresponding `znodeType`. */
extern znodeType nodeSymbolMap[DAT_MAX_SYMBOLS];

/**
 * Gets the evaluation type (enumerated value) of the given record.
 */
inline znodeType nodeRecType(zvalue record) {
    return nodeSymbolMap[recNameIndex(record)];
}

/**
 * Returns whether the evaluation type of the given record is as given.
 */
inline bool nodeRecTypeIs(zvalue record, znodeType type) {
    return nodeRecType(record) == type;
}

/**
 * Gets the evaluation type (enumerated value) of the given symbol.
 */
inline znodeType nodeSymbolType(zvalue symbol) {
    return nodeSymbolMap[symbolIndex(symbol)];
}

#endif
