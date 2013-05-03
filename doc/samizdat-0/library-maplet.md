Samizdat Layer 0: Core Library
==============================

Maplets
-------

<br><br>
### Primitive Definitions

#### `mapletAdd maplet1 maplet2 <> maplet`

Returns a maplet consisting of the combination of the mappings of the
two argument maplets. For any keys in common between the two maplets,
the second argument's value is the one that ends up in the result.

#### `mapletDel maplet key <> maplet`

Returns a maplet just like the one given as an argument, except that
the result does not have a binding for the key `key`. If the given
maplet does not have `key` as a key, then this returns the given
maplet as the result.

#### `mapletGet maplet key notFound? <> . | ~.`

Returns the value mapped to the given key (an arbitrary value) in
the given maplet. If there is no such mapping, then this
returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.

#### `mapletHasKey maplet key <> boolean`

Returns `true` iff the given maplet has a mapping for the given key.

#### `mapletNth maplet n notFound? <> . | ~.`

Returns the `n`th (zero-based) mapping of the given maplet, if `n` is
a valid intlet index into the maplet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

The ordering of the mappings is by sort order of the keys.

#### `mapletNthKey maplet n notFound? <> . | ~.`

Returns the key of the `n`th (zero-based) mapping of the given maplet,
if `n` is a valid intlet index into the maplet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

The ordering of the mappings is by sort order of the keys.

#### `mapletNthValue maplet n notFound? <> . | ~.`

Returns the value of the `n`th (zero-based) mapping of the given maplet,
if `n` is a valid intlet index into the maplet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

The ordering of the mappings is by sort order of the keys.

#### `mapletPut maplet key value <> maplet`

Returns a maplet just like the given one, except with a new binding
for `key` to `value`. The result has a replacement for the existing
binding for `key` in `maplet` if such a one existed, or has an
additional binding in cases where `maplet` didn't already bind `key`.
These two scenarios can be easily differentiated by either noting a
change in size (or not) between original and result, or by explicitly
checking for the existence of `key` in the original.


<br><br>
### In-Language Definitions

#### `mapletCat maplets* <> maplet`

Concatenates zero or more maplets together into a single resulting
maplet. If there are any duplicate keys among the arguments, then
value associated with the last argument in which that key appears
is the value that "wins" in the final result.

#### `mapletForEach maplet function <> ~.`

Calls the given function for each mapping in the given maplet. The
function is called with two arguments, a value from the maplet and
its corresponding key (in that order). Always returns void.

#### `mapletMap maplet function <> maplet`

Maps the values of a maplet using the given mapping function,
resulting in a maplet whose keys are the same as the given maplet but
whose values may differ. In key order, the function is called with
two arguments representing the binding, a value and a key (in that
order, because it is common enough to want to ignore the key). The
return value of the function becomes the bound value for the given
key in the final result.

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

See note on `stringletMap` about choice of argument order.

#### `mapletReduce base maplet function <> . | ~.`

Reduces a maplet to a single value, given a base value and a reducer
function, operating in key order. The given function is called on each
maplet binding, with three arguments: the last (or base) reduction
result, the value associated with the binding, and its key. The
function result becomes the reduction result, which is passed to the
next call of `function` or becomes the return value of the call to
this function if it was the call for the final binding.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringletMap` about choice of argument order.
