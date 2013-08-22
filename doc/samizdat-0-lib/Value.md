Samizdat Layer 0: Core Library
==============================

Values (the base type)
----------------------

<br><br>
### Generic Function Definitions: `Value` protocol (applies to all values)

#### `perEq(value1, value2) <> . | void`

Performs a "per-type" (type-specific) equality comparison of the two given
values. When called, the two values are guaranteed to be the same type.
If a client calls with different-typed values, it is a fatal error
(terminating the runtime).

The return value is either `value1` (or `value2` really) if the two values
are in fact identical, or `void` if they are not.

Each type specifies its own per-type equality check. See specific types for
details. Transparent derived types all compare for equality by comparing
both the payload value (if any). In addition, a default implementation
calls through to `perOrder` to determine sameness.

**Note:** In order for the system to operate consistently, `perEq` must
always behave consistently with `perOrder`, in that for a given pair of
values, `perEq` must indicate equality if and only if `perOrder` would return
`0`. `perEq` exists at all because it is often possible to determine
equality much quicker than determining order.

**Note:** This is the generic function which underlies the implementation
of all cross-type equality comparison functions.

#### `perOrder(value1, value2) <> int`

Returns the "per-type" (type-specific) order of the two given values.
When called, the two values are guaranteed to be the same type. If a client
calls with different-typed values, it is a fatal error (terminating the
runtime).

The return value is one of `-1 0 1` indicating how the two values sort with
each other, using the reasonably standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Each type specifies its own per-type ordering. See specific types for
details. Transparent derived types all order by performing ordering
on the respective payload values, with a lack of payload counting as
"before" any non-void payload. In addition, a default implementation
compares value identities; this will fail if the type of the values is
not an "identified" one.

**Note:** This is the generic function which underlies the implementation
of all cross-type ordering functions.


<br><br>
### Primitive Definitions

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

**Syntax Note:** Used in the translation of `@type` and `@[type: value]`
forms.

#### `isOpaqueValue(value) <> logic`

Returns `value` if it is an opaque value &mdash; that is, if its type has
an associated secret &mdash; or void if not.

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
