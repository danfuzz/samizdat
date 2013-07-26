Samizdat Layer 0: Core Library
==============================

Ints
----

<br><br>
### Primitive Definitions

#### `iabs(int) <> int`

Returns the absolute value of the given value. This is `value` itself if
it is non-negative, or `-value` if it is negative values.

#### `iadd(int1, int2) <> int`

Returns the sum of the given values.

#### `iand(int1, int2) <> int`

Returns the binary-and (intersection of all one-bits) of the given values.

#### `ibit(int, n) <> int`

Returns as an int (`0` or `1`) the bit value in the first
argument at the bit position (zero-based) indicated by the second
argument. It is an error (terminating the runtime) if the second
argument is negative.

#### `idiv(int1, int2) <> int`

Returns the quotient of the given values (first over second),
truncated (rounded toward zero) to yield an int result. It is an
error (terminating the runtime) if the second argument is `0`.

#### `imod(int1, int2) <> int`

Returns the division modulus of the given values (first over
second). The sign of the result will always match the sign of the
second argument. It is an error (terminating the runtime) if the
second argument is `0`.

`imod(x, y)` can be thought of as the smallest magnitude value `m` with
the same sign as `y` such that `isub(x, m)` is a multiple of `y`.

`imod(x, y)` can be defined as `irem(iadd(irem(x, y), y), y)`.

**Note:** This differs from the `irem` in the treatment of negative
numbers.

#### `imul(int1, int2) <> int`

Returns the product of the given values.

#### `ineg(int) <> int`

Returns the negation (same magnitude, opposite sign) of the given
value.

#### `inot(int) <> int`

Returns the binary complement (all bits opposite) of the given value.

#### `intGet(int, value)` <> int

Map-style access. This returns the same as `ibit(int, value)` if
`value` is an int in the range `0...coreSizeOf(int)`. Otherwise this
returns void.

#### `intNth(int, value)` <> int

List-style access. This returns the same as `ibit(int, value)` if
`value` is an int in the range `0...coreSizeOf(int)`. Otherwise, if
`value` is a non-negative int, this returns void. Otherwise, this
terminates the runtime with an error.

#### `ior(int1, int2) <> int`

Returns the binary-or (union of all one-bits) of the given values.

#### `irem(int1, int2) <> int`

Returns the truncated-division remainder of the given values (first
over second). The sign of the result will always match the sign of the
first argument. It is an error (terminating the runtime) if the second
argument is `0`.

`irem(x, y)` can be defined as `isub(x, imul(idiv(x, y), y))`.

**Note:** This differs from the `imod` in the treatment of negative
numbers.

#### `ishl(int, shift) <> int`

Returns the first argument (an int) bit-shifted an amount indicated
by the second argument (also an int). If `shift` is positive, this
is a left-shift operation. If `shift` is negative, this is a right-shift
operation. If `shift` is `0`, this is a no-op, returning the first
argument unchanged.

**Note:** The `shift` argument is not limited in any particular way (not
masked, etc.).

#### `ishr(int, shift) <> int`

Returns the first argument bit-shifted by an amount indicated by the
second argument. This is identical to `ishl`, except that the sense of
positive and negative `shift` is reversed.

**Note:** Unlike some other languages, there is no unsigned right-shift
operation in *Samizdat Layer 0*. This is because ints are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative ints.

#### `isign(int) <> int`

Returns the sign of the given value: `-1` for negative values,
`1` for positive values, or `0` for `0`.

#### `isub(int1, int2) <> int`

Returns the difference of the given values (first minus second).

#### `ixor(int1, int2) <> int`

Returns the binary-xor (bitwise not-equal) of the given values.


<br><br>
### In-Language Definitions

(none)
