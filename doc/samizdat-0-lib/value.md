Samizdat Layer 0: Core Library
==============================

General Value Handling
----------------------

<br><br>
### Primitive Definitions

#### `call(function, args*) <> . | void`

Calls the given function with the given arguments. This function isn't
normally that useful, in that `call(x, y)` is the same as saying `x(y)`.
However, this function is in fact a generic function, and it is possible
to use various other functions to reflect on it.

#### `canCall(function, value) <> logic`

Returns `value` if it can be passed as the first argument to the given
`function`, or void if not. `function` can be either a regular or
generic function. In the case of a regular function, this merely checks
if the function accepts at least one argument. In the case of a generic,
this checks to see if value's type is bound.

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

#### `dataOf(value, secret?) <> .`

Returns the data payload of the given arbitrary value, if any.
For a type-only value, this returns void.

For transparent, if `secret` is passed, then this function returns void.

For opaque values (including most core values), the given `secret` must match
the value's associated secret (associated with the type). If the secret
does not match (including if it was not passed at all), then this function
returns void.

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
