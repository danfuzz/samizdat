Samizdat Layer 0: Core Library
==============================

Strings
-------

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

#### `stringAdd(strings*) <> string`

Returns a string consisting of the concatenation of the contents
of all the argument strings, in argument order.

#### `stringNth(string, n, notFound?) <> . | !.`

Returns the `n`th (zero-based) element of the given string, as a
single-element string, if `n` is a valid int index into the given
string. If `n` is not valid (not an int, or out of range),
then this returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.

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

Takes a list of tokenizer-style character tokens (that is, tokens whose
type tags are each a single-character string), returning the result
of concatenating all the characters together in order.

This function is intended to aid in the building of tokenizers.
