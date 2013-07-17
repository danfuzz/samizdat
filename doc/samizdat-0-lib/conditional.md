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

#### `ifNot(predicate, notFunction) <> . | !.`

This is identical to `ifIs`, except that the `isFunction` argument is omitted.

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

#### `ifValueOr(function, voidFunction) <> . | !.`

This is identical to `ifValue`, except that the `valueFunction` is
omitted and taken to be the identity function, and the `voidFunction`
is required (not an optional argument). That is, `ifValueOr(x, y)` is the
same as `ifValue(x, { value :: <> value }, y)`.

The reason `voidFunction` is required is because it is pointless to omit it,
in that `ifValueOr(x)` would mean the same thing as `x()`, and
`ifValueOr({ <> x })` would mean the same thing as just `x`.

This function is meant as the primitive that higher-layer logical-or
expressions bottom out into, hence the name.

#### `loop(function) <> !.`

Primitive unconditional loop construct. This repeatedly calls the given
function with no arguments.

In order for the loop to terminate, the function must use a nonlocal exit.

#### `loopReduce(function, args*) <> !.`

Primitive unconditional loop construct, with reduce semantics. This repeatedly
calls the given function with a list of arguments. The arguments are the
most recent non-void result of calling the function (which must be a list), or
the original `args` list if the function has yet to return non-void (including
notably to the first call to the function).

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
