Samizdat Layer 0: Core Library
==============================

General Low-Layer Values
------------------------

<br><br>
### Primitive Definitions

#### `lowSize(value) <> int`

Returns the "size" of the given value. Every low-layer value has
a size, defined as follows:

* `int` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an int is 1, which
  is the size of both `0` and `-1`.

* `string` &mdash; the number of characters.

* `list` &mdash; the number of elements.

* `map` &mdash; the number of mappings (bindings).

* `uniqlet` &mdash; always `0`.

* `token` &mdash; `0` for a valueless token, or `1` for a
  valued token.

#### `lowType(value) <> string`

Returns the type name of the low-layer type of the given value. The
result will be one of: `"int"` `"string"` `"list"` `"map"`
`"uniqlet"` `"deriv"`


<br><br>
### In-Language Definitions

#### `isToken(value) <> logic`

Returns the given `value` if it is a token. Returns void if not.

#### `isInt(value) <> logic`

Returns the given `value` if it is an int. Returns void if not.

#### `isList(value) <> logic`

Returns the given `value` if it is a list. Returns void if not.

#### `isMap(value) <> logic`

Returns the given `value` if it is a map. Returns void if not.

#### `isString(value) <> logic`

Returns the given `value` if it is a string. Returns void if not.

#### `isUniqlet(value) <> logic`

Returns the given `value` if it is a uniqlet. Returns void if not.
