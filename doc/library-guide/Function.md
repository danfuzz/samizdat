Samizdat Layer 0: Core Library
==============================

Function Protocol
-----------------

There is no `Function` type per se. There is, however, effectively a
`Function` protocol, consisting of the single method `call`. Any value
that binds that method is a `Function`.


<br><br>
### Method Definitions

#### `.call(args*) -> . | void`

Calls the given function with the given arguments. This function isn't
normally that useful, in that `x.call(y)` is the same as saying `x(y)` when
`x` is a function of some sort. However, this is in fact a method, and it is
possible to use various other facilities to reflect on it.
