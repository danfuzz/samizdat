Samizdat Layer 0: Core Library
==============================

Maps
----

<br><br>
### Primitive Definitions

#### `mapAdd(maps*) <> map`

Returns a map consisting of the combination of the mappings of the
argument maps. For any keys in common between the maps,
the lastmost argument's value is the one that ends up in the result.

#### `mapDel(map, key) <> map`

Returns a map just like the one given as an argument, except that
the result does not have a mapping for the key `key`. If the given
map does not have `key` as a key, then this returns the given
map as the result.

#### `mapGet(map, key, notFound?) <> . | !.`

Returns the value mapped to the given key (an arbitrary value) in
the given map. If there is no such mapping, then this
returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.

#### `mapHasKey(map, key) <> boolean`

Returns `true` iff the given map has a mapping for the given key.

#### `mapKey(map) <> .`

Returns the sole key of the given map, which must be a single-mapping map.

#### `mapNth(map, n, notFound?) <> . | !.`

Returns the `n`th (zero-based) mapping of the given map, if `n` is
a valid int index into the map. If `n` is not a valid index
(either an out-of-range int, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied. When given a valid index,
the return value is a single-mapping map, which is suitable as the
argument to `mapKey` and `mapValue`.

The ordering of the mappings is by sort order of the keys.

#### `mapPut(map, key, value) <> map`

Returns a map just like the given one, except with a new mapping
for `key` to `value`. The result has a replacement for the existing
mapping for `key` in `map` if such a one existed, or has an
additional mapping in cases where `map` didn't already bind `key`.
These two scenarios can be easily differentiated by either noting a
change in size (or not) between original and result, or by explicitly
checking for the existence of `key` in the original.

#### `mapValue(map) <> .`

Returns the sole value of the given map, which must be a single-mapping map.


<br><br>
### In-Language Definitions

#### `mapForEach(map, function) <> !.`

Calls the given function for each mapping in the given map. The
function is called with two arguments, a key from the map and
its corresponding value (in that order). Always returns void.

**Note:** See `stringMap` for discussion about choice of argument order.

#### `mapMap(map, function) <> map`

Maps the values of a map using the given mapping function,
resulting in a map whose keys are the same as the given map but
whose values may differ. In key order, the function is called with
two arguments representing the mapping, a key and a value (in that
order). The return value of the function becomes the bound value for
the given key in the final result.

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

**Note:** See `stringMap` for discussion about choice of argument order.

#### `mapReduce(base, map, function) <> . | !.`

Reduces a map to a single value, given a base value and a reducer
function, operating in key order. The given function is called on each
mapping, with three arguments: the last (or base) reduction
result, a key from the map, and its associated value. The
function result becomes the reduction result, which is passed to the
next call of `function` or becomes the return value of the call to
this function if it was the call for the final mapping.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

**Note:** See `stringMap` for discussion about choice of argument order.
