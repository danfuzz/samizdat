Samizdat Layer 0: Core Library
==============================

Boolean
-------

A `Boolean` is both a truth value and a single-bit `Bitwise` value.

<br><br>
### Generic Function Definitions: `Bitwise` protocol

#### `and(bitwise1, bitwise2) <> bitwise`

Straightforward protocol implementation.

#### `bit(bitwise, int) <> int`

Returns `1` if called as `bit(true, 0)`. Returns `0` for any other
valid pair of arguments.

#### `bitSize(bitwise) <> int`

Returns `1`, always.

#### `not(bitwise) <> bitwise`

Straightforward protocol implementation.

#### `or(bitwise1, bitwise2) <> bitwise`

Straightforward protocol implementation.

#### `shl(bitwise, int) <> bitwise`

Returns the given argument if shifted by `0`, or if `false` is shifted
by any amount. Returns `false` if `true` is right-shifted by any
amount.

It is a terminal error to try to shift `true` left by any amount.

#### `xor(bitwise1, bitwise2) <> bitwise`

Straightforward protocol implementation.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### Constant: `false`

The boolean value false. It is defined as `@[Boolean: 0]`.

#### Constant: `true`

The boolean value true. It is defined as `@[Boolean: 1]`.
