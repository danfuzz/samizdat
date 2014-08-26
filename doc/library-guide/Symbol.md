Samizdat Layer 0: Core Library
==============================

Symbol
------

A `Symbol` is an identifier used when referring to methods and other
language constructs.


<br><br>
### Method Definitions: `Value` protocol

#### `.debugName() -> string`

Returns the name of the symbol, as a string.

#### `.debugString() -> string`

Returns a string representation of the symbol. This includes a suggestive
prefix `.`. It also includes `anon-` before the name if this is an anonymous
symbol.

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

#### `makeAnonymousSymbol(name) -> symbol`

Creates and returns a new anonymous symbol. Even though anonymous,
it must have a name (which must be a string). The anonymity comes from
the fact that the symbol returned from this function will never be
found by doing a name lookup, as with `makeInternedSymbol`.

#### `makeInternedSymbol(name) -> symbol`

Finds and returns the interned (non-anonymous) symbol with the given `name`
(which must be a string). If the so-named symbol has not been created yet,
this function creates it.

#### `symbolIsInterned(symbol) -> symbol | void`

Returns `symbol` if it is interned (that is, non-anonymous). Returns void
otherwise.

#### `symbolName(symbol) -> string`

Returns the name of `symbol`.

<br><br>
### In-Language Definitions

(none)
