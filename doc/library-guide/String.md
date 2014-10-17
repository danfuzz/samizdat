Samizdat Layer 0: Core Library
==============================

String
------


<br><br>
### Class Method Definitions

#### `.castFrom(value) -> string | void`

This class knows how to cast as follows:

* `Core` &mdash; Returns `value`.

* `Int` &mdash; Returns a single-character string with the Unicode code
  point `value`.

* `String` &mdash; Returns `value`.

* `Symbol` &mdash; Returns the name of `value` as a string.

* `Value` &mdash; Returns `value`.


<br><br>
### Method Definitions: `Value` protocol

#### `.castToward(cls) -> . | void`

This class knows how to cast as follows:

* `Core` &mdash; Returns `this`.

* `Int` &mdash; Returns the Unicode code point of the sole character of
  `this`. Only works on single-character strings.

* `String` &mdash; Returns `this`.

* `Symbol` &mdash; Returns an interned symbol whose name is `this`.

  **Note:** If you want an unlisted symbol, call `.toUnlisted()` on a
  symbol with the desired name, e.g. `cast(Symbol, name).toUnlisted()`
  or (if you know the name statically) `@foo.toUnlisted()`.

* `Value` &mdash; Returns `this`.

#### `.perEq(other) -> string | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> string | void`

Compares two strings. Two strings are equal if they have equal characters in
identical orders.

#### `.totalOrder(other) -> int`

Compares two strings for order. Strings order by pairwise
corresponding-character comparison, with a strict prefix always ordering
before its longer brethren.


<br><br>
### Method Definitions: `Collection` and `Sequence` protocols

#### `.cat(more*) -> string`

Returns a string consisting of the concatenation of the contents of `ths`
and all the arguments, in argument order. Arguments are allowed to be
either strings or symbols.

**Syntax Note:** Used in the translation of interpolated string forms.

#### `.del(n) -> string`

Returns a string like the given one, but without the `n`th character.

#### `.get(key) -> . | void`

Defined as per the `Sequence` protocol.

#### `.get_size() -> int`

Returns the number of characters in the string.

#### `.keyList() -> list`

Defined as per the `Sequence` protocol.

#### `.nth(n) -> . | void`

Gets the nth character of the string, as a single-element string.

#### `.nthMapping(n) -> map | void`

Gets the nth mapping of the string.

#### `.put(n, char) -> string`

Returns a string like the given one, but with the `n`th character replaced
with the given `char`, or added if `n == #string`. It is an error
if `char` is not a string of size `1`.

#### `.reverse() -> string`

Returns a string like the one given, except with characters in the opposite
order.

#### `.sliceExclusive(start, end?) -> string`

Returns an end-exclusive slice of the given string.

#### `.sliceInclusive(start, end?) -> string`

Returns an end-inclusive slice of the given string.

#### `.valueList(n) -> list`

Defined as per the `Sequence` protocol.


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(filterFunction) -> list`

Collects or filters the characters of `string`.

#### `.fetch() -> string | void`

Returns void on an empty string. Returns `this` on a single-character string.
Terminates with an error in all other cases.

#### `.nextValue(box) -> generator | void`

On a non-empty string, calls `box.store(string[0])` and returns
`string[1..]`. On an empty string, this returns void.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
