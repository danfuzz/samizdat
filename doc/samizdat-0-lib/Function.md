Samizdat Layer 0: Core Library
==============================

Functions
---------

There is no `Function` type per se. There is, however, effectively a
`Function` protocol, consisting of the two methods `call` and `canCall`.
Any value that binds those two methods is a `Function`.


<br><br>
### Generic Function Definitions

#### `call(function, args*) <> . | void`

Calls the given `function` with the given arguments. This function isn't
normally that useful, in that `call(x, y)` is the same as saying `x(y)`.
However, this function is in fact a generic function, and it is possible
to use various other functions to reflect on it.

#### `canCall(function, value) <> logic`

Returns `value` if it can be passed as the first argument to the given
`function`, or void if not. In the case of a regular function, this
merely checks if the function accepts at least one argument. In the case
of a generic function, this checks to see if value's type is bound.


<br><br>
### Primitive Definitions

#### `nonlocalExit(exitFunction, optValue?) <> n/a # Returns elsewhere.`

Helper for calling nonlocal exit functions. This calls the given
`exitFunction`, passing it `optValue*` (that is, the given value as a
single argument if present, or with no arguments).

This function expects that `exitFunction` will not return to it. If it
does, then that is considered an error (terminating the runtime). The point
of this is so that code can rely on calls to *this* function to never
return, even in the face of misbehaving exit functions.

**Syntax Note:** Used in the translation of `break` and other nonlocal
exit forms.


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
