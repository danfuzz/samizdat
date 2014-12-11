Samizdat Layer 0: Core Library
==============================

core.Range :: OpenRange
-----------------------

An `OpenRange` represents an open-ended (unbounded) sequential range of
either numeric or character values.


<br><br>
### Class Method Definitions

#### `class.new(firstValue, optIncrement?) -> isa OpenRange`

Creates an open (never voided) range generator for numbers or single-character
strings. `firstValue` must be either a number or a single-character string.
`optIncrement` defaults to `1` and if supplied must be a number.

The first `nextValue` call to the resulting generator yields the `firstValue`,
and each subsequent call yields the previous value plus the given increment
(converted to a single-character string if `firstValue` is a string).

**Syntax Note:** Used in the translation of `(expression..)` forms.


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> n/a  ## Always reports error.`

Reports a fatal error, as `OpenRange` values are unbounded generators.

#### `.nextValue(box) -> isa OpenRange`

Yields the first element of the range, and returns a range representing
the remaining elements.
