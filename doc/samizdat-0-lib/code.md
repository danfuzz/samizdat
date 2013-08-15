Samizdat Layer 0: Core Library
==============================

Functions And Code
------------------

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
