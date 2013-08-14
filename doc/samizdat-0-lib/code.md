Samizdat Layer 0: Core Library
==============================

Functions And Code
------------------

<br><br>
### Primitive Definitions

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
