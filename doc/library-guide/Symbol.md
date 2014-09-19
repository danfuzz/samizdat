Samizdat Layer 0: Core Library
==============================

Symbol
------

A `Symbol` is an identifier used when referring to methods and other
language constructs.


<br><br>
### Method Definitions: `Symbol` protocol

#### `isInterned() -> symbol | void`

Returns `this` if it is interned (that is, non-anonymous). Returns void
otherwise.

#### `.makeAnonymous() -> symbol`

Returns a new anonymous symbol whose name is the same as `this`'s. This
*always* returns a fresh symbol. (That is, if given an anonymous symbol,
this method does *not* just return `this`.)

#### `.toString() -> string`

Returns the name of the symbol as a string.

<br><br>
### Method Definitions: `Value` protocol

#### `.debugString() -> string`

Returns a string representation of the symbol. This includes a suggestive
prefix before the name of `@` for interned symbols or `@+` for anonymous
symbols.

#### `.debugSymbol() -> symbol`

Returns `this`.

#### `.perEq(other) -> symbol | void`

Default implementation.

#### `.perOrder(other) -> int | void`

Default implementation.

#### `.totalEq(other) -> builtin | void`

Default implementation.

#### `.totalOrder(other) -> int | void`

Orders symbols by internedness (primary) and name (secondary), with
interned symbols getting ordered *before* anonymous symbols. Two
different anonymous symbols with the same name are considered unordered
(not equal).

<br><br>
### Method Definitions: `Function` protocol

#### `.call(args+) -> . | void`

Calls the given symbol with the given arguments. This performs method
dispatch on `args[0]`.


<br><br>
### Primitive Definitions

(none)

<br><br>
### In-Language Definitions

(none)
