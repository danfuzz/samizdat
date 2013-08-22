Samizdat Layer 0: Core Library
==============================

Strings
-------

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(string1, string2) <> string | void`

Compares two strings. Two strings are equal if they have equal characters in
identical orders.

#### `perOrder(string1, string2) <> int`

Compares two strings for order. Strings order by pairwise
corresponding-character comparison, with a strict prefix always ordering
before its longer brethren.

<br><br>
### Generic Function Definitions: `Collection` protocol

#### `cat(string, more*) <> string`

Returns a string consisting of the concatenation of the contents
of all the argument strings, in argument order.

**Syntax Note:** Used in the translation of interpolated string forms.

#### `get(string, key) <> . | void`

This is identical to `nth(string, key)`, except that the result of passing
a negative or non-`Int` value for `key` is void and not an error.

#### `nth(string, n) <> . | void`

Gets the nth character of the string, as a single-element string.

#### `size(string) <> int`

Returns the number of characters in the string.

#### `slice(string, start, end?) <> string`

Returns a slice of the given string.


<br><br>
### Primitive Definitions

#### `charFromInt(int) <> string`

Returns a single-character string that consists of the character
code indicated by the given int argument, which must be in the
range for representation as an unsigned 32-bit quantity.

#### `intFromChar(string) <> int`

Given a single-character string, returns the character code
of its sole character, as an int. It is an error (terminating
the runtime) if `string` is not a string of size 1.

#### `stringSlice(list, start, end?) <> list`

Returns a string consisting of a "slice" of elements of the given
string, from the `start` index (inclusive) through the `end` index
(exclusive). `start` and `end` must both be ints, must be valid indices
into `string` (with the string size being a valid index), and must form a
range (though possibly empty) with `start <= end`. If `end` is not
specified, it defaults to the end of the string.


<br><br>
### In-Language Definitions

#### `stringFromTokenList(tokens) <> string`

Takes a list of tokenizer-style character tokens (that is, derived values
whose type tags are each a single-character string), returning the result
of concatenating all the characters together in order.

This function is intended to aid in the building of tokenizers.
