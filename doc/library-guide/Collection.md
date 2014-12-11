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

#### `.cat(more*) -> isa Collection`

Returns the concatenation of all of the given `more*` values to `this`.
The `more*` values must be "compatible" with `this` (as defined by the class
of `this`), and the result is typically expected to be of the same class as
`this`. It is an error (terminating the runtime) if one of the arguments is
incompatible.

To the extent that a value is unconstrained in terms of its constituent
elements and their arrangement, the result of concatenation consists
of the elements of all the original values, in order, in the order of the
arguments.

For classes that have element constraints, a concatenation will not
necessarily contain all the original constituent elements, and the order might
be different. See individual implementation docs for details.

#### `.del(keys*) -> isa Collection`

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

Returns the constituent element of `this` that corresponds to the given
`key`. `key` is an arbitrary value. Returns void if there is no unique
corresponding value for the given `key` (including if `key` is not
bound in `this` at all).

**Syntax Note:** This is the method that underlies the `value[key]`
syntactic form.

#### `.get_size() -> isa Int`

Gets the size (element count) of `this`.

**Syntax Note:** This is the method underlying the `#value` syntactic
form (prefix `#` operator).

#### `.keyList() -> isa List`

Returns the list of keys mapped by the collection.

#### `.valueList() -> isa List`

Returns the list of values mapped by the collection.
