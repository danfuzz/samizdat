Samizdat Layer 0: Core Library
==============================

Collection Protocol
-------------------

A `Collection` is a set of key-value mappings. Every `Collection` has
a defined order of iteration. *Some* `Collection`s allow arbitrary
keys.

The `Collection` protocol is defined in a `proto.` module as one
would expect.

<br><br>
### Method Definitions: `Collection` protocol

#### `.cat(more*) -> collection`

(Implementation of `OneOff` method.)

Concatenates the given collections.

#### `.del(keys*) -> collection`

Returns a collection just like the given one, except that the mappings for the
given `keys`, if any, are removed. If any of the `keys` is a duplicate, then
it is no different as if that key is only specified once. If `this` does not
bind any of the given keys, then this method returns `this`.

**Note:** On sequence-like collections, this shifts elements after the
deleted element down in index, such that there is no gap in the resulting
collection. However, all such shifting happens *after* selecting of
elements to delete; so, for example, `[0, 1, 2].del(0, 1)` returns `[2]` and
not `[1]`. Similarly, `[0, 1, 2].del(0, 0)` returns `[1, 2]`.

#### `.get(key) -> . | void`

(Implementation of `OneOff` method.)

Gets the value in `this` associated with the given `key`.

#### `.get_size() -> int`

(Implementation of `OneOff` method.)

Gets the size (element count) of `this`.

#### `.keyList() -> list`

Returns the list of keys mapped by the collection.

#### `.nthMapping(n) -> map | void`

Returns the nth (zero-based) mapping of the collection, in its predefined
iteration order. The result is a one-element map of the key and corresponding
value. Returns void if `n < 0` or `n >= #this`. It is an error
(terminating the runtime) if `n` is not an `Int`.

#### `.put(key, value) -> collection`

Returns a collection just like the given one, except that
the mapping for the given `key` is to the given `value`. This will
replace an existing mapping for the `key`, or add a new one.

It is an error (terminating the runtime) if the `key` or `value` is
invalid for `this`.

**Note:** On sequence-like collections, the only valid keys are ints
in the range `0..#this` (inclusive of the size).

**Note:** To differentiate between adding a new mapping versus replacing
a mapping, either check the sizes of the original and result, or
check for the existence of `key` in the original.

#### `.valueList() -> list`

Returns the list of values mapped by the collection.
