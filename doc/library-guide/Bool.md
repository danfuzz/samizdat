Samizdat Layer 0: Core Library
==============================

Bool
----

A `Bool` is both a truth value and a single-bit `Bitwise` value. The
keywords `true` and `false` refer to the only two values of this class.


<br><br>
### Class Method Definitions

#### `class.castFrom(value) -> is Bool | void`

This class knows how to cast as follows:

* `Bool` &mdash; Returns `value`.
* `Core` &mdash; Returns `value`.
* `Int` &mdash; Returns `false` given `0`, or `1` given `true`.
* `Value` &mdash; Returns `value`.

#### `class.fromLogic(value?) -> is Bool`

Returns the boolean equivalent of the argument-or-not. This is a bridge
from value-or-void logic. If given an argument, this returns `true`. If
given no argument, this returns `false`.

**Syntax Note:** Used in the translation of `expression??` forms.


<br><br>
### Method Definitions: `Value` protocol

#### `.castToward(cls) -> . | void`

This class knows how to cast as follows:

* `Bool` &mdash; Returns `this`.
* `Core` &mdash; Returns `this`.
* `Int` &mdash; Returns `0` given `false`, or `true` given `1`.
* `Value` &mdash; Returns `this`.

#### `.crossEq(other) -> is Bool | void`

Compares two boolean values. This is only logical-true if the two given
values are the same.

#### `.crossOrder(other) -> is Symbol`

Compares the given boolean values, ordering `false` before `true`.

#### `.debugString() -> is String`

Returns `"true"` or `"false'`, in the reasonably expected cases.

#### `.perEq(other) -> is Bool | void`

Default implementation.

#### `.perOrder(other) -> is Symbol`

Default implementation.


<br><br>
### Method Definitions: `Bool` protocol

#### `.toLogic() -> == true | void`

Converts `this` into a value useful for logic operations. In particular, this
returns `true` if given `true` and void if given `false`.

**Syntax Note:** Used in the translation of `expr**` forms.


<br><br>
### Method Definitions: `Bitwise` protocol

#### `.and(other) -> is Bool`

Straightforward protocol implementation.

#### `.bit(int) -> is Int`

Returns `1` if called as `bit(true, 0)`. Returns `0` for any other
valid pair of arguments.

#### `.bitSize() -> is Int`

Returns `1`, always.

#### `.not() -> is Bool`

Straightforward protocol implementation.

#### `.or(other) -> is Bool`

Straightforward protocol implementation.

#### `.shl(int) -> is Bool`

Returns the given argument if shifted by `0`, or if `false` is shifted
by any amount. Returns `false` if `true` is right-shifted by any
amount.

It is a terminal error to try to shift `true` left by any amount.

#### `.xor(other) -> is Bool`

Straightforward protocol implementation.
