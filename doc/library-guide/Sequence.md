Samizdat Layer 0: Core Library
==============================

Sequence Protocol
-----------------

The `Sequence` protocol is for collections keyed by zero-based int index.
All `Sequence`s are also `Collection`s.


### Method Definitions: `Sequence` protocol

#### `.get(key) -> . | void`

(Refinement of `Collection` protocol.)

For sequences, `get` behaves the same as `nth`, except that it returns
void for `key` values that are either non-ints or negative (instead of
reporting a terminal error).

#### `.keyList() -> list`

(Refinement of `Collection` protocol.)

Returns the list `[0..!#this]`.

#### `.nthMapping(n) -> . | void`

(Refinement of `Collection` protocol.)

When the return value is non-void, the key of the returned mapping is the
given `n`.

#### `.nth(n) -> . | void`

(Implementation of `OneOff` method.)

Returns the nth (zero-based) element of the sequence.

#### `.reverse() -> sequence`

Returns a sequence just like the given one, except with elements in
the opposite order.

**Syntax Note:** This is the function that underlies the `^this`
syntactic form (prefix `^` operator).

#### `.sliceExclusive(start, end?) -> sequence | void`

Returns a sequence of the same class as `this`, consisting of an
index-based "slice" of elements taken from `this`, from the `start`
index (inclusive) through the `end` index (exclusive). `start` and `end`
must both be ints. `end` defaults to `#this - 1` if omitted.

In the usual case, `start < end`, `start < #this`, and `end > start`.
In this case, the result is the expected slice. If `start < 0`, then it is
treated as if it were passed as `0`. If `end > #this`, then it is
treated as if it were passed as `#this`.

If `start == end`, this returns an empty sequence.

In all other cases (as long as the type restrictions hold), this returns void.

#### `.sliceInclusive(start, end?) -> sequence | void`

Returns a sequence of the same class as `this`, consisting of an
index-based "slice" of elements taken from `this`, from the `start`
index (inclusive) through the `end` index (inclusive). `start` and `end`
must both be ints. `end` defaults to `#this - 1` if omitted.

This is equivalent to calling `this.sliceExclusive(start, end + 1)`.

#### `.valueList() -> list`

(Refinement of `Collection` protocol.)

Returns the elements of `this`, always as a list per se.


<br><br>
### Functions

#### `reverseNth(sequence, n) -> . | void`

Returns the nth (zero-based) element of the sequence, but counting backward
from the end of the sequence. Returns void if `n < 0` or `n >= #sequence`.
It is an error (terminating the runtime) if `n` is not an `Int`.

In general, the result of this call should be the same as saying
`sequence.reverse.nth(n)`.

#### `sliceGeneral(sequence, style, start, end?) -> sequence | void`

Returns a sequence of the same class as `sequence`, consisting of an
index-based "slice" of elements taken from `sequence`, from the `start`
index through the `end` index.

The `start` is always an inclusive index. `style` indicates whether the
end is inclusive (`@inclusive`) or exclusive (`@exclusive`).

Each of `start` and `end` must be a `@fromStart` or `@fromEnd` record, with
a `{value: int}` as the payload. The class indicates which end of the
sequence is to be counted from.

This function in turn calls one of `sliceExclusive` or `sliceInclusive` to
perform the actual slicing.
