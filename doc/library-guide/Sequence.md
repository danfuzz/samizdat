Samizdat Layer 0: Core Library
==============================

proto.Sequence
--------------

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

Returns the list `[0..!#sequence]`.

#### `nthMapping(sequence, n) <> . | void`

(Refinement of `Collection` protocol.)

When the return value is non-void, the key of the returned mapping is the
given `n`.

#### `nth(sequence, n) <> . | void`

(Implementation of `OneOff` generic function.)

Returns the nth (zero-based) element of the sequence.

#### `reverse(sequence) <> sequence`

Returns a sequence just like the given one, except with elements in
the opposite order.

**Syntax Note:** This is the function that underlies the `^sequence`
syntactic form (prefix `^` operator).

#### `sliceExclusive(sequence, start, end?) <> sequence | void`

Returns a sequence of the same type as `sequence`, consisting of an
index-based "slice" of elements taken from `sequence`, from the `start`
index (inclusive) through the `end` index (exclusive). `start` and `end`
must both be ints. `end` defaults to `#sequence - 1` if omitted.

In the usual case, `start < end`, `start < #sequence`, and `end > start`.
In this case, the result is the expected slice. If `start < 0`, then it is
treated as if it were passed as `0`. If `end > #sequence`, then it is
treated as if it were passed as `#sequence`.

If `start == end`, this returns an empty sequence.

In all other cases (as long as the type restrictions hold), this returns void.

#### `sliceInclusive(sequence, start, end?) <> sequence | void`

Returns a sequence of the same type as `sequence`, consisting of an
index-based "slice" of elements taken from `sequence`, from the `start`
index (inclusive) through the `end` index (inclusive). `start` and `end`
must both be ints. `end` defaults to `#sequence - 1` if omitted.

This is equivalent to calling `sliceExclusive(sequence, start, end + 1)`.


<br><br>
### In-Language Definitions

#### `nthFromEnd(sequence, n) <> . | void`

Returns the nth (zero-based) element of the sequence, but counting backward
from the end of the sequence. Returns void if `n < 0` or `n >= #sequence`.
It is an error (terminating the runtime) if `n` is not an `Int`.

#### `sliceGeneral(sequence, style, start, end?) <> sequence | void`

Returns a sequence of the same type as `sequence`, consisting of an
index-based "slice" of elements taken from `sequence`, from the `start`
index through the `end` index.

The `start` is always an inclusive index. `style` indicates whether the
end is inclusive (`@inclusive`) or exclusive (`@exclusive`).

Each of `start` and `end` must be a derived data value of type
`@fromStart` or `@fromEnd` with an int payload. The type indicates which
end of the sequence is to be counted from.

This function in turn calls one of `sliceExclusive` or `sliceInclusive` to
perform the actual slicing.
