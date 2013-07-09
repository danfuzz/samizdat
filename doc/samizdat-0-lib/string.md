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

#### `intFromString(string) <> int`

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

#### `stringForEach(string, function) <> !.`

Calls the given function on each element (character) of the given string.
The given function is called with two arguments, namely the element's
index (zero-based) and the element's value (as a single-character string).
Always returns void.

**Note:** See `stringMap` for discussion about choice of argument order.

#### `stringFromTokenList(tokens) <> string`

Takes a list of tokenizer-style character tokens (that is, tokens whose
type tags are each a single-character string), returning the result
of concatenating all the characters together in order.

This function is intended to aid in the building of tokenizers.

#### `stringMap(string, function) <> list`

Maps each element of a string using a mapping function, collecting
the results into a list (note, not into a string). The given
function is called on each element (character), with two arguments,
namely the element's index (zero-based) and the element's value
(as a single-character string).

Similar to `listMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

**Note:** Unlike many other languages with similar functions, the
function argument is the *last* one and not the *first* one. This is
specifically done to make it natural to write a multi-line function
without losing track of the other two arguments.

#### `stringReduce(base, string, function) <> . | !.`

Reduces a string to a single value, given a base value and a
reducer function, operating in low-to-high index order (that is, this
is a left-reduce operation). The given function is called once per
string element (character), with three arguments: the last (or base)
reduction result, the element's index (zero-based), and the element's
value (as a single-character string). The function result becomes the
reduction result, which is passed to the next call of `function` or becomes
the return value of the call to this function if it was the call for the
final element.

Similar to `listReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

**Note:** See `stringMap` for discussion about choice of argument order.
