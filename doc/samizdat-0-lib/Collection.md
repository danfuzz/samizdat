Samizdat Layer 0: Core Library
==============================

Collections (multiple-value containers)
---------------------------------------

There is no `Collection` type per se. There is, however, effectively a
`Collection` protocol as well as a couple sub-protocols, as documented
here.


<br><br>
### Generic Function Definitions

#### `cat(collection, more*) <> collection`

Returns the concatenation of all of the given collections. The collections
must all be of the same type, and the result is the same type as the given
type. It is an error (terminating the runtime) if one of the arguments is
of a different type.

To the extent that a collection is unconstrained in terms of its
elements and their arrangement, the result of concatenation consists
of the elements of all the original collections, in order, in the order
of the arguments.

For collections that have element constraints, a concatenation will
not necessarily contain all the original elements, and the order may
be different. See their docs for details.

**Note:** To account for the possibility of passing *no* arguments to
concatenate (e.g. when handling a list of arguments generically), include
a first argument of the empty value of the desired type, e.g.
`""` to ensure string concatenation.

#### `del(collection, key) <> collection`

Returns a collection just like the given `collection`, except that
the mapping for the given `key` is removed. If the `collection`
does not have a particular given key, then this returns `collection`.

**Note:** On sequence-like collections, this shifts elements after the
deleted element down in index, such that there is no gap in the resulting
collection.

#### `get(collection, key) <> . | void`

Returns the element of the collection that corresponds to the given
`key`. `key` is an arbitrary value. Returns void if there is no unique
corresponding value for the given `key` (including if `key` is not
bound in the collection at all).

#### `nth(collection, n) <> . | void`

Returns the nth (zero-based) element of the collection, in its defined
fixed order. Returns void if `n >= size(collection)`. It is an error
(terminating the runtime) if `n < 0` or if `n` is not an `Int`.

#### `put(collection, key, value) <> collection`

Returns a collection just like the given `collection`, except that
the mapping for the given `key` is to the given `value`. This will
replace an existing mapping for the `key`, or add a new one.

It is an error (terminating the runtime) if the `key` or `value` is
invalid for `collection`.

**Note:** On sequence-like collections, the only valid keys are ints
in the range `0..size(collection)` (inclusive of the size).

**Note:** To differentiate between adding a new mapping versus replacing
a mapping, either check the sizes of the original and result, or
check for the existence of `key` in the original.

#### `size(collection) <> int`

Returns the number of elements in the given collection.

#### `slice(collection, start, end?) <> collection`

Returns a collection of the same type as `collection`, consisting of an
index-based "slice" of elements taken from `collection`, from the `start`
index (inclusive) through the `end` index (exclusive). `start` and `end`
must both be ints, must be valid indices into `collection` *or* the
collection size, and must form a range (though possibly empty) with
`start <= end`. If `end` is not specified, it defaults to the end of the
collection.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `butFirst(collection) <> collection | void`

Returns a collection consisting of all the elements of the given `collection`
except for the first element (that is, all but the first element). If
`collection` is empty, this returns void. This is similar to
`slice(collection, 1)`, except that this function returns void instead of
reporting an error when `collection` is empty.

#### `butLast(collection) <> collection | void`

Returns a collection consisting of all the elements of the given `collection`
except for the last element (that is, all but the last element). If
`collection` is empty, this returns void. This is similar to
`slice(collection, 0, size(collection) - 1)`, except that this function
returns void instead of reporting an error when `collection` is empty.

#### `first(collection) <> . | void`

Returns the first element of the given `collection` or void if it is empty.
This is just a convenient shorthand for `nth(collection, 0)`.

#### `last(collection) <> . | void`

Returns the last element of the given `collection` or void if the it is empty.
This is just a convenient shorthand for
`nth(collection, size(collection) - 1)`.
