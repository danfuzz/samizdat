Samizdat Layer 0: Core Library
==============================

Null
----

There is only one value of class `Null`. It's refered to by the keyword
`null` in the language.


<br><br>
### Method Definitions: `Value` protocol

#### `.debugString() -> string`

Returns `"null"`.

#### `.perEq(other) -> null | void`

Default implementation.

#### `.perOrder(other) -> symbol`

Default implementation.

#### `.totalEq(other) -> null | void`

Compares two `Null` values. This is only logical-true if the two given
values are both `null` per se. That is, this function returns `null` if
called as `null.totalEq(null)` and will terminate with an error in
all other cases.

#### `.totalOrder(other) -> symbol`

Compares the given `Null` values for order. As there is only one instance
of `Null`, this will only ever return `@same` when called appropriately. That
is, this function returns `@same` if called as `null.totalOrder(null)` and
will terminate with an error in all other cases.
