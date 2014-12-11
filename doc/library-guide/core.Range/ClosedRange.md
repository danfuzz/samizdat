Samizdat Layer 0: Core Library
==============================

core.Range :: ClosedRange
-------------------------

A `ClosedRange` represents a sequential range of either numeric or
character values, with a finite size.


<br><br>
### Class Method Definitions:

#### `class.new(firstValue, size, optIncrement?) -> isa ClosedRange`

Creates a closed range. `firstValue` must be either a number or
a single-character string. `size` must be an int.
`optIncrement` defaults to `1` and if supplied must be a number.

**Note:** If `size` is non-positive (`0` or negative), this returns a valid,
but voided, instance.

#### `class.newExclusive(firstValue, limit, optIncrement?) -> isa ClosedRange`

End-exclusive range generator for int or single-character strings.
This is a convenient wrapper for `.new()`.

Takes an initial value, which must either be an int or a single-character
string, a limit of the same class as the initial value, and an optional
numeric (always numeric) increment which defaults to `1`. The first `nextValue`
call to the resulting generator yields the `firstValue`, and each subsequent
call yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string). If the value yielded
would be the same as or beyond the given `limit`, the generator becomes
voided.

Special cases:

Special cases:

* If `firstValue` is at or beyond `limit` (beyond in the direction of the
  increment), then this returns a valid, but voided, instance.

* Barring the above, if the increment is `0`, the resulting generator is
  of size `1` (just yielding `firstValue` and then becoming voided).

**Syntax Note:** Used in the translation of `(expression..!expression)`
forms.

#### `class.newInclusive(firstValue, limit, optIncrement?) -> isa ClosedRange`

End-inclusive range generator for int or single-character strings.
This is a convenient wrapper for `.new()`.

Takes an initial value, which must either be an int or a single-character
string, a limit of the same class as the initial value, and an optional
numeric (always numeric) increment which defaults to `1`. The first `nextValue`
call to the resulting generator yields the `firstValue`, and each subsequent
call yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string). If the value yielded
would be beyond the given `limit`, the generator becomes voided.

As a special case, if the increment is `0`, the resulting generator is
forced to be of size `1` (just yielding `firstValue` and then becoming
voided).

**Syntax Note:** Used in the translation of `(expression..expression)`
forms.


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> isa List`

Returns all the elements of the range as a list.

#### `.nextValue(box) -> isa ClosedRange | void`

If non-empty, yields the first element of the range, and returns a range
representing the remainder. If empty, returns void.
