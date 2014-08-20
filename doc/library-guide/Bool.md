Samizdat Layer 0: Core Library
==============================

Bool
----

A `Bool` is both a truth value and a single-bit `Bitwise` value. The
keywords `true` and `false` refer to the only two values of this class.



<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> bool | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> bool | void`

Compares two boolean values. This is only logical-true if the two given
values are the same.

#### `.totalOrder(other) -> int`

Compares the given boolean values, ordering `false` before `true`.


<br><br>
### Method Definitions: One-Offs

#### `.toInt() -> int`

Returns `0` given `false`, or `1` given `true`.

#### `.toNumber() -> int`

Same as `.toInt()`.


<br><br>
### Method Definitions: `Bitwise` protocol

#### `.and(other) -> bool`

Straightforward protocol implementation.

#### `.bit(int) -> int`

Returns `1` if called as `bit(true, 0)`. Returns `0` for any other
valid pair of arguments.

#### `.bitSize() -> int`

Returns `1`, always.

#### `.not() -> bool`

Straightforward protocol implementation.

#### `.or(other) -> bool`

Straightforward protocol implementation.

#### `.shl(int) -> bool`

Returns the given argument if shifted by `0`, or if `false` is shifted
by any amount. Returns `false` if `true` is right-shifted by any
amount.

It is a terminal error to try to shift `true` left by any amount.

#### `.xor(other) -> bool`

Straightforward protocol implementation.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
