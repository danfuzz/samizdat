Samizdat Layer 0: Core Library
==============================

Generics
--------

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(generic1, generic2) <> generic | void`

Performs an identity comparison. Two generics are only equal if they are
truly the same box.

#### `perOrder(generic1, generic2) <> int`

Performs an identity comparison. Generics have a consistent, transitive, and
symmetric &mdash; but arbitrary &mdash; total order.


<br><br>
### Generic Function Definitions: `Callable` protocol

#### `call(generic, args*) <> . | void`

Calls the given generic function with the given arguments. `args` must
have at least one element, and the first element is what is used to
figure out which bound function to dispatch to.

#### `canCall(generic, value) <> logic`

Returns `value` if it can be passed as the first argument to the given
generic function, that is, whether `value`'s type is bound to the
generic. Returns the `value` argument if so.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
