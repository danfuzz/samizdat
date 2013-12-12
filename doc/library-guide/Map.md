Samizdat Layer 0: Core Library
==============================

Map
---

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(map, other) <> map | void`

Calls `totEq`.

#### `perOrder(map, other) <> int`

Calls `totOrder`.

#### `totEq(map1, map2) <> map | void`

Compares two maps. Two maps are equal if they have equal sets of mappings.

#### `totOrder(map1, map2) <> int`

Compares two maps for order. Maps order primarily by ordered lists of
keys, with the same rules as list comparison. Given two maps with equal
key lists, ordering is by comparing corresponding lists of values, in
key order.

<br><br>
### Generic Function Definitions: One-Offs

#### `keyOf(map) <> .`

Returns the sole key of the given map, which must be a single-mapping map.
It is a terminal error if `map` does not contain exactly one mapping.

#### `valueOf(map) <> .`

Returns the sole value of the given map, which must be a single-mapping map.
It is a terminal error if `map` does not contain exactly one mapping.


<br><br>
### Generic Function Definitions: `Collection` protocol

#### `cat(map, more*) <> map`

Returns a map consisting of the combination of the mappings of the
argument maps. For any keys in common between the maps,
the lastmost argument's value is the one that ends up in the result.
Despite the `cat` name, strictly speaking this isn't a linear concatenation,
but it is as close as one can get to it given the type's key ordering
and uniqueness constraints.

**Syntax Note:** Used in the translation of `{key: value, ...}`
and `switch` forms.

#### `del(map, key) <> map`

Returns a map just like the one given as an argument, except that
the result does not have a mapping for the given `key`. If `map` does
not bind `key`, then this returns `map`.

#### `get(map, key) <> . | void`

Returns the value mapped to the given key (an arbitrary value) in
the given map. If there is no such mapping, then this returns void.

#### `keyList(map) <> list`

Returns a list of all the keys mapped by the given `map`, in sorted order.

#### `nth(map, n) <> . | void`

Returns the nth (zero-based) *mapping* of the given map.

When given a valid index, the return value is a single-mapping map, which is
suitable as the argument to `keyOf` and `valueOf`.

The ordering of the mappings is by sort order of the keys.

#### `put(map, key, value) <> map`

Returns a map just like the given one, except with a new mapping
for `key` to `value`. The result has a replacement for the existing
mapping for `key` in `map` if such a one existed, or has an
additional mapping in cases where `map` didn't already bind `key`.

#### `sizeOf(map) <> int`

Returns the number of mappings in the map.

#### `slice(map, start, end?) <> map`

Returns a slice of the given map. **Note:** `start` and `end` are int
indices, not map keys.


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(map) <> list`

Returns a list of all the individual mappings of `map`, in sorted order.

#### `filter(map, filterFunction) <> list`

Filters the mappings of `map` using `filterFunction`.

#### `nextValue(map, box) <> generator | void`

On a non-empty map, calls `store(box, first(map))` and returns
`butFirst(map)`. On an empty map, calls `store(box)` and returns void.


<br><br>
### Primitive Definitions

#### `makeValueMap(keys*, value) <> map`

This makes a map which maps any number of keys (including none)
to the same value. If no keys are specified, then this function returns
the empty map. For example:

```
v = {(k1): v};      is equivalent to  v = makeValueMap(k1, v);
v = {[k1, k2]*: v}; is equivalent to  v = makeValueMap(k1, k2, v);
[etc.]
```

Note that the argument list is "stretchy" in front, which isn't really
representable in Samizdat syntax as presented.

**Syntax Note:** Used in the translation of `{key: value, ...}`
and `switch` forms.


<br><br>
### In-Language Definitions

(none)
