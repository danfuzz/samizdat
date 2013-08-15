Samizdat Layer 0: Core Library
==============================

Values (the base type)
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

* derived values &mdash; `0` for a type-only derived value, or `1` for one
  with a data payload.

* anything else &mdash; always `0`.

#### `dataOf(value, secret?) <> .`

Returns the data payload of the given arbitrary value, if any.
For a type-only value, this returns void.

For transparent, if `secret` is passed, then this function returns void.

For opaque values (including most core values), the given `secret` must match
the value's associated secret (associated with the type). If the secret
does not match (including if it was not passed at all), then this function
returns void.

#### `makeValue(type, value?) <> .`

Returns a general value with the given type tag (an arbitrary value)
and optional data payload value (also an arbitrary value). These
equivalences hold for *Samizdat Layer 0* source code:

```
v = @[(type)];         is equivalent to  v = makeValue(type);
v = @[(type): value];  is equivalent to  v = makeValue(type, value);
```

If `type` names a core type, and the given `value` is a value
of that type, then this function returns `value` directly.

#### `isOpaqueValue(value) <> logic`

Returns `value` if it is an opaque value &mdash; that is, if its type has
an associated secret &mdash; or void if not.

#### `typeName(type) <> .`

Returns the name of the type. This is an arbitrary value associated with
a type, which is typically (but not necessarily) a string.

#### `typeOf(value) <> .`

Returns the type tag of the given arbitrary value. For transparent values,
this returns the type tag the value was constructed with, as a regular value.
For opaque values, this returns a value of type `Type`.

**Note:** It is invalid to construct a transparent value with a type tag
of type `Type`.


<br><br>
### In-Language Definitions

#### `isBox(value) <> logic`

Returns the given `value` if it is a box. Returns void if not.

#### `isFunction(value) <> logic`

Returns the given `value` if it is a (non-generic) function. Returns void
if not.

#### `isGeneric(value) <> logic`

Returns the given `value` if it is a generic function. Returns void if not.

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
