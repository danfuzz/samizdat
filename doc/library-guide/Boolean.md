Samizdat Layer 0: Core Library
==============================

Boolean
-------

A `Boolean` is both a truth value and a single-bit `Bitwise` value.

<br><br>
### Generic Function Definitions: One-Offs

#### `toInt(boolean) <> int`

Returns `0` given `false`, or `1` given `true`.

#### `toNumber(boolean) <> int`

Same as `toInt`.


<br><br>
### Generic Function Definitions: `Bitwise` protocol

#### `and(boolean1, boolean2) <> boolean`

Straightforward protocol implementation.

#### `bit(boolean, int) <> int`

Returns `1` if called as `bit(true, 0)`. Returns `0` for any other
valid pair of arguments.

#### `bitSize(boolean) <> int`

Returns `1`, always.

#### `not(boolean) <> boolean`

Straightforward protocol implementation.

#### `or(boolean1, boolean2) <> boolean`

Straightforward protocol implementation.

#### `shl(boolean, int) <> boolean`

Returns the given argument if shifted by `0`, or if `false` is shifted
by any amount. Returns `false` if `true` is right-shifted by any
amount.

It is a terminal error to try to shift `true` left by any amount.

#### `xor(boolean1, boolean2) <> boolean`

Straightforward protocol implementation.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### Constant: `false`

The boolean value false. It is defined as `@Boolean(0)`.

#### Constant: `true`

The boolean value true. It is defined as `@Boolean(1)`.
