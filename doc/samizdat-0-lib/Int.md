Samizdat Layer 0: Core Library
==============================

Int
---

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(int, other) <> int | void`

Calls `totEq`.

#### `perOrder(int, other) <> int`

Calls `totOrder`.

#### `totEq(int1, int2) <> int | void`

Compares the integer values of two ints.

#### `totOrder(int1, int2) <> int`

Compares the integer values of two ints, ordering by value in the usual
manner.


<br><br>
### Generic Function Definitions: `Bitwise` protocol

#### `and(int1, int2) <> int`

Straightforward protocol implementation.

#### `bit(int1, int2) <> int`

Straightforward protocol implementation.

**Note**: Bits at or beyond `bitSize(int1)` are considered to be the
same as `bit(int1, bitSize(int1) - 1)`.

#### `bitSize(bitwise) <> int`

Straightforward protocol implementation.

**Note:** The minimum size of an int is `1`, which is the size of
both `0` and `-1`.

#### `not(bitwise) <> bitwise`

Straightforward protocol implementation.

#### `or(bitwise1, bitwise2) <> bitwise`

Straightforward protocol implementation.

#### `shl(bitwise, int) <> bitwise`

Straightforward protocol implementation.

**Note:** Unlike some other languages, there is no unsigned int right-shift
operation in *Samizdat Layer 0*. This is because ints are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative ints.

#### `xor(bitwise1, bitwise2) <> bitwise`

Straightforward protocol implementation.


<br><br>
### Generic Function Definitions: `Number` protocol

#### `abs(int) <> int`

Straightforward protocol implementation.

#### `add(int1, int2) <> int`

Straightforward protocol implementation.

#### `div(int1, int2) <> int`

Straightforward protocol implementation, using truncated division.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the type.

#### `divEu(int1, int2) <> int`

Straightforward protocol implementation, using truncated Euclidean division.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the type.

#### `mod(int1, int2) <> int`

Straightforward protocol implementation. The truncated modulo operation
`x % y` can be defined in terms of truncated division as `x - (x / y) * y`.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the type.

#### `modEu(int1, int2) <> int`

Straightforward protocol implementation. The truncated Euclidean modulo
operation `x %% y` can be defined in terms of truncated Euclidean division
as `x - (x // y) * y`.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the type.

#### `mul(int1, int2) <> int`

Straightforward protocol implementation.

#### `neg(int) <> int`

Straightforward protocol implementation.

#### `sign(int) <> int`

Straightforward protocol implementation.

#### `sub(int1, int2) <> number`

Straightforward protocol implementation.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
