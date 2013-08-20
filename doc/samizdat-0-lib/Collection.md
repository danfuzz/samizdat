Samizdat Layer 0: Core Library
==============================

Collections (multiple-value containers)
---------------------------------------

There is no `Collection` type per se. There is, however, effectively a
`Collection` protocol as well as a couple sub-protocols, as documented
here.


<br><br>
### Generic Function Definitions

#### `size(collection) <> int`

Returns the number of elements in the given collection. For core collections,
this is defined as follows:

* `Int` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an int is 1, which
  is the size of both `0` and `-1`.

* `String` &mdash; the number of characters.

* `List` &mdash; the number of elements.

* `Map` &mdash; the number of mappings (bindings).

<br><br>
### In-Language Definitions

(none)
