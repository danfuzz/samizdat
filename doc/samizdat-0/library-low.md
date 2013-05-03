Samizdat Layer 0: Core Library
==============================

General Low-Layer Values
------------------------

<br><br>
### Primitive Definitions

#### `lowOrder value1 value2 <> intlet`

Returns the order of the two given values in the total order of
Samizdat low-layer values. This returns one of `@-1 @0 @1` indicating
how the two values sort with each other, using the reasonably
standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Ordering is calculated as follows:

The major order is by type &mdash; `intlet < stringlet < listlet <
maplet < uniqlet < highlet` &mdash; and minor order is type-dependant.

* Intlets order by integer value.

* Listlets and stringlets order by pairwise corresponding-element
  comparison, with a strict prefix always ordering before its
  longer brethren.

* Maplets order by first comparing corresponding ordered lists
  of keys with the same rules as listlet comparison. If the key
  lists are identical, then the result is the comparison of
  corresponding lists of values, in key order.

* Any given uniqlet never compares as identical to anything but
  itself. Any two uniqlets have a consistent, transitive, and
  commutative &mdash; but otherwise arbitrary &mdash; ordering.

* Highlets compare by type as primary, and value as secondary.
  With types equal, highlets without a value order earlier than
  ones with a value.

#### `lowOrderIs value1 value2 check1 check2? <> boolean`

The two values are compared as with `lowOrder`. The intlet
result of that comparison are checked for equality with
the one or two check values. If the comparison result is equal
to either check value, this function returns `true`. Otherwise
it returns `false`.

**Note:** This function exists in order to provide a primitive
comparison function that returns a boolean. Without it (or something
like it), there would be no way to define boolean-returning
comparators in-language.

#### `lowSize value <> intlet`

Returns the "size" of the given value. Every low-layer value has
a size, defined as follows:

* `intlet` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an intlet is 1, which
  is the size of both `@0` and `@-1`.

* `stringlet` &mdash; the number of characters.

* `listlet` &mdash; the number of elements.

* `maplet` &mdash; the number of mappings (bindings).

* `uniqlet` &mdash; always `@0`.

* `highlet` &mdash; `@0` for a valueless highlet, or `@1` for a
  valued highlet.

#### `lowType value <> stringlet`

Returns the type name of the low-layer type of the given value. The
result will be one of: `@intlet` `@stringlet` `@listlet` `@maplet`
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

#### `isStringlet value <> boolean`

Returns `true` iff the given value is a stringlet.

#### `isUniqlet value <> boolean`

Returns `true` iff the given value is a uniqlet.
