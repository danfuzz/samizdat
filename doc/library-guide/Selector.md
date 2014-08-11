Samizdat Layer 0: Core Library
==============================

Selector
-------

A `Selector` is an identifier used when referring to methods.


<br><br>
### Method Definitions: `Value` protocol

#### `debugName(selector) -> string`

Returns the name of the selector, as a string.

#### `debugString(selector) -> string`

Returns a string representation of the selector. This includes a suggestive
prefix `.`. It also includes `anon-` before the name if this is an anonymous
selector.

#### `perEq(selector, other) -> selector | void`

Default implementation.

#### `perOrder(selector, other) -> int | void`

Default implementation.

#### `totalEq(builtin1, builtin2) -> builtin | void`

Default implementation.

#### `totalOrder(builtin1, builtin2) -> int | void`

Default implementation.


<br><br>
### Method Definitions: `Function` protocol

#### `call(selector, args+) -> . | void`

Calls the given selector with the given arguments. This performs method
dispatch on `args[0]`.


<br><br>
### Primitive Definitions

#### `makeAnonymousSelector(name) -> selector`

Creates and returns a new anonymous selector. Even though anonymous,
it must have a name (which must be a string). The anonymity comes from
the fact that the selector returned from this function will never be
found by doing a name lookup, as with `selectorFromName`.

#### `selectorFromName(name) -> selector`

Finds and returns the (non-anonymous) selector with the given `name` (which
must be a string). If the so-named selector has not been created yet, this
function creates it.


<br><br>
### In-Language Definitions

(none)
