Samizdat Layer 0: Core Library
==============================

Builtin
-------

`Builtin` is the type for functions that are built into the lowest level
of the system.


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> logic`

Performs an identity comparison. Two builtins are only equal if they
refer to the exact same functionality.

#### `.crossOrder(other) -> isa Symbol | void`

Performs an identity comparison. Builtins do not have a defined order.

#### `.debugSymbol() -> isa Symbol | void`

Returns the symbolic name of the builtin, if it has one.

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol | void`

Default implementation.


<br><br>
### Method Definitions: `Function` protocol

#### `.call(args*) -> . | void`

Calls the given builtin with the given arguments.
