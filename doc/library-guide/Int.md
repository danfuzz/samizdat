Samizdat Layer 0: Core Library
==============================

Int
---

<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> int | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> int | void`

Compares the integer values of two ints.

#### `.totalOrder(other) -> int`

Compares the integer values of two ints, ordering by value in the usual
manner.


<br><br>
### Method Definitions: `Bitwise` protocol

#### `.and(other) -> int`

Straightforward protocol implementation.

#### `.bit(other) -> int`

Straightforward protocol implementation.

**Note**: Bits at or beyond `this.bitSize()` are considered to be the
same as `this.bit(this.bitSize() - 1)`.

#### `.bitSize() -> int`

Straightforward protocol implementation.

**Note:** The minimum size of an int is `1`, which is the size of
both `0` and `-1`.

#### `.not() -> bitwise`

Straightforward protocol implementation.

#### `.or(other) -> bitwise`

Straightforward protocol implementation.

#### `.shl(int) -> bitwise`

Straightforward protocol implementation.

**Note:** Unlike some other languages, there is no unsigned int right-shift
operation in Samizdat. This is because ints are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative ints.

#### `.xor(other) -> bitwise`

Straightforward protocol implementation.


<br><br>
### Method Definitions: `Number` protocol

#### `.abs() -> int`

Straightforward protocol implementation.

#### `.add(other) -> int`

Straightforward protocol implementation.

#### `.div(other) -> int`

Straightforward protocol implementation, using truncated division.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.divEu(other) -> int`

Straightforward protocol implementation, using truncated Euclidean division.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.mod(other) -> int`

Straightforward protocol implementation. The truncated modulo operation
`x % y` can be defined in terms of truncated division as `x - (x / y) * y`.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.modEu(other) -> int`

Straightforward protocol implementation. The truncated Euclidean modulo
operation `x %% y` can be defined in terms of truncated Euclidean division
as `x - (x // y) * y`.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.mul(other) -> int`

Straightforward protocol implementation.

#### `.neg() -> int`

Straightforward protocol implementation.

#### `.sign() -> int`

Straightforward protocol implementation.

#### `.sub(other) -> number`

Straightforward protocol implementation.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
