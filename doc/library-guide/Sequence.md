Samizdat Layer 0: Core Library
==============================

proto::Sequence
---------------

The `Sequence` protocol is for collections keyed by zero-based int index.
All `Sequence`s are also `Collection`s.


### Generic Function Definitions: `Sequence` protocol

#### `get(sequence, key) <> . | void`

(Refinement of `Collection` protocol.)

For sequences, `get` behaves the same as `nth`, except that it returns
void for `key` values that are either non-ints or negative (instead of
reporting a terminal error).

#### `keyList(sequence) <> list`

(Refinement of `Collection` protocol.)

Returns the list `[(0..!sizeOf(sequence))*]`.

#### `reverse(sequence) <> sequence`

Returns a sequence just like the given one, except with elements in
the opposite order.
