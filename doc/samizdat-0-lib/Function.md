Samizdat Layer 0: Core Library
==============================

Functions
---------

<br><br>
### Generic Function Definitions: `Value` protocol

#### `totEq(function1, function2) <> function | void`

Performs an identity comparison. Two functions are only equal if they are
truly the same box.

#### `totOrder(function1, function2) <> int`

Performs an identity comparison. Functions have a consistent, transitive, and
symmetric &mdash; but arbitrary &mdash; total order.


<br><br>
### Generic Function Definitions: `Callable` protocol

#### `call(function, args*) <> . | void`

Calls the given function with the given arguments.


#### `canCall(function, value) <> logic`

Checks if the given function accepts at least one argument. Returns the
`value` argument if so.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
