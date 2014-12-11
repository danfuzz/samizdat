Samizdat Layer 0: Core Library
==============================

Bitwise Protocol
----------------

`Bitwise` is a protocol for values that can be treated as fixed-width
lists of bits.

Almost all of the methods in this protocol are defined to return the same
class that they operate on (as a first argument). Some of the multiple-argument
methods in this protocol are defined to take arguments only of the same
classes, but a couple take a second argument specifically of class `Int`
(as noted).


<br><br>
### Method Definitions: `Bitwise` protocol

#### `.and(other) -> isa Bitwise`

Returns the binary-and (intersection of all one-bits) of the given values.

#### `.bit(int) -> isa Int`

Returns as an int (`0` or `1`) the bit value in the first
argument at the bit position (zero-based) indicated by the second
argument. It is an error (terminating the runtime) if the second
argument is negative.

#### `.bitSize() -> isa Int`

Returns the number of significant bits (not bytes) in
the value when represented in twos-complement form, including a
high-order sign bit.

#### `.not() -> isa Bitwise`

Returns the binary complement (all bits opposite) of the given value.

#### `.or(other) -> isa Bitwise`

Returns the binary-or (union of all one-bits) of the given values.

#### `.shl(int) -> isa Bitwise`

Returns the first argument (a bitwise) bit-shifted an amount indicated
by the second argument (an int). If `shift` is positive, this
is a left-shift operation. If `shift` is negative, this is a right-shift
operation. If `shift` is `0`, this is a no-op, returning the first
argument unchanged.

**Note:** The `shift` argument is not limited in any particular way (not
masked, etc.).

#### `shr(bitwise, int) -> isa Bitwise`

Returns the first argument bit-shifted by an amount indicated by the
second argument, with the opposite sense of shift direction compared
to `shl`. This must always be equivalent to saying `this.shl(-int)`.

#### `.xor(other) -> isa Bitwise`

Returns the binary-xor (bitwise not-equal) of the given values.
