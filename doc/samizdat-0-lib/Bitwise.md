Samizdat Layer 0: Core Library
==============================

Bitwise
-------

`Bitwise` is a protocol for values that can be treated as fixed-width
lists of bits.

Almost all of the generics in this protocol are defined to return the same
type that they operate on (as a first argument). Some of the multiple-argument
generics in this protocol are defined to take arguments only of the same
types, but a couple take a second argument specifically of type `Int`
(as noted).


<br><br>
### Generic Function Definitions: `Bitwise` protocol

#### `and(bitwise1, bitwise2) <> bitwise`

Returns the binary-and (intersection of all one-bits) of the given values.

#### `bit(bitwise, int) <> int`

Returns as an int (`0` or `1`) the bit value in the first
argument at the bit position (zero-based) indicated by the second
argument. It is an error (terminating the runtime) if the second
argument is negative.

#### `bitSize(bitwise) <> int`

Returns the number of significant bits (not bytes) in
the value when represented in twos-complement form, including a
high-order sign bit.

#### `not(bitwise) <> bitwise`

Returns the binary complement (all bits opposite) of the given value.

#### `or(bitwise1, bitwise2) <> bitwise`

Returns the binary-or (union of all one-bits) of the given values.

#### `shl(bitwise, int) <> bitwise`

Returns the first argument (a bitwise) bit-shifted an amount indicated
by the second argument (an int). If `shift` is positive, this
is a left-shift operation. If `shift` is negative, this is a right-shift
operation. If `shift` is `0`, this is a no-op, returning the first
argument unchanged.

**Note:** The `shift` argument is not limited in any particular way (not
masked, etc.).

#### `shr(bitwise, int) <> bitwise`

Returns the first argument bit-shifted by an amount indicated by the
second argument. This is identical to `shl`, except that the sense of
positive and negative `shift` is reversed.

#### `xor(bitwise1, bitwise2) <> bitwise`

Returns the binary-xor (bitwise not-equal) of the given values.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
