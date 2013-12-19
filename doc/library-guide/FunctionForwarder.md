Samizdat Layer 0: Core Library
==============================

core::FunctionForwarder
--------------------

This module provides a class that knows how to forward functions.


<br><br>
### Function Definitions

#### `make() <> functionBox`

Simple function forward declaration utility. The result of a call to this
function is another function, called the "forwarding function" or
more succinctly the "forwarder".

In addition to being a callable function, a forwarder is also a yield box.
To set up a forward, first create the forwarder (with a call to this
function), then &mdash; arbitrarily later in the source file in
question &mdash; define an implementation for the function,
and call `store(forwarder, implementation)`. Once that is done, it is
safe to call the forwarder, and it will in turn call the implementation
with the same arguments and returns whatever the implementation returns.

This function is meant to make it a little easier to deal with the fact
that Samizdat Layer 0 prohibits use-before-def.
