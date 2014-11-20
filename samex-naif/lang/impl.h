// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

#include <stdbool.h>

#include "const.h"
#include "lang.h"
#include "type/Class.h"
#include "type/Record.h"
#include "type/Symbol.h"


/** Implementation limits. */
enum {
    /** Maximum number of formal arguments to a function. */
    LANG_MAX_FORMALS = 20,

    /**
     * Maximum number of tokens in a given parse (which is the maximum
     * number of characters for a tokenizer).
     */
    LANG_MAX_TOKENS = 100000,

    /**
     * Maximum number of characters in a tokenized string constant,
     * identifier, or directive.
     */
    LANG_MAX_STRING_CHARS = 200
};

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
 * Active execution frame. These are passed around during evaluation
 * as code executes, and can become referenced by closures that are
 * released "in the wild."
 */
typedef struct Frame {
    /** Parent closure value. May be `NULL`. */
    zvalue parentClosure;

    /** Parent frame. May be `NULL`. */
    struct Frame *parentFrame;

    /** Variables defined in this frame, as a map from names to values. */
    zvalue vars;

    /** Is this frame on the heap? Used for validation/asserts. */
    bool onHeap;
} Frame;

/** Type for closure functions. */
extern zvalue CLS_Closure;

/** Type for converted `closure` nodes. */
extern zvalue CLS_ClosureNode;

/** Type for executable nodes. */
extern zvalue CLS_ExecNode;

/**
 * Executes a translated `closure` node, which means that a closure is to be
 * constructed. This takes a `ClosureNode` (not an `ExecNode`) and returns a
 * `Closure` instance.
 */
zvalue exnoBuildClosure(zvalue node, Frame *frame);

/**
 * Calls a closure, using the given `node` to drive argument binding and
 * execution. This is where `Closure.call()` bottoms out to do most of its
 * work. `parentFrame` is the execution frame that was active at the time the
 * closure was constructed. `parentClosure` is the object that holds
 * `parentFrame`. `args` are the arguments being passed to this call.
 */
zvalue exnoCallClosure(zvalue node, Frame *parentFrame, zvalue parentClosure,
        zarray args);

/**
 * Converts an `expression` node or list (per se) of same. This converts
 * nodes into instances of `ExecNode`, and stores a reference to the
 * replacement via the given pointer. As a convenience, if `*orig` is `NULL`,]
 * this function does nothing.
 */
void exnoConvert(zvalue *orig);

/**
 * Executes a translated `expression` node, in particular an instance of
 * `ExecNode`. This allows for converted `maybe` and `void` nodes.
 */
zvalue exnoExecute(zvalue node, Frame *frame);

/**
 * Executes a list (per se) of translated `expression` nodes, treating them
 * as statements. (E.g., it allows variable definitions and doesn't care if
 * they yield void.)
 */
void exnoExecuteStatements(zvalue statements, Frame *frame);

/**
 * Initializes the given frame. The `frame` is assumed to live on the
 * C stack. The `parentFrame` if non-`NULL` must live on the heap.
 */
void frameInit(Frame *frame, Frame *parentFrame, zvalue parentClosure,
    zvalue vars);

/**
 * Does gc value marking.
 */
void frameMark(Frame *frame);

/**
 * Defines a new variable to the given frame, binding it to the given box.
 */
void frameDef(Frame *frame, zvalue name, zvalue box);

/**
 * Fetches the box associated with a variable, out of the given frame. Fails
 * with a terminal error if `name` is not found.
 */
zvalue frameGet(Frame *frame, zvalue name);

/**
 * Snapshots the given frame into the given target. The `target` is assumed
 * to be part of a heap-allocated structure.
 */
void frameSnap(Frame *target, Frame *source);

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
