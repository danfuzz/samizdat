Samizdat Layer 0: Core Library
==============================

Conditionals And Iteration
--------------------------

<br><br>
### Primitive Definitions


#### `argsMap function rest* <> listlet`

Primitive mapping iterator. This calls the given function on each of
the rest of the arguments in sequence, collecting all the non-void
results into a listlet, which is returned. The function is called with
exactly one argument, namely the item to process.

**Note:** Unlike most of the map/reduce functions, this one takes its
function as the first argument. This is done specifically so that it is
convenient to `apply` it.

#### `argsReduce function base rest* <> . | ~.`

Primitive reducing iterator. This calls the given function on each of the
rest of the arguments in sequence, yielding a final reduction result.
(That is, this is a left-reduce operation.) The function is called with
exactly two arguments, first the previous non-void reduction result (or
the `base` for the first item in `rest`), and second the item to process.
The return value of this call is what would have been passed as the
reduction result to a would-be next function call.

**Note:** Unlike most of the map/reduce functions, this one takes its
function as the first argument. This is done specifically so that it is
convenient to `apply` it.

#### `ifTrue predicate trueFunction falseFunction? <> . | ~.`

Primitive boolean conditional. This calls the given predicate with no
arguments, expecting it to return a boolean.

If the predicate returns `true`, then the `trueFunction` is called
with no arguments. If the predicate returns `false`, then the
`falseFunction` (if any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

#### `ifValue function valueFunction voidFunction? <> . | ~.`

Primitive value conditional. This calls the given function with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunction` is called
with one argument, namely the value returned from the original
function. If the function returns void, then the `voidFunction` (if
any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

#### `loop function <> ~.`

Primitive unconditional loop construct. This repeatedly calls the given
function with no arguments.

In order for the loop to terminate, the function must use a nonlocal exit.

#### `loopReduce base function <> ~.`

Primitive unconditional loop construct, with reduce semantics. This repeatedly
calls the given function with a single argument. The argument is the
most recent non-void result of calling the function, or the `base` value
if the function has yet to return non-void (including notably to the first
call to the function).

In order for the loop to terminate, the function must use a nonlocal exit.


<br><br>
### In-Language Definitions

#### `and predicates* <> boolean`

Short-circuit conjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns `false`, in which case this function also returns
`false`. If no predicate returns `false`, this function returns
`true`.

#### `ifFalse predicate falseFunction trueFunction? <> . | ~.`

This is identical to `ifTrue` except that the order of the second
and third arguments is reversed.

#### `ifVoid function voidFunction valueFunction? <> . | ~.`

This is identical to `ifValue` except that the order of the second
and third arguments is reversed.

#### `or predicates* <> boolean`

Short-circuit disjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns `true`, in which case this function also returns
`true`. If no predicate returns `true`, this function returns `false`.
