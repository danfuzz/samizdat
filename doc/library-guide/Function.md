Samizdat Layer 0: Core Library
==============================

Function
--------

There is no `Function` type per se. There is, however, effectively a
`Function` protocol, consisting of the single method `call`. Any value
that binds that method is a `Function`.


<br><br>
### Generic Function Definitions

#### `call(function, args*) -> . | void`

Calls the given `function` with the given arguments. This function isn't
normally that useful, in that `call(x, y)` is the same as saying `x(y)`.
However, this function is in fact a generic function, and it is possible
to use various other functions to reflect on it.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
