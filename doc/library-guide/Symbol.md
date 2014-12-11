Samizdat Layer 0: Core Library
==============================

Symbol
------

A `Symbol` is an identifier used when referring to methods and other
language constructs.


<br><br>
### Method Definitions: `Symbol` protocol

#### `.cat(more*) -> isa Symbol`

Returns an interned symbol whose name consists of `this`'s name
concatenated with the names of all the arguments, in argument order.
Arguments must all be symbols.

#### `.isInterned() -> isa Symbol | void`

Returns `this` if it is interned (that is, *not* unlisted). Returns void
otherwise.

#### `.toUnlisted() -> isa Symbol`

Returns a new unlisted symbol whose name is the same as `this`'s. This
*always* returns a fresh symbol. (That is, if given an unlisted symbol,
this method does *not* just return `this`.)


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> logic`

Returns `this` if `other` is a reference to the same symbol, or void if
not.

#### `.crossOrder(other) -> isa Symbol | void`

Orders symbols by internedness (primary) and name (secondary), with
interned symbols getting ordered *before* unlisted symbols. Two
different unlisted symbols with the same name are considered unordered
(but not equal).

#### `.debugString() -> isa String`

Returns a string representation of the symbol. This includes a suggestive
prefix before the name of `@` for interned symbols or `@+` for unlisted
symbols.

#### `.debugSymbol() -> isa Symbol`

Returns `this`.

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol | void`

Default implementation.
