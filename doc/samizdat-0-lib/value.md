Samizdat Layer 0: Core Library
==============================

General Value Handling
----------------------

<br><br>
### Primitive Definitions

#### `coreSizeOf(value) <> int`

Returns the core (low level) "size" of the given value. Every value has
a size, defined as follows:

* `Int` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an int is 1, which
  is the size of both `0` and `-1`.

* `String` &mdash; the number of characters.

* `List` &mdash; the number of elements.

* `Map` &mdash; the number of mappings (bindings).

* `Uniqlet` &mdash; always `0`.

* derived values &mdash; `0` for a type-only derived value, or `1` for one
  with a data payload.

#### `dataOf(value) <> .`

Returns the data payload of the given arbitrary value, if any.
For a core value, this returns `value` itself. For a derived value, this
returns the data payload it was constructed with, if any. For a
type-only value, this returns void.

#### `isCoreValue(value) <> logic`

Returns `value` if it is a core (non-derived) value, or void if not.
This function could be defined as:

```
fn isCoreValue(value) {
    <> eq(value, &dataOf(value))
}
```

#### `typeOf(value) <> .`

Returns the type tag of the given arbitrary value. For a core value,
this returns its standard type name, particularly one of
`"Int"` `"List"` `"Map"` `"String"` or `"Uniqlet"`. For a derived
value, this returns the type tag that it was constructed with.


<br><br>
### In-Language Definitions

#### `isCoreValue(value) <> logic`

Returns the given `value` if it is a core (not derived) value.
Returns void if not.

#### `isInt(value) <> logic`

Returns the given `value` if it is an int. Returns void if not.

#### `isList(value) <> logic`

Returns the given `value` if it is a list. Returns void if not.

#### `isMap(value) <> logic`

Returns the given `value` if it is a map. Returns void if not.

#### `isString(value) <> logic`

Returns the given `value` if it is a string. Returns void if not.

#### `isUniqlet(value) <> logic`

Returns the given `value` if it is a uniqlet. Returns void if not.
