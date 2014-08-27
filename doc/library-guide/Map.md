Samizdat Layer 0: Core Library
==============================

Map
---

A `Map` is a kind of `Collection`. It represents a set of mappings from
keys to values, where the keys are ordered by the total order of values
as defined by the global function `order()`.

<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> map | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> map | void`

Compares two maps. Two maps are equal if they have equal sets of mappings.

#### `.totalOrder(other) -> int`

Compares two maps for order. Maps order primarily by ordered lists of
keys, with the same rules as list comparison. Given two maps with equal
key lists, ordering is by comparing corresponding lists of values, in
key order.

<br><br>
### Method Definitions: One-Offs

#### `.get_key() -> .`

Returns the sole key of the given map, which must be a single-mapping map.
It is a terminal error if `this` does not contain exactly one mapping.

#### `.get_value() -> .`

Returns the sole value of the given map, which must be a single-mapping map.
It is a terminal error if `this` does not contain exactly one mapping.


<br><br>
### Method Definitions: `Collection` protocol

#### `.cat(more*) -> map`

Returns a map consisting of the combination of the mappings of the
argument maps. For any keys in common between the maps,
the lastmost argument's value is the one that ends up in the result.
Despite the `cat` name, strictly speaking this isn't a linear concatenation,
but it is as close as one can get to it given the class's key ordering
and uniqueness constraints.

**Syntax Note:** Used in the translation of `{key: value, ...}`
and `switch` forms.

#### `.del(key) -> map`

Returns a map just like the one given as an argument, except that
the result does not have a mapping for the given `key`. If `map` does
not bind `key`, then this returns `map`.

#### `.get(key) -> . | void`

Returns the value mapped to the given key (an arbitrary value) in
the given map. If there is no such mapping, then this returns void.

#### `.get_size() -> int`

Returns the number of mappings in the map.

#### `.keyList() -> list`

Returns a list of all the keys mapped by the given `map`, in sorted order.

#### `.nthMapping(n) -> map | void`

Gets the nth mapping of the given map.

#### `.put(key, value) -> map`

Returns a map just like the given one, except with a new mapping
for `key` to `value`. The result has a replacement for the existing
mapping for `key` in `map` if such a one existed, or has an
additional mapping in cases where `map` didn't already bind `key`.

#### `.valueList() -> list`

Returns a list of all the values mapped by the given `map`, in order of the
sorted keys.


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> list`

Collects or filters the mappings of `this`.

#### `.fetch() -> map | void`

Returns void on an empty map. Returns `this` on a single-element map.
Terminates with an error in all other cases.

#### `.nextValue(box) -> generator | void`

On a non-empty map, calls `box.store(mapping)` where `mapping` is
the first mapping in the map in its iteration order, and returns
a map of the remaining mappings. On an empty map, this just returns void.


<br><br>
### Primitive Definitions

#### `makeValueMap(keys*, value) -> map`

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
