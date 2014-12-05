Samizdat Layer 0: Core Library
==============================

String
------


<br><br>
### Class Method Definitions

#### `class.castFrom(value) -> is String | void`

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
  symbol with the desired name, e.g. `Class.cast(Symbol, name).toUnlisted()`
  or (if you know the name statically) `@foo.toUnlisted()`.

* `Value` &mdash; Returns `this`.

#### `.crossEq(other) -> is String | void`

Compares two strings. Two strings are equal if they have equal characters in
identical orders.

#### `.crossOrder(other) -> is Symbol`

Compares two strings for order. Strings order by pairwise
corresponding-character comparison, with a strict prefix always ordering
before its longer brethren.

#### `.perEq(other) -> is String | void`

Default implementation.

#### `.perOrder(other) -> is Symbol`

Default implementation.


<br><br>
### Method Definitions: `Collection` and `Sequence` protocols

#### `.cat(more*) -> is String`

Returns a string consisting of the concatenation of the contents of `ths`
and all the arguments, in argument order. Arguments are allowed to be
either strings or symbols.

**Syntax Note:** Used in the translation of interpolated string forms.

#### `.del(ns*) -> is String`

Returns a string like the given one, but without the indicated characters
(by index). If a given index is repeated more than once, it has the same
effect as if it were only mentioned once.

#### `.get(key) -> . | void`

Defined as per the `Sequence` protocol.

#### `.get_size() -> is Int`

Returns the number of characters in the string.

#### `.keyList() -> is List`

Defined as per the `Sequence` protocol.

#### `.nth(n) -> . | void`

Gets the nth character of the string, as a single-element string.

#### `.repeat(count) -> is String`

Returns a string consisting of `count` repetitions of the contents of `this`.
`count` must be a non-negative int.

#### `.reverse() -> is String`

Returns a string like the one given, except with characters in the opposite
order.

#### `.sliceExclusive(start, end?) -> is String`

Returns an end-exclusive slice of the given string.

#### `.sliceInclusive(start, end?) -> is String`

Returns an end-inclusive slice of the given string.

#### `.valueList(n) -> is List`

Defined as per the `Sequence` protocol.


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(filterFunction) -> is List`

Collects or filters the characters of `string`.

#### `.fetch() -> is String | void`

Returns void on an empty string. Returns `this` on a single-character string.
Terminates with an error in all other cases.

#### `.nextValue(box) -> is String | void`

On a non-empty string, calls `box.store(string[0])` and returns
`string[1..]`. On an empty string, this returns void.
