Samizdat Layer 0: Core Library
==============================

Conditionals And Iteration
--------------------------

<br><br>
### Primitive Definitions

#### `ifIs(predicate, isFunction, notFunction?) <> . | !.`

Primitive logic conditional. This calls the given predicate with no
arguments, taking note of its return value or lack thereof.

If the predicate returns a value, then the `isFunction` is called
with no arguments. If the predicate returns void, then the
`notFunction` (if any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

This function is identical to `ifValue`, except that in the value case,
this function calls the consequent function with no arguments, whereas
`ifIs` calls it with an argument.

#### `ifValue(function, valueFunction, voidFunction?) <> . | !.`

Primitive logic conditional. This calls the given function with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunction` is called
with one argument, namely the value returned from the original
function. If the function returns void, then the `voidFunction` (if
any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

This function is identical to `ifIs`, except that in the value case,
this function calls the consequent function with an argument, whereas
`ifIs` calls it with no arguments.

#### `loop(function) <> !.`

Primitive unconditional loop construct. This repeatedly calls the given
function with no arguments.

In order for the loop to terminate, the function must use a nonlocal exit.

#### `loopReduce(base, function) <> !.`

Primitive unconditional loop construct, with reduce semantics. This repeatedly
calls the given function with a single argument. The argument is the
most recent non-void result of calling the function, or the `base` value
if the function has yet to return non-void (including notably to the first
call to the function).

In order for the loop to terminate, the function must use a nonlocal exit.


<br><br>
### In-Language Definitions

#### `and(predicates*) <> logic`

Short-circuit logic conjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns void, in which case this function also returns
void. If no predicate returns void, this function returns whatever
value was returned by the last predicate.

If no predicates were supplied, this returns `true`.

#### `booleanAnd(predicates*) <> boolean`

Short-circuit boolean conjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns `false`, in which case this function also returns
`false`. If no predicate returns `false`, this function returns `true`.

If no predicates were supplied, this returns `true`. It is an error
(terminating the runtime) if any predicate returns anything other than
a boolean value.

#### `booleanOr(predicates*) <> boolean`

Short-circuit boolean disjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns `true`, in which case this function also returns
`true`. If no predicate returns `true`, this function returns `false`.

If no predicates were supplied, this returns `false`. It is an error
(terminating the runtime) if any predicate returns anything other than
a boolean value.

#### `ifNot(predicate, notFunction, isFunction?) <> . | !.`

This is identical to `ifIs` except that the order of the second
and third arguments is reversed.

#### `ifVoid(function, voidFunction, valueFunction?) <> . | !.`

This is identical to `ifValue` except that the order of the second
and third arguments is reversed.

#### `or(predicates*) <> logic`

Short-circuit logic disjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns a value (not void), in which case this function also returns
that value. If no predicate returns a value (including if no
predicates were supplied), this function returns void.
