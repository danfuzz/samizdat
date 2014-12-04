Samizdat Layer 0: Core Library
==============================

Builtin
-------

`Builtin` is the type for functions that are built into the lowest level
of the system.


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> builtin | void`

Performs an identity comparison. Two builtins are only equal if they
refer to the exact same functionality.

#### `.crossOrder(other) -> symbol | void`

Performs an identity comparison. Builtins do not have a defined order.

#### `.debugSymbol() -> symbol | void`

Returns the symbolic name of the builtin, if it has one.

#### `.perEq(other) -> builtin | void`

Default implementation.

#### `.perOrder(other) -> symbol | void`

Default implementation.


<br><br>
### Method Definitions: `Function` protocol

#### `.call(args*) -> . | void`

Calls the given builtin with the given arguments.
