Samizdat Layer 0: Core Library
==============================

General Low-Layer Values
------------------------

<br><br>
### Primitive Definitions

#### `lowSize value <> intlet`

Returns the "size" of the given value. Every low-layer value has
a size, defined as follows:

* `intlet` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an intlet is 1, which
  is the size of both `@0` and `@-1`.

* `string` &mdash; the number of characters.

* `listlet` &mdash; the number of elements.

* `maplet` &mdash; the number of mappings (bindings).

* `uniqlet` &mdash; always `@0`.

* `highlet` &mdash; `@0` for a valueless highlet, or `@1` for a
  valued highlet.

#### `lowType value <> string`

Returns the type name of the low-layer type of the given value. The
result will be one of: `@intlet` `@string` `@listlet` `@maplet`
`@uniqlet` `@highlet`


<br><br>
### In-Language Definitions

#### `isHighlet value <> boolean`

Returns `true` iff the given value is a highlet.

#### `isIntlet value <> boolean`

Returns `true` iff the given value is an intlet.

#### `isListlet value <> boolean`

Returns `true` iff the given value is a listlet.

#### `isMaplet value <> boolean`

Returns `true` iff the given value is a maplet.

#### `isString value <> boolean`

Returns `true` iff the given value is a string.

#### `isUniqlet value <> boolean`

Returns `true` iff the given value is a uniqlet.
