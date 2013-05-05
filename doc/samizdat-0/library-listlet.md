Samizdat Layer 0: Core Library
==============================

Listlets
--------

<br><br>
### Primitive Definitions

#### `listletAdd listlets* <> listlet`

Returns a listlet consisting of the concatenation of the elements
of all the argument listlets, in argument order.

#### `listletDelNth listlet n <> listlet`

Returns a listlet just like the given one, except that the `n`th
(zero-based) element is deleted, if `n` is a valid intlet index into
the given listlet. If `n` is not a valid index (not an intlet, or with
a value out of range), then this returns the original listlet as the
result.

#### `listletInsNth listlet n value <> listlet`

Returns a listlet just like the given one, except that the `n`th
(zero-based) element is set to be the given value (an arbitrary
value), and all elements at or beyond index `n` in the original
are shifted up by one index.

`n` must be non-negative and less than or equal to the size of the
listlet. If not, it is an error (terminating the runtime).

#### `listletNth listlet n notFound? <> . | ~.`

Returns the `n`th (zero-based) element of the given listlet, if `n` is
a valid intlet index into the listlet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

#### `listletPutNth listlet n value <> listlet`

Returns a listlet just like the given one, except that the `n`th
(zero-based) element is set to be the given value (an arbitrary
value), replacing whatever was at that index in the original. If
`n` is the size of the original listlet, then this call behaves
exactly like `listletAppend`.

`n` must be non-negative and less than or equal to the size of the
listlet. If not, it is an error (terminating the runtime).


<br><br>
### In-Language Definitions


#### `listletAppend listlet value <> listlet`

Returns a listlet consisting of the elements of the given
listlet argument followed by the given additional value.

#### `listletCat listlets* <> listlet`

Concatenates zero or more listlets together into a single resulting
listlet.

#### `listletForEach listlet function <> ~.`

Calls the given function on each element of the given listlet.
The given function is called with two arguments, namely the element
and its index number (zero-based). Always returns void.

#### `listletMap listlet function <> listlet`

Maps each element of a listlet using a mapping function, collecting
the results into a new listlet. The given function is called on each
listlet element, with two arguments, namely the element and its index
number (zero-based).

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

See note on `stringletMap` about choice of argument order.

#### `listletPrepend value listlet <> listlet`

Returns a listlet consisting of the given first value followed by the
elements of the given listlet argument.

**Note:** The arguments are given in an order meant to reflect the
result (and not listlet-first).

#### `listletReduce base listlet function <> . | ~.`

Reduces a listlet to a single value, given a base value and a reducer
function, operating in low-to-high index order (that is, this is a
left-reduce operation). The given function is called on each listlet
element, with three arguments: the last (or base) reduction result,
the element, and its index number (zero-based). The function result
becomes the reduction result, which is passed to the next call of
`function` or becomes the return value of the call to this function if
it was the call for the final element.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringletMap` about choice of argument order.
