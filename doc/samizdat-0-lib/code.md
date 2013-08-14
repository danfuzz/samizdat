Samizdat Layer 0: Core Library
==============================

Functions And Code
------------------

<br><br>
### Primitive Definitions

#### `boxCanStore(box) <> logic`

Returns `box` if the `box` can be stored to. Otherwise returns void.
`box` must be a box as returned by either `makeMutableBox` or `makeYieldBox`.

#### `boxFetch(box) <> . | void`

Gets the value inside a box, if any. If the box either is unset or has
been set to void, this returns void. `box` must be a box as returned by
either `makeMutableBox` or `makeYieldBox`.

#### `boxStore(box, value?) <> .`

Sets the value of a box to the given value, or to void if `value` is
not supplied. This function always returns `value` (or void if `value` is
not supplied). `box` must be a box as returned by either `makeMutableBox` or
`makeYieldBox`.

It is an error (terminating the runtime) for `box` to be a yield box on
which `boxStore` has already been called.

#### `makeMutableBox(value?) <> box`

Creates a mutable box, with optional pre-set value. The result of a call to
this function is a box which can be set any number of times using
`boxStore`. The contents of the box are accessible by calling `boxFetch`.

The initial box content value is the `value` given to this function. This
is what is returned from `boxFetch` until `boxStore` is called to replace it.
If `value` is not supplied, `boxFetch` returns void until `boxStore` is called.

This function is meant to be the primary way to define (what amount to)
mutable variables, in that *Samizdat Layer 0* only provides immutably-bound
variables. It is hoped that this facility will be used as minimally as
possible, so as to not preclude the system from performing functional-style
optimizations.

#### `makeYieldBox() <> box`

Creates a set-once "yield box". The result of a call to this function is a
box which can be stored to at most once, using `boxStore`. Subsequent
attempts to store to the box will fail (terminating the runtime). The
contents of the box are accessible by calling `boxFetch`. `boxFetch` returns
void until and unless `boxStore` is called with a second argument.

This function is meant to be the primary way to capture the yielded values
from functions (such as object service functions and parser functions) which
expect to yield values by calling a function.

#### `nonlocalExit(yieldFunction, thunk?) <> n/a # Returns elsewhere.`

Helper for calling nonlocal exit functions. This takes a function of
zero-or-one argument &mdash; such as in particular the "yield" functions
defined when using nonlocal exit syntax &mdash; and an optional thunk
(function of no arguments).

If `thunk` is passed, then this function call calls `thunk()`. If the call
to `thunk` returns a value, then it then calls `yieldFunction` passing it
that value. If the call to `thunk` returns void, then it calls `yieldFunction`
with no arguments.

If `thunk` is not passed, then this function calls `yieldFunction` with no
arguments.

It is an error (terminating the runtime) if the call to `yieldFunction`
returns to this function.

<br><br>
### In-Language Definitions

#### `forwardFunction() <> function`

Simple function forward declaration utility. The result of a call to this
function is another function, called the "forwarding function".

The first time the forwarding function is called, it expects to be passed
another function, called the "target" function. It keeps this target function
as internal state, and it returns it as a result of the (first) call.

Every subsequent time the forwarding function is called, it passes the
arguments it is called with directly to the target function, and returns
whatever the target function returned (including void).

This function is meant to make it a little easier to deal with the fact
that *Samizdat Layer 0* prohibits use-before-def.
