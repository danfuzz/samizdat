Samizdat Layer 0: Core Library
==============================

Int
---

<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> is Int | void`

Compares the integer values of two ints.

#### `.crossOrder(other) -> is Symbol`

Compares the integer values of two ints, ordering by value in the usual
manner.

#### `.perEq(other) -> is Int | void`

Default implementation.

#### `.perOrder(other) -> is Symbol`

Default implementation.


<br><br>
### Method Definitions: `Bitwise` protocol

#### `.and(other) -> is Int`

Straightforward protocol implementation.

#### `.bit(other) -> is Int`

Straightforward protocol implementation.

**Note**: Bits at or beyond `this.bitSize()` are considered to be the
same as `this.bit(this.bitSize() - 1)`.

#### `.bitSize() -> is Int`

Straightforward protocol implementation.

**Note:** The minimum size of an int is `1`, which is the size of
both `0` and `-1`.

#### `.not() -> is Bitwise`

Straightforward protocol implementation.

#### `.or(other) -> is Bitwise`

Straightforward protocol implementation.

#### `.shl(int) -> is Bitwise`

Straightforward protocol implementation.

**Note:** Unlike some other languages, there is no unsigned int right-shift
operation in Samizdat. This is because ints are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative ints.

#### `.xor(other) -> is Bitwise`

Straightforward protocol implementation.


<br><br>
### Method Definitions: `Number` protocol

#### `.abs() -> is Int`

Straightforward protocol implementation.

#### `.add(other) -> is Int`

Straightforward protocol implementation.

#### `.div(other) -> is Int`

Straightforward protocol implementation, using truncated division.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.divEu(other) -> is Int`

Straightforward protocol implementation, using truncated Euclidean division.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.mod(other) -> is Int`

Straightforward protocol implementation. The truncated modulo operation
`x % y` can be defined in terms of truncated division as `x - (x / y) * y`.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.modEu(other) -> is Int`

Straightforward protocol implementation. The truncated Euclidean modulo
operation `x %% y` can be defined in terms of truncated Euclidean division
as `x - (x // y) * y`.

It is an error (terminating the runtime) if the second argument
is `0`, as there is no "infinity" representation in the class.

#### `.mul(other) -> is Int`

Straightforward protocol implementation.

#### `.neg() -> is Int`

Straightforward protocol implementation.

#### `.sign() -> is Int`

Straightforward protocol implementation.

#### `.sub(other) -> is Number`

Straightforward protocol implementation.
