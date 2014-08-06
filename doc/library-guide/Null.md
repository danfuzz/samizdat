Samizdat Layer 0: Core Library
==============================

Null
----

There is only one value of class `Null`. It's refered to by the keyword
`null` in the language.


<br><br>
### Generic Function Definitions: `Null` protocol

#### `perEq(null, other) -> bool | void`

Default implementation.

#### `perOrder(null, other) -> int`

Default implementation.

#### `totalEq(null1, null2) -> bool | void`

Compares two `Null` values. This is only logical-true if the two given
values are both `null` per se. That is, this function returns `null` if
called as `totalEq(null, null)` and will terminate with an error in
all other cases.

#### `totalOrder(bool1, bool2) -> int`

Compares the given `Null` values for order. As there is only one instance
of `Null`, this will only ever return `0` when called appropriately. That is,
this function returns `0` if called as `totalOrder(null, null)` and will
terminate with an error in all other cases.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
