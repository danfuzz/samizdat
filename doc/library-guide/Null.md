Samizdat Layer 0: Core Library
==============================

Null
----

There is only one value of class `Null`. It's refered to by the keyword
`null` in the language.


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> logic`

Compares two `Null` values. This is only logical-true if the two given
values are both `null` per se. That is, this function returns `null` if
called as `null.crossEq(null)` and will terminate with an error in
all other cases.

#### `.crossOrder(other) -> isa Symbol`

Compares the given `Null` values for order. As there is only one instance
of `Null`, this will only ever return `@same` when called appropriately. That
is, this function returns `@same` if called as `null.crossOrder(null)` and
will terminate with an error in all other cases.

#### `.debugString() -> isa String`

Returns `"null"`.

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol`

Default implementation.
