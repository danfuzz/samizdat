Samizdat Layer 0: Core Library
==============================

Function
--------

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

(none)


<br><br>
### In-Language Definitions

(none)
