Samizdat Layer 0: Core Library
==============================

Symbol
------

A `Symbol` is an identifier used when referring to methods and other
language constructs.


<br><br>
### Method Definitions: `Symbol` protocol

#### `.isInterned() -> symbol | void`

Returns `this` if it is interned (that is, *not* unlisted). Returns void
otherwise.

#### `.toString() -> string`

Returns the name of the symbol as a string.

#### `.toUnlisted() -> symbol`

Returns a new unlisted symbol whose name is the same as `this`'s. This
*always* returns a fresh symbol. (That is, if given an unlisted symbol,
this method does *not* just return `this`.)

<br><br>
### Method Definitions: `Value` protocol

#### `.debugString() -> string`

Returns a string representation of the symbol. This includes a suggestive
prefix before the name of `@` for interned symbols or `@+` for unlisted
symbols.

#### `.debugSymbol() -> symbol`

Returns `this`.

#### `.perEq(other) -> symbol | void`

Default implementation.

#### `.perOrder(other) -> int | void`

Default implementation.

#### `.totalEq(other) -> builtin | void`

Returns `this` if `other` is a reference to the same symbol, or void if
not.

#### `.totalOrder(other) -> int | void`

Orders symbols by internedness (primary) and name (secondary), with
interned symbols getting ordered *before* unlisted symbols. Two
different unlisted symbols with the same name are considered unordered
(but not equal).

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
