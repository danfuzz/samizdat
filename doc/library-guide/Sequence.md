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

#### `.keyList() -> isa List`

(Refinement of `Collection` protocol.)

Returns the list `[0..!#this]`.

#### `.nth(n) -> . | void`

Returns the nth (zero-based) element of the sequence. Returns void if `n < 0`
or `n >= #this`. It is an error (terminating the runtime) if `n` is not an
`Int`.

#### `.repeat(count) -> isa Sequence`

Returns a sequence consisting of `count` repetitions of the contents of `this`.
`count` must be a non-negative int.

#### `.reverse() -> isa Sequence`

Returns a sequence just like the given one, except with elements in
the opposite order.

**Syntax Note:** This is the function that underlies the `^value`
syntactic form (prefix `^` operator).

#### `.reverseNth(n) -> . | void`

Returns the nth (zero-based) element of the sequence, counting from the
end of the sequence. This is equivalent to `this.reverse().nth(n)`.

**Syntax Note:** This is the function that underlies the `seq[^value]`
syntactic form.

#### `.sliceExclusive(start, end?) -> isa Sequence | void`

Returns a sequence of the same class as `this`, consisting of an
index-based "slice" of elements taken from `this`, from the `start`
index (inclusive) through the `end` index (exclusive). `start` and `end`
must both be ints. `end` defaults to `#this - 1` if omitted.

As special cases (in order):
* It is an error (terminating the runtime) if either `start` or `end` is
  not an int.
* If any of `end < 0`, `start > #this`, or `end < start`, then this returns
  void.
* If `start < 0`, then it is treated as if it were passed as `0`.
* If `end > #this`, then it is treated as if it were passed as `#this`.
* If `start == end` (after modification per previous two items), then this
  returns an empty sequence.

In the usual case, `start < end`, `start < #this`, and `end > start`.
In this case, the result is a sequence consisting of elements of `this`
starting at index `start` and continuing through, but not including, index
`end`.

#### `.sliceGeneral(style, start, end?) -> isa Sequence | void`

Returns a sequence of the same class as `this`, consisting of an index-based
"slice" of elements taken from `sequence`, from the `start` index through the
`end` index.

The `start` is always an inclusive index. `style` indicates whether the
end is inclusive (`@inclusive`) or exclusive (`@exclusive`).

Each of `start` and `end` must be a `@fromStart` or `@fromEnd` record, with
`{value: int}` as the payload. The class indicates which end of the sequence
is to be counted from.

This method in turn calls one of `.sliceExclusive()` or `.sliceInclusive()` to
perform the actual slicing.

#### `.sliceInclusive(start, end?) -> isa Sequence | void`

Returns a sequence of the same class as `this`, consisting of an
index-based "slice" of elements taken from `this`, from the `start`
index (inclusive) through the `end` index (inclusive). `start` and `end`
must both be ints. `end` defaults to `#this - 1` if omitted.

This is equivalent to calling `this.sliceExclusive(start, end + 1)`.

#### `.valueList() -> isa List`

(Refinement of `Collection` protocol.)

Returns the elements of `this`, always as a list per se.
