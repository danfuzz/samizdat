Samizdat Layer 0: Core Library
==============================

Functions And Code
------------------

### Boxes

A few of the functions in this module are concerned with "boxes".
A box is a holder for some other value. Boxes are sometimes also known
as "cells".

In addition to the box constructor functions, the three functions that
deal with boxes are `boxGet` to get the contents of a box (or void if
the box value has yet to be set), `boxSet` to set the contents of
a box, and `boxIsSet` to indicate whether `boxSet` has been called
(which is not the same as `boxGet` returning non-void).

As a special case, it is valid to pass the library constant `nullBox`
as the `box` argument to box-related functions, in which case the function
acts as if the box is permanently un-set. Notably, `boxSet(nullBox, value)`
is effectively a no-op. This arrangement is done in order to make it easy
to pass around a "box" whose value is to be ignored.


<br><br>
### Primitive Definitions

#### `boxGet(box) <> . | !.`

Gets the value inside a box, if any. If the box either is unset or has
been set to void, this returns void. `box` must be a box as returned by
either `mutableBox` or `yieldBox`.

#### `boxIsSet(box) <> logic`

Returns `box` if the `box` was either initialized with a value or has
had `boxSet` called on it (whether with a value or void). Otherwise returns
void. `box` must be a box as returned by either `mutableBox` or `yieldBox`.

#### `boxSet(box, value?) <> .`

Sets the value of a box to the given value, or to void if `value` is
not supplied. This function always returns `value` (or void if `value` is
not supplied). `box` must be a box as returned by either `mutableBox` or
`yieldBox`.

It is an error (terminating the runtime) for `box` to be a yield box on
which `boxSet` has already been called.

#### `mutableBox(value?) <> box`

Creates a mutable box, with optional pre-set value. The result of a call to
this function is a box which can be set any number of times using
`boxSet`. The contents of the box are accessible by calling `boxGet`.

The initial box content value is the `value` given to this function. This
is what is returned from `boxGet` until `boxSet` is called to replace it.
If `value` is not supplied, `boxGet` returns void until `boxSet` is called.

This function is meant to be the primary way to define (what amount to)
mutable variables, in that *Samizdat Layer 0* only provides immutably-bound
variables. It is hoped that this facility will be used as minimally as
possible, so as to not preclude the system from performing functional-style
optimizations.

#### `nonlocalExit(yieldFunction, thunk?) <> !. # Returns elsewhere.`

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

#### `object(implementation, state) <> function`

Constructs and returns an "object". In *Samizdat Layer 0* an object is
merely the combination of a mutable `state` value (an arbitarary
value) with an `implementation` function (an arbitrary function).
The return value from the call to `object` is another function,
referred to as the "object interface function".

When the object interface function is called, it in turn calls the
`implementation` function, passing it the same arguments as it was
called with, along with two additional arguments at the head of the
argument list:

* The first argument is a `yield` function. This function must be
  called exactly once during the course of a call to `implementation`.
  This is used to indicate the return value from the object interface
  function. If `yield` is called with no argument, then the object
  interface function returns void. If `yield` is called with one argument,
  then the object interface function returns that value.

  When the `yield` function is called, it does *not* cause the
  implementation function to return; it merely causes the eventual
  result value (or void) to be cached until the function does finally
  return.

* The second argument is a `state` value. This is the latest state
  for the object, as defined immediately below.

The implementation function can return either a value or void. If
it returns a value, then the value becomes the new `state` for
the object, replacing either the `state` originally passed to
`object`, or whatever other state had been returned in the mean time.

To avoid confusion as well as hew closely to the actor model,
it is invalid (terminating the runtime) for an object implementation
function to call its own interface function, either directly or
indirectly. To implement a recursive operation, it is necessary to do
so without going through the interface.

#### `yieldBox() <> box`

Creates a set-once "yield box". The result of a call to this function is a
box which can be set at most once, using `boxSet`. Subsequent attempts to
set the box value will fail (terminating the runtime). The contents of the
box are accessible by calling `boxGet`. `boxGet` returns void until
`boxSet` is called.

This function is meant to be the primary way to capture the yielded values
from functions (such as object service functions and parser functions) which
expect to yield values by calling a function.

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
