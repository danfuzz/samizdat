Samizdat Layer 0: Core Library
==============================

Stringlets
----------

<br><br>
### Primitive Definitions

#### `intletFromStringlet stringlet <> intlet`

Given a single-character stringlet, returns the character code
of its sole character, as an intlet. It is an error (terminating
the runtime) if `stringlet` is not a stringlet of size 1.

#### `stringletAdd stringlets* <> stringlet`

Returns a stringlet consisting of the concatenation of the contents
of all the argument stringlets, in argument order.

#### `stringletFromIntlet intlet <> stringlet`

Returns a single-character stringlet that consists of the character
code indicated by the given intlet argument, which must be in the
range for representation as an unsigned 32-bit quantity.

#### `stringletNth stringlet n notFound? <> . | ~.`

Returns the `n`th (zero-based) element of the given stringlet, as a
single-element stringlet, if `n` is a valid intlet index into the given
stringlet. If `n` is not valid (not an intlet, or out of range),
then this returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.


<br><br>
### In-Language Definitions

#### `stringletForEach stringlet function <> ~.`

Calls the given function on each element (character) of the given stringlet.
The given function is called with two arguments, namely the element (as a
single-character stringlet) and its index number (zero-based). Always returns
void.

#### `stringletMap stringlet function <> listlet`

Maps each element of a stringlet using a mapping function, collecting
the results into a listlet (note, not into a stringlet). The given
function is called on each element (character), with two arguments,
namely the element (as a single-character stringlet) and its index
number (zero-based).

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

**Note:** Unlike many other languages with similar functions, the
function argument is the *last* one and not the *first* one. This is
specifically done to make it natural to write a multi-line function
without losing track of the other two arguments.

#### `stringletReduce base stringlet function <> . | ~.`

Reduces a stringlet to a single value, given a base value and a
reducer function, operating in low-to-high index order (that is, this
is a left-reduce operation). The given function is called once per
stringlet element (character), with three arguments: the last (or base)
reduction result, the element (as a single-character stringlet), and its
index number (zero-based). The function result becomes the reduction
result, which is passed to the next call of `function` or becomes the
return value of the call to this function if it was the call for the
final element.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringletMap` about choice of argument order.
