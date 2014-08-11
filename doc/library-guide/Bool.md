Samizdat Layer 0: Core Library
==============================

Bool
----

A `Bool` is both a truth value and a single-bit `Bitwise` value. The
keywords `true` and `false` refer to the only two values of this class.



<br><br>
### Method Definitions: `Value` protocol

#### `perEq(bool, other) -> bool | void`

Default implementation.

#### `perOrder(bool, other) -> int`

Default implementation.

#### `totalEq(bool1, bool2) -> bool | void`

Compares two boolean values. This is only logical-true if the two given
values are the same.

#### `totalOrder(bool1, bool2) -> int`

Compares the given boolean values, ordering `false` before `true`.


<br><br>
### Method Definitions: One-Offs

#### `toInt(bool) -> int`

Returns `0` given `false`, or `1` given `true`.

#### `toNumber(bool) -> int`

Same as `toInt`.


<br><br>
### Method Definitions: `Bitwise` protocol

#### `and(bool1, bool2) -> bool`

Straightforward protocol implementation.

#### `bit(bool, int) -> int`

Returns `1` if called as `bit(true, 0)`. Returns `0` for any other
valid pair of arguments.

#### `bitSize(bool) -> int`

Returns `1`, always.

#### `not(bool) -> bool`

Straightforward protocol implementation.

#### `or(bool1, bool2) -> bool`

Straightforward protocol implementation.

#### `shl(bool, int) -> bool`

Returns the given argument if shifted by `0`, or if `false` is shifted
by any amount. Returns `false` if `true` is right-shifted by any
amount.

It is a terminal error to try to shift `true` left by any amount.

#### `xor(bool1, bool2) -> bool`

Straightforward protocol implementation.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
