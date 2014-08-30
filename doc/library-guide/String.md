Samizdat Layer 0: Core Library
==============================

String
------

<br><br>
### Method Definitions: `String` protocol

#### `.toSymbol() -> symbol`

Returns an interned symbol whose name is `this`.


<br><br>
### Method Definitions: `Value` protocol

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
### Method Definitions: One-Offs

#### `.toInt() -> int`

Given a single-character string, returns the character code
of its sole character, as an int. It is an error (terminating
the runtime) if `string` is not a string of size 1.

#### `.toNumber() -> int`

Same as `.toInt()`.

#### `.toString() -> string`

Returns `this`.


<br><br>
### Method Definitions: `Collection` and `Sequence` protocols

#### `.cat(more*) -> string`

Returns a string consisting of the concatenation of the contents
of all the argument strings, in argument order.

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
