Samizdat Layer 0: Core Library
==============================

Generics
--------

<br><br>
### Generic Function Definitions

#### `call(callable, args*) <> . | void`

Calls the given generic function with the given arguments. `args` must
have at least one element, and the first element is what is used to
figure out which bound function to dispatch to.

#### `canCall(callable, value) <> logic`

Returns `value` if it can be passed as the first argument to the given
generic function, that is, whether `value`'s type is bound to the
generic. Returns the `value` argument if so.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
