Samizdat Layer 0: Core Library
==============================

Symbol
------

A `Symbol` is an identifier used when referring to methods and other
language constructs.


<br><br>
### Method Definitions: `Symbol` protocol

#### `.makeAnonymous() -> symbol`

Returns a new anonymous symbol whose name is the same as `this`'s. This
*always* returns a fresh symbol. (That is, if given an anonymous symbol,
this method does *not* just return `this`.)


<br><br>
### Method Definitions: `Value` protocol

#### `.debugString() -> string`

Returns a string representation of the symbol. This includes a suggestive
prefix `.`. It also includes `anon-` before the name if this is an anonymous
symbol.

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

#### `makeSymbol(name) -> symbol`

Finds and returns the interned (non-anonymous) symbol with the given `name`
(which must be a string). If the so-named symbol has not been created yet,
this function creates it.

**Note:** If you want an anonymous symbol, call `.makeAnonymous()` on a
symbol with the desired name, e.g. `makeSymbol(name).makeAnonymous()`
or (if you know the name statically) `@.foo.makeAnonymous()`.

#### `symbolIsInterned(symbol) -> symbol | void`

Returns `symbol` if it is interned (that is, non-anonymous). Returns void
otherwise.

#### `symbolName(symbol) -> string`

Returns the name of `symbol`.

<br><br>
### In-Language Definitions

(none)
