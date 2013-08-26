Samizdat Layer 0: Core Library
==============================

ClosedRange
-----------

A `ClosedRange` represents a sequential range of either numeric or
character values, with a finite size.


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(range) <> list`

Returns all the elements of the range as a list.

#### `nextValue(range, box) <> range | void`

Yields the first element of the range, and returns either a range representing
the remaining elements (if there are any) or `nullGenerator` if there are
none left.

**Note:** It is possible to manually construct a `ClosedRange` that will
return void, but with normal usage, this never happens.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `makeClosedRange(firstValue, increment, size) <> range | nullGenerator`

Creates a closed range. `firstValue` must be either a number or
a single-character string. `increment` must be a number. `size` must be
a non-negative int.

If `size` is positive, this returns a `ClosedRange`. Otherwise
(`size` is `0`), this returns `nullGenerator`.

#### `makeExclusiveRange(firstValue, increment, limit) <> range`

End-exclusive range generator for int or single-character strings.
This is a convenient wrapper for `makeClosedRange`.

Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first `nextValue` call to
the resulting generator yields the `firstValue`, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string). If the value yielded
would be the same as or beyond the given `limit`, the generator becomes
voided.

As a special case, if `increment` is `0`, the resulting generator is
forced to be of size `1` (just yielding `firstValue` and then becoming
voided).

**Syntax Note:** Used in the translation of `expression..!expression`
forms.

#### `makeInclusiveRange(firstValue, increment, limit) <> range`

End-inclusive range generator for int or single-character strings.
This is a convenient wrapper for `makeClosedRange`.

Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first `nextValue` call to
the resulting generator yields the `firstValue`, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string). If the value yielded
would be beyond the given `limit`, the generator becomes voided.

As a special case, if `increment` is `0`, the resulting generator is
forced to be of size `1` (just yielding `firstValue` and then becoming
voided).

**Syntax Note:** Used in the translation of `expression..expression`
forms.
