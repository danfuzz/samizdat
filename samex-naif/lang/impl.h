/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Private implementation details
 */

#ifndef _IMPL_H_
#define _IMPL_H_

#include "lang.h"
#include "type/Type.h"
#include "zlimits.h"

#include <stdbool.h>

/** Simple enumeration for all the evaluable node types. */
typedef enum {
    EVAL_call = 1, // 1, so that it won't be a "sneaky default."
    EVAL_closure,
    EVAL_expression,
    EVAL_interpolate,
    EVAL_literal,
    EVAL_varBind,
    EVAL_varDeclare,
    EVAL_varDef,
    EVAL_varRef,
    EVAL_voidable
} zevalType;

/** Mapping from `Type` index to corresponding `zevalType`. */
extern zevalType langTypeMap[DAT_MAX_TYPES];

/** Function called into by `nleCall`. */
typedef zvalue (*znleFunction)(void *state, zvalue exitFunction);

/**
 * Active execution frame. These are passed around during evaluation
 * as code executes, and can become referenced by closures that are
 * released "in the wild".
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
extern zvalue TYPE_Closure;

/** Type for nonlocal exit functions. */
extern zvalue TYPE_NonlocalExit;

/**
 * Gets the evaluation type (enumerated value) of the given node.
 */
inline zevalType evalTypeOf(zvalue node) {
    return langTypeMap[typeIndexOf(node)];
}

/**
 * Executes a `closure` form.
 */
zvalue execClosure(Frame *frame, zvalue closureNode);

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
zvalue execExpressionVoidOk(Frame *frame, zvalue expression);

/**
 * Executes a `varDeclare` node, by updating the given execution frame
 * as appropriate.
 */
void execVarDeclare(Frame *frame, zvalue varDeclare);

/**
 * Executes a `varDef` node, by updating the given execution frame
 * as appropriate.
 */
void execVarDef(Frame *frame, zvalue varDef);

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
 * Adds a new variable to the given frame.
 */
void frameAdd(Frame *frame, zvalue name, zvalue value);

/**
 * Binds the given variable in the given frame.
 */
void frameBind(Frame *frame, zvalue name, zvalue value);

/**
 * Declares a bind-once variable in the given frame.
 */
void frameDeclare(Frame *frame, zvalue name);

/**
 * Gets a variable's value out of the given frame.
 */
zvalue frameGet(Frame *frame, zvalue name);

/**
 * Snapshots the given frame into the given target. The `target` is assumed
 * to be part of a heap-allocated structure.
 */
void frameSnap(Frame *target, Frame *source);

/**
 * Sets up a nonlocal exit, and calls the given function with the
 * given state and a nonlocal exit function.
 */
zvalue nleCall(znleFunction function, void *state);

#endif
