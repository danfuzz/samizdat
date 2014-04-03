Samizdat Layer 0: Core Library
==============================

core.Collection / proto.Collection
----------------------------------

A `Collection` is a set of key-value mappings. Every `Collection` has
a defined order of iteration. *Some* `Collection`s allow arbitrary
keys.

The `Collection` protocol is defined in a `proto.` module as one
would expect.

<br><br>
### Generic Function Definitions: `Collection` protocol

#### `cat(collection, more*) <> collection`

(Implementation of `OneOff` generic function.)

Concatenates the given collections.

#### `del(collection, key) <> collection`

Returns a collection just like the given `collection`, except that
the mapping for the given `key` is removed. If the `collection`
does not have a particular given key, then this returns `collection`.

**Note:** On sequence-like collections, this shifts elements after the
deleted element down in index, such that there is no gap in the resulting
collection.

#### `get(collection, key) <> . | void`

(Implementation of `OneOff` generic function.)

Gets the value in `collection` associated with the given `key`.

#### `keyList(collection) <> list`

Returns the list of keys mapped by the collection.

#### `nthMapping(collection, n) <> map | void`

Returns the nth (zero-based) mapping of the collection, in its predefined
iteration order. The result is a one-element map of the key and corresponding
value. Returns void if `n < 0` or `n >= #collection`. It is an error
(terminating the runtime) if `n` is not an `Int`.

#### `put(collection, key, value) <> collection`

Returns a collection just like the given `collection`, except that
the mapping for the given `key` is to the given `value`. This will
replace an existing mapping for the `key`, or add a new one.

It is an error (terminating the runtime) if the `key` or `value` is
invalid for `collection`.

**Note:** On sequence-like collections, the only valid keys are ints
in the range `0..sizeOf(collection)` (inclusive of the size).

**Note:** To differentiate between adding a new mapping versus replacing
a mapping, either check the sizes of the original and result, or
check for the existence of `key` in the original.

#### `sizeOf(collection) <> int`

(Implementation of `OneOff` generic function.)

Gets the size (element count) of the `collection`.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
