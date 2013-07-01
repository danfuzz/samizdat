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

### Generators

In *Samizdat*, generators are the primary means of "spreading" a computation
across elements of a collection. Most basically, a generator is just
a function (possibly, but not necessarily a pure function) with a particular
contract.

The contract is as follows:

* A generator as a function always accepts exactly one argument. That
  argument is (or is the equivalent of) a "yield box".

* When a generator is not "voided" (out of elements), calling it causes
  two things to be done:

  * It calls `boxSet(box, value)` on its argument in order to yield
    one value out of itself.
  * It returns a new generator as a result which, when called, yields
    the *next* value, and so on.

* When a generator has yielded its final element, it returns a voided
  generator.

* When a voided generator is called, it does these two things:

  * It calls `boxSet(box)` (with no payload argument) on its argument
    in order to yield void.
  * It returns void.

TODO: Generators are not yet used anywhere in the system.

<br><br>
### Primitive Definitions

#### `boxGet(box, ifNotSet?) <> . | !.`

Gets the value inside a box. If the box does not (yet) have a value, this
returns the `ifNotSet` value if suppplied or void if not. `box` must be
a box as returned by either `mutableBox` or `yieldBox`.

#### `boxIsSet(box) <> boolean`

Returns `true` if the `box` was either initialized with a value or has
had `boxSet` called on it (whether with a value or void). `box` must be a
box as returned by either `mutableBox` or `yieldBox`.

#### `boxSet(box, value?) <> .`

Sets the value of a box to the given value, or to void if `value` is
not supplied. This function always returns `value` (or void if `value` is
not supplied). `box` must be a box as returned by either `mutableBox` or
`yieldBox`.

It is an error (terminating the runtime) for `box` to be a yield box on
which `boxSet` has already been called.

#### `mutableBox(value?) <> function`

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

#### `yieldBox() <> function`

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

#### `partialApply(function, value*) <> function`

Partial function application. This takes a function and a number of arguments
to call it with, returning a new function. The new function, when called,
in turn calls the original function with the arguments specified here as
the first arguments, followed by any arguments passed to the new function.

For example, `partialApply(iadd, 2)` is a function which when called returns
the sum of 2 and whatever value it was passed.

#### `yCombinator(function) <> function`

The Y combinator, in a form suitable for use in Samizdat Layer 0 when
defining self-recursive functions.

**Summary**

If you want to make a self-recursive function in *Samizdat Layer 0*,
you can write it like this:

```
myRecursiveFunction = yCombinator { selfCall ::
    # Inner function.
    <> { myArg1 myArg2 ... ::
        ... my code ...
        selfCall args ...
        ... my code ...
    }
};
```

In the example above, a call to `selfCall` will end up recursively
calling into the (so-labeled) inner function.

**Detailed Explanation**

This function takes another function as its argument, called the
"wrapper" function. That function must take a single argument, which itself
is a function, called the "recurser" function. The "wrapper" function must
return yet another function, called the "inner" function. The return value
of this function, called the "result" function, is a function which, when
called, ends up calling the wrapper function and then calling the inner
function that the wrapper function returned.

The inner function is an arbitrary function, taking arbitrary arguments,
returning anything including void, and performing any arbitrary
actions in its body. In particular, it can be written to call the
"recurser" argument passed to its wrapper. If it does so, that will in
turn result in a recursive call to itself.

This function is used to write recursive functions without relying
on use-before-def variable binding.

See Wikipedia [Fixed-point
combinator](http://en.wikipedia.org/wiki/Fixed-point_combinator) for
more details about some of the theoretical basis for this stuff.
This function is in the "Y combinator family" but is not exactly any
of the ones described on that page. It is most similar to the Z
combinator, which is also known as the "call-by-value Y combinator"
and the "applicative-order Y combinator", but this function is not
*exactly* the Z combinator. In particular, this version is written
such that the wrapper function always gets called directly with the
result of a U combinator call. It is unclear whether this results
in any meaningful difference in behavior. See also [this question on
StackOverflow](http://stackoverflow.com/questions/16258308).

In traditional notation (and with
the caveat that `a` represents an arbitrary number of arguments here),
this function would be written as:

```
U = λx.x x
Y_sam = λf . U (λs . (λa . (f (U s)) a))
```

#### `yStarCombinator(functions*) <> [functions*]`

The Y* combinator, in a form suitable for use in Samizdat Layer 0 when
defining sets of mutually-recursive functions.

This is like `yCombinator`, except that it can take any number of
functions as arguments, resulting in a list of mutual-recursion-enabled
result functions.

If you want to make a set of N mututally-recursive functions in
*Samizdat Layer 0*, you can write it like this:

```
myRecursiveFunctions = yStarCombinator
    { selfCall1, selfCall2, ... ::
        # Inner function.
        <> { myArg1, myArg2, ... ::
            ... my code ...
            selfCall1(args, ...) # Call this function self-recursively.
            selfCall2(args, ...) # Call the other function.
            ... my code ...
        }
    }
    { selfCall1, selfCall2, ... ::
        # Inner function.
        <> { myArg1, myArg2, ... ::
            ... my code ...
            selfCall1(args, ...) # Call the other function.
            selfCall2(args, ...) # Call this function self-recursively.
            ... my code ...
        }
    };
```

This results in an array of functions corresponding to the original argument
functions. Each of those functions can recurse by calling any of the other
functions, via the arguments passed into the wrapper functions (arguments
prefixed `selfCall` in the example here).

See `yCombinator` for more detailed discussion and explanation.
