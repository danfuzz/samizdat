Samizdat Layer 0: Core Library
==============================

OpenRange
---------

An `OpenRange` represents an open-ended (unbounded) sequential range of
either numeric or character values.


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(range) <> list`

Reports a fatal error, as `OpenRange` values are unbounded generators.

#### `nextValue(range, box) <> range | void`

Yields the first element of the range, and returns a range representing
the remaining elements.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions: `Range` module

#### `makeOpenRange(firstValue, increment) <> range`

Creates an open (never voided) range generator for numbers or single-character
strings. `firstValue` must be either a number or a single-character string.
`increment` must be a number. The first `nextValue` call to the
resulting generator yields the `firstValue`, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string).

**Syntax Note:** Used in the translation of `(expression..)` forms.
