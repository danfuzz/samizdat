Samizdat Layer 0: Core Library
==============================

Intlets
-------

<br><br>
### Primitive Definitions

#### `iadd intlet1 intlet2 <> intlet`

Returns the sum of the given values.

#### `iand intlet1 intlet2 <> intlet`

Returns the binary-and (intersection of all one-bits) of the given values.

#### `ibit intlet n <> intlet`

Returns as an intlet (`@0` or `@1`) the bit value in the first
argument at the bit position (zero-based) indicated by the second
argument. It is an error (terminating the runtime) if the second
argument is negative.

#### `idiv intlet1 intlet2 <> intlet`

Returns the quotient of the given values (first over second),
truncated (rounded toward zero) to yield an intlet result. It is an
error (terminating the runtime) if the second argument is `@0`.

#### `imod intlet1 intlet2 <> intlet`

Returns the division modulus of the given values (first over
second). The sign of the result will always match the sign of the
second argument. It is an error (terminating the runtime) if the
second argument is `@0`.

`imod x y` can be thought of as the smallest magnitude value `m` with
the same sign as `y` such that `isub x m` is a multiple of `y`.

`imod x y` can be defined as `irem (iadd (irem x y) y) y`.

**Note:** This differs from the `irem` in the treatment of negative
numbers.

#### `imul intlet1 intlet2 <> intlet`

Returns the product of the given values.

#### `ineg intlet <> intlet`

Returns the negation (same magnitude, opposite sign) of the given
value.

#### `inot intlet <> intlet`

Returns the binary complement (all bits opposite) of the given value.

#### `ior intlet1 intlet2 <> intlet`

Returns the binary-or (union of all one-bits) of the given values.

#### `irem intlet1 intlet2 <> intlet`

Returns the truncated-division remainder of the given values (first
over second). The sign of the result will always match the sign of the
first argument. It is an error (terminating the runtime) if the second
argument is `@0`.

`irem x y` can be defined as `isub x (imul (idiv x y) y)`.

**Note:** This differs from the `imod` in the treatment of negative
numbers.

#### `ishl intlet shift <> intlet`

Returns the first argument (an intlet) bit-shifted an amount indicated
by the second argument (also an intlet). If `shift` is positive, this
is a left-shift operation. If `shift` is negative, this is a right-shift
operation. If `shift` is `@0`, this is a no-op, returning the first
argument unchanged.

**Note:** The `shift` argument is not limited in any particular way (not
masked, etc.).

#### `ishr intlet shift <> intlet`

Returns the first argument bit-shifted by an amount indicated by the
second argument. This is identical to `ishl`, except that the sense of
positive and negative `shift` is reversed.

**Note:** Unlike some other languages, there is no unsigned right-shift
operation in *Samizdat Layer 0*. This is because intlets are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative intlets.

#### `isub intlet1 intlet2 <> intlet`

Returns the difference of the given values (first minus second).

#### `ixor intlet1 intlet2 <> intlet`

Returns the binary-xor (bitwise not-equal) of the given values.


<br><br>
### In-Language Definitions

#### `intletSign intlet <> intlet`

Returns the sign of the given value: `@-1` for negative values,
`@1` for positive values, or `@0` for `@0`.
