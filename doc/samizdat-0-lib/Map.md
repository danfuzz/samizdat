Samizdat Layer 0: Core Library
==============================

Maps
----

<br><br>
### Primitive Definitions

#### `makeMapping(keys*, value) <> map`

This makes a map which maps any number of keys (including none)
to the same value. If no keys are specified, then this function returns
the empty map. For example:

```
v = [(k1): v];      is equivalent to  v = makeMapping(k1, v);
v = [[k1, k2]*: v;  is equivalent to  v = makeMapping(k1, k2, v);
[etc.]
```

Note that the argument list is "stretchy" in front, which isn't really
representable in Samizdat syntax as presented.

**Syntax Note:** Used in the translation of `[key: value, ...]`
and `switch` forms.

#### `mapAdd(maps*) <> map`

Returns a map consisting of the combination of the mappings of the
argument maps. For any keys in common between the maps,
the lastmost argument's value is the one that ends up in the result.

**Syntax Note:** Used in the translation of `[key: value, ...]`
and `switch` forms.

#### `mapDel(map, keys*) <> map`

Returns a map just like the one given as an argument, except that
the result does not have mappings for the given keys. If the given
map does not have a particular given key, then that key has no
affect on the result.

#### `mapGet(map, key) <> . | void`

Returns the value mapped to the given key (an arbitrary value) in
the given map. If there is no such mapping, then this returns void.

#### `mapKeys(map) <> list`

Returns the list of keys contained in the given map, in sorted order.

#### `mapNth(map, n) <> . | void`

Returns the `n`th (zero-based) mapping of the given map, if `n` is
a valid int index into the map. Otherwise, if `n` is a non-negative int,
this returns void. Otherwise, this terminates the runtime with an error.

When given a valid index, the return value is a single-mapping map, which is
suitable as the argument to `mappingKey` and `mappingValue`.

The ordering of the mappings is by sort order of the keys.

#### `mapPut(map, key, value) <> map`

Returns a map just like the given one, except with a new mapping
for `key` to `value`. The result has a replacement for the existing
mapping for `key` in `map` if such a one existed, or has an
additional mapping in cases where `map` didn't already bind `key`.
These two scenarios can be easily differentiated by either noting a
change in size (or not) between original and result, or by explicitly
checking for the existence of `key` in the original.

#### `mappingKey(map) <> .`

Returns the sole key of the given map, which must be a single-mapping map.

#### `mappingValue(map) <> .`

Returns the sole value of the given map, which must be a single-mapping map.


<br><br>
### In-Language Definitions

(none)
