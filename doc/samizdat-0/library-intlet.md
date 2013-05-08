Samizdat Layer 0: Core Library
==============================

Integers
--------

<br><br>
### Primitive Definitions

#### `iadd integer1 integer2 <> integer`

Returns the sum of the given values.

#### `iand integer1 integer2 <> integer`

Returns the binary-and (intersection of all one-bits) of the given values.

#### `ibit integer n <> integer`

Returns as an integer (`@0` or `@1`) the bit value in the first
argument at the bit position (zero-based) indicated by the second
argument. It is an error (terminating the runtime) if the second
argument is negative.

#### `idiv integer1 integer2 <> integer`

Returns the quotient of the given values (first over second),
truncated (rounded toward zero) to yield an integer result. It is an
error (terminating the runtime) if the second argument is `@0`.

#### `imod integer1 integer2 <> integer`

Returns the division modulus of the given values (first over
second). The sign of the result will always match the sign of the
second argument. It is an error (terminating the runtime) if the
second argument is `@0`.

`imod x y` can be thought of as the smallest magnitude value `m` with
the same sign as `y` such that `isub x m` is a multiple of `y`.

`imod x y` can be defined as `irem (iadd (irem x y) y) y`.

**Note:** This differs from the `irem` in the treatment of negative
numbers.

#### `imul integer1 integer2 <> integer`

Returns the product of the given values.

#### `ineg integer <> integer`

Returns the negation (same magnitude, opposite sign) of the given
value.

#### `inot integer <> integer`

Returns the binary complement (all bits opposite) of the given value.

#### `ior integer1 integer2 <> integer`

Returns the binary-or (union of all one-bits) of the given values.

#### `irem integer1 integer2 <> integer`

Returns the truncated-division remainder of the given values (first
over second). The sign of the result will always match the sign of the
first argument. It is an error (terminating the runtime) if the second
argument is `@0`.

`irem x y` can be defined as `isub x (imul (idiv x y) y)`.

**Note:** This differs from the `imod` in the treatment of negative
numbers.

#### `ishl integer shift <> integer`

Returns the first argument (an integer) bit-shifted an amount indicated
by the second argument (also an integer). If `shift` is positive, this
is a left-shift operation. If `shift` is negative, this is a right-shift
operation. If `shift` is `@0`, this is a no-op, returning the first
argument unchanged.

**Note:** The `shift` argument is not limited in any particular way (not
masked, etc.).

#### `ishr integer shift <> integer`

Returns the first argument bit-shifted by an amount indicated by the
second argument. This is identical to `ishl`, except that the sense of
positive and negative `shift` is reversed.

**Note:** Unlike some other languages, there is no unsigned right-shift
operation in *Samizdat Layer 0*. This is because integers are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative integers.

#### `isub integer1 integer2 <> integer`

Returns the difference of the given values (first minus second).

#### `ixor integer1 integer2 <> integer`

Returns the binary-xor (bitwise not-equal) of the given values.


<br><br>
### In-Language Definitions

#### `integerSign integer <> integer`

Returns the sign of the given value: `@-1` for negative values,
`@1` for positive values, or `@0` for `@0`.
