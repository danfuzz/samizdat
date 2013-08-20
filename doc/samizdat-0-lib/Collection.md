Samizdat Layer 0: Core Library
==============================

Collections (multiple-value containers)
---------------------------------------

There is no `Collection` type per se. There is, however, effectively a
`Collection` protocol as well as a couple sub-protocols, as documented
here.


<br><br>
### Generic Function Definitions

#### `get(collection, key) <> . | void`

Returns the element of the collection that corresponds to the given
`key`. `key` is an arbitrary value. Returns void if there is no unique
corresponding value for the given `key` (including if `key` is not
bound in the collection at all).

#### `nth(collection, n) <> . | void`

Returns the nth (zero-based) element of the collection, in its defined
fixed order. Returns void if `n >= size(collection)`. It is an error
(terminating the runtime) if `n < 0` or if `n` is not an `Int`.

#### `size(collection) <> int`

Returns the number of elements in the given collection.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
