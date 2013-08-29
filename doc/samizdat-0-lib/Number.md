Samizdat Layer 0: Core Library
==============================

Number
------

`Number` is a protocol for values that can be treated numerically.
All the generics in this protocol are defined to return the same type
as they operate on. The multiple-argument generics in this protocol are
defined to only take arguments of the same type.

In cases where the implied computation isn't defined or isn't definable,
methods in this protocol must terminate with an error rather than
produce an incorrect result or return void.

<br><br>
### Generic Function Definitions: `Number` protocol

#### `abs(number) <> number`

Returns the absolute value of the given value. This is `value` itself if
it is non-negative, or `-value` if it is negative.

#### `add(number1, number2) <> number`

Returns the sum of the given values.

#### `div(number1, number2) <> number`

Returns the quotient of the given values (first over second),
to yield an number result, using traditional division.

#### `divEu(number1, number2) <> number`

Returns the quotient of the given values (first over second),
using Euclidean division, to yield a number result.

Euclidean division differs from traditional division when given
arguments of opposite sign. The usual equivalence holds with Euclidean
division that `x == (x // y) + (x %% y)` (for `y != 0`), but the
modulo result is guaranteed to be non-negative, and this means that
division is correspondingly different.

#### `mod(number1, number2) <> number`

Returns the remainder after traditional division of the given values (first
over second). The sign of the result will always match the sign of the
first argument.

The modulo operation `x % y` can be defined in terms of truncated division as
`x - trunc(x / y) * y`.

#### `modEu(number1, number2) <> number`

Returns the remainder after Euclidean division of the given values (first
over second). The sign of the result will always be positive.
It is an error (terminating the runtime) if the second
argument is `0`.

The Euclidean modulo operation `x %% y` can be defined in terms of
Euclidean division as `x - (x // y) * y`.

#### `mul(number1, number2) <> number`

Returns the product of the given values.

#### `neg(number) <> number`

Returns the negation (same magnitude, opposite sign) of the given
value.

#### `sign(number) <> number`

Returns the sign of the given value: `-1` for negative values,
`1` for positive values, or `0` for `0`.

#### `sub(number1, number2) <> number`

Returns the difference of the given values (first minus second).


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
