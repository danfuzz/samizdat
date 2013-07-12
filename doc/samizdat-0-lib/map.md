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

#### `mappingKey(map) <> .`

Returns the sole key of the given map, which must be a single-mapping map.

#### `mapNth(map, n, notFound?) <> . | !.`

Returns the `n`th (zero-based) mapping of the given map, if `n` is
a valid int index into the map. If `n` is not a valid index
(either an out-of-range int, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied. When given a valid index,
the return value is a single-mapping map, which is suitable as the
argument to `mappingKey` and `mappingValue`.

The ordering of the mappings is by sort order of the keys.

#### `mapPut(map, key, value) <> map`

Returns a map just like the given one, except with a new mapping
for `key` to `value`. The result has a replacement for the existing
mapping for `key` in `map` if such a one existed, or has an
additional mapping in cases where `map` didn't already bind `key`.
These two scenarios can be easily differentiated by either noting a
change in size (or not) between original and result, or by explicitly
checking for the existence of `key` in the original.

#### `mappingValue(map) <> .`

Returns the sole value of the given map, which must be a single-mapping map.


<br><br>
### In-Language Definitions

(none)
