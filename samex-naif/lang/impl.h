// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Private implementation details
//

#ifndef _IMPL_H_
#define _IMPL_H_

#include <stdbool.h>

#include "lang.h"
#include "type/Class.h"
#include "zlimits.h"


/** Simple enumeration for all the evaluable node types. */
typedef enum {
    EVAL_apply = 1,  // 1, so that it won't be a "sneaky default."
    EVAL_call,
    EVAL_closure,
    EVAL_fetch,
    EVAL_importModule,
    EVAL_importModuleSelection,
    EVAL_importResource,
    EVAL_literal,
    EVAL_maybe,
    EVAL_noYield,
    EVAL_store,
    EVAL_varRef,
    EVAL_varDef,
    EVAL_varDefMutable,
    EVAL_void
} zevalType;

/** Mapping from `Type` index to corresponding `zevalType`. */
extern zevalType langTypeMap[DAT_MAX_CLASSES];

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

/**
 * Executes a `closure` form.
 */
zvalue execClosure(Frame *frame, zvalue closureNode);

/**
 * Executes an `expression` form, with `@maybe` and `@void` nodes allowed.
 * Returns the evaluated result. Only returns void (represented as `NULL`)
 * if given a `@maybe` node which evaluated to void, or a `@void` node.
 */
zvalue execExpressionOrMaybe(Frame *frame, zvalue e);

/**
 * Executes a list of `statement` forms.
 */
void execStatements(Frame *frame, zvalue statements);

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
 * Gets the evaluation type (enumerated value) of the given node.
 */
inline zevalType get_evalType(zvalue node) {
    return langTypeMap[get_classIndex(node)];
}

#endif
