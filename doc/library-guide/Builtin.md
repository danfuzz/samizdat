Samizdat Layer 0: Core Library
==============================

Builtin
-------

`Builtin` is the type for functions that are built into the lowest level
of the system.


<br><br>
### Generic Function Definitions: `Value` protocol

#### `debugName(type) <> . | void`

Returns the name of the builtin, if it has one. This is can be
an arbitrary value, which is typically (but not necessarily) a string.

#### `perEq(builtin, other) <> builtin | void`

Calls `totalEq`.

#### `perOrder(builtin, other) <> int`

Calls `totalOrder`.

#### `totalEq(builtin1, builtin2) <> builtin | void`

Performs an identity comparison. Two builtins are only equal if they
refer to the exact same functionality.

#### `totalOrder(builtin1, builtin2) <> int`

Performs an identity comparison. Builtins have a consistent, transitive, and
symmetric &mdash; but arbitrary &mdash; total order.


<br><br>
### Generic Function Definitions: `Function` protocol

#### `call(builtin, args*) <> . | void`

Calls the given builtin with the given arguments.


#### `canCall(builtin, value) <> logic`

Checks if the given builtin accepts at least one argument. Returns the
`value` argument if so.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
