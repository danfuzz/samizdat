Samizdat Layer 0: Core Library
==============================

Strings
-------

<br><br>
### Primitive Definitions

#### `intFromString string <> integer`

Given a single-character string, returns the character code
of its sole character, as an integer. It is an error (terminating
the runtime) if `string` is not a string of size 1.

#### `stringAdd strings* <> string`

Returns a string consisting of the concatenation of the contents
of all the argument strings, in argument order.

#### `stringFromInt integer <> string`

Returns a single-character string that consists of the character
code indicated by the given integer argument, which must be in the
range for representation as an unsigned 32-bit quantity.

#### `stringNth string n notFound? <> . | ~.`

Returns the `n`th (zero-based) element of the given string, as a
single-element string, if `n` is a valid integer index into the given
string. If `n` is not valid (not an integer, or out of range),
then this returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.


<br><br>
### In-Language Definitions

#### `stringForEach string function <> ~.`

Calls the given function on each element (character) of the given string.
The given function is called with two arguments, namely the element's
index (zero-based) and the element's value (as a single-character string).
Always returns void.

#### `stringMap string function <> list`

Maps each element of a string using a mapping function, collecting
the results into a list (note, not into a string). The given
function is called on each element (character), with two arguments,
namely the element's index (zero-based) and the element's value
(as a single-character string).

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

**Note:** Unlike many other languages with similar functions, the
function argument is the *last* one and not the *first* one. This is
specifically done to make it natural to write a multi-line function
without losing track of the other two arguments.

#### `stringReduce base string function <> . | ~.`

Reduces a string to a single value, given a base value and a
reducer function, operating in low-to-high index order (that is, this
is a left-reduce operation). The given function is called once per
string element (character), with three arguments: the last (or base)
reduction result, the element's index (zero-based), and the element's
value (as a single-character string). The function result becomes the
reduction result, which is passed to the next call of `function` or becomes
the return value of the call to this function if it was the call for the
final element.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringMap` about choice of argument order.
