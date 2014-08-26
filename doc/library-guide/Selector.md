Samizdat Layer 0: Core Library
==============================

Selector
-------

A `Selector` is an identifier used when referring to methods and other
language constructs.


<br><br>
### Method Definitions: `Value` protocol

#### `.debugName() -> string`

Returns the name of the selector, as a string.

#### `.debugString() -> string`

Returns a string representation of the selector. This includes a suggestive
prefix `.`. It also includes `anon-` before the name if this is an anonymous
selector.

#### `.perEq(other) -> selector | void`

Default implementation.

#### `.perOrder(other) -> int | void`

Default implementation.

#### `.totalEq(other) -> builtin | void`

Default implementation.

#### `.totalOrder(other) -> int | void`

Orders selectors by internedness (primary) and name (secondary), with
interned selectors getting ordered *before* anonymous selectors. Two
different anonymous selectors with the same name are considered unordered
(not equal).

<br><br>
### Method Definitions: `Function` protocol

#### `.call(args+) -> . | void`

Calls the given selector with the given arguments. This performs method
dispatch on `args[0]`.


<br><br>
### Primitive Definitions

#### `makeAnonymousSymbol(name) -> selector`

Creates and returns a new anonymous selector. Even though anonymous,
it must have a name (which must be a string). The anonymity comes from
the fact that the selector returned from this function will never be
found by doing a name lookup, as with `makeInternedSymbol`.

#### `makeInternedSymbol(name) -> selector`

Finds and returns the interned (non-anonymous) selector with the given `name`
(which must be a string). If the so-named selector has not been created yet,
this function creates it.

#### `selectorIsInterned(selector) -> selector | void`

Returns `selector` if it is interned (that is, non-anonymous). Returns void
otherwise.

#### `selectorName(selector) -> string`

Returns the name of `selector`.

<br><br>
### In-Language Definitions

(none)
