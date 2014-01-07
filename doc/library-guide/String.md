Samizdat Layer 0: Core Library
==============================

String
------

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(string, other) <> string | void`

Calls `totEq`.

#### `perOrder(string, other) <> int`

Calls `totOrder`.

#### `totEq(string1, string2) <> string | void`

Compares two strings. Two strings are equal if they have equal characters in
identical orders.

#### `totOrder(string1, string2) <> int`

Compares two strings for order. Strings order by pairwise
corresponding-character comparison, with a strict prefix always ordering
before its longer brethren.

<br><br>
### Generic Function Definitions: One-Offs

#### `toInt(string) <> int`

Given a single-character string, returns the character code
of its sole character, as an int. It is an error (terminating
the runtime) if `string` is not a string of size 1.

#### `toNumber(string) <> int`

Same as `toInt(string)`.

#### `toString(string) <> string`

Returns the argument.


<br><br>
### Generic Function Definitions: `Collection` and `Sequence` protocols

#### `cat(string, more*) <> string`

Returns a string consisting of the concatenation of the contents
of all the argument strings, in argument order.

**Syntax Note:** Used in the translation of interpolated string forms.

#### `del(string, n) <> string`

Returns a string like the given one, but without the `n`th character.

#### `get(string, key) <> . | void`

Defined as per the `Sequence` protocol.

#### `keyList(string) <> list`

Defined as per the `Sequence` protocol.

#### `nth(string, n) <> . | void`

Gets the nth character of the string, as a single-element string.

#### `nthMapping(string, n) <> map | void`

Gets the nth mapping of the string.

#### `put(string, n, char) <> string`

Returns a string like the given one, but with the `n`th character replaced
with the given `char`, or added if `n == sizeOf(string)`. It is an error
if `char` is not a string of size `1`.

#### `reverse(string) <> string`

Returns a string like the one given, except with characters in the opposite
order.

#### `sizeOf(string) <> int`

Returns the number of characters in the string.

#### `sliceExclusive(string, start, end?) <> string`

Returns an end-exclusive slice of the given string.

#### `sliceInclusive(string, start, end?) <> string`

Returns an end-inclusive slice of the given string.


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(string) <> list`

Returns a list of all the characters of `string`, in order.

#### `filter(string, filterFunction) <> list`

Filters the characters of `string` using `filterFunction`.

#### `nextValue(string, box) <> generator | void`

On a non-empty string, calls `store(box, string[0])` and returns
`string[1..]`. On an empty string, calls `store(box)` and returns void.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
