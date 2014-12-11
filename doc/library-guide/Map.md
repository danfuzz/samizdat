Samizdat Layer 0: Core Library
==============================

Map
---

A `Map` is a kind of `Collection`. It represents a set of mappings from
keys to values, where the keys are ordered by the total order of values
as defined by the class method `Cmp.order()`.

<br><br>
### Class Method Definitions

#### `class.castFrom(value) -> isa Map | void`

This class knows how to cast as follows:

* `Core` &mdash; Returns `value`.

* `Map` &mdash; Returns `value`.

* `Record` &mdash; Returns the data payload of `value`. **Note:** This
  cast exists so that it is possible to do interpolation of records in when
  constructing maps.

* `SymbolTable` &mdash; Returns a map with the same bindings as `value`.
  Only works if `value`s keys are all ordered. See `Symbol` documentation
  for restrictions on symbol ordering.

* `Value` &mdash; Returns `value`.

#### `class.new(args*) -> isa Map`

This makes a map from a series of mappings, given as pairs of
key-then-value arguments. For example:

```
{a: 10}           is equivalent to Map.new(@a, 10, @b, 20)
{a: 10, "x": 20}  is equivalent to Map.new(@a, 10, "x", 20)
[etc.]
```

It is a fatal error (terminating the runtime) to pass an odd number of
arguments to this function.

**Syntax Note:** Used in the translation of `{key: value, ...}`
and `switch` forms.

#### `class.singleValue(keys*, value) -> isa Map`

This makes a map which maps any number of keys (including none)
to the same value. If no keys are specified, then this function returns
the empty map. For example:

```
v = {(k1): v};      is equivalent to  v = Map.singleValue(k1, v);
v = {[k1, k2]*: v}; is equivalent to  v = Map.singleValue(k1, k2, v);
[etc.]
```

Note that the argument list is "stretchy" in front, which isn't
representable in real Samizdat syntax.

**Syntax Note:** Used in the translation of `{key: value, ...}`
and `switch` forms.


<br><br>
### Method Definitions: `Value` protocol

#### `.castToward(cls) -> . | void`

This class knows how to cast as follows:

* `Core` &mdash; Returns `this`.

* `Map` &mdash; Returns `this`.

* `SymbolTable` &mdash; Returns a symbol table with the same mappings as
  `this`. Only works on maps where all the keys are symbols.

* `Value` &mdash; Returns `this`.

#### `.crossEq(other) -> logic`

Compares two maps. Two maps are equal if they have equal sets of mappings.

#### `.crossOrder(other) -> isa Symbol`

Compares two maps for order. Maps order primarily by ordered lists of
keys, with the same rules as list comparison. Given two maps with equal
key lists, ordering is by comparing corresponding lists of values, in
key order.

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol`

Default implementation.


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

#### `.cat(more*) -> isa Map`

Returns a map consisting of the combination of the mappings of `ths` and the
arguments. Arguments are allowed to be maps or anything which can be cast
to a map, including notably records and symbol tables.

For any keys in common between the arguments, the lastmost argument's value
is the one that ends up in the result. Despite the `cat` name, strictly
speaking this isn't a linear concatenation, but it is as close as one can
get to it given the class's key ordering and uniqueness constraints.

**Syntax Note:** Used in the translation of `{key: value, ...}`
and `switch` forms.

#### `.del(keys*) -> isa Map`

Returns a map just like the given one, except that
the mappings for the given `keys`, if any, are removed. If `this`
does not bind any of the given keys, then this returns `this`.

#### `.get(key) -> . | void`

Returns the value mapped to the given key (an arbitrary value) in
the given map. If there is no such mapping, then this returns void.

#### `.get_size() -> isa Int`

Returns the number of mappings in the map.

#### `.keyList() -> isa List`

Returns a list of all the keys mapped by the given `map`, in sorted order.

#### `.valueList() -> isa List`

Returns a list of all the values mapped by the given `map`, in order of the
sorted keys.


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> isa List`

Collects or filters the mappings of `this`.

#### `.fetch() -> isa Map | void`

Returns void on an empty map. Returns `this` on a single-element map.
Terminates with an error in all other cases.

#### `.nextValue(box) -> isa Map | void`

On a non-empty map, calls `box.store(mapping)` where `mapping` is
the first mapping in the map in its iteration order, and returns
a map of the remaining mappings. On an empty map, this just returns void.
