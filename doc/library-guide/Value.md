Samizdat Layer 0: Core Library
==============================

Value (the base type)
---------------------

<br><br>
### Generic Function Definitions: `Value` protocol (applies to all values)

#### `perEq(value, other) <> . | void`

Performs a per-type equality comparison of the two given values, using the
per-type order. This should return `value` if the two values are to be
considered "equal," return void if the two values are to be considered
"unequal," or fail terminally if the two values are considered "incomparable."

Each type can specify its own per-type equality check, and the two arguments
are notably *not* required to be of the same type. The default implementation
calls through to `totEq` (see which).

**Note:** This is the generic function which underlies the implementation
of all per-type equality comparison functions.

**Syntax Note:** Used in the translation of `expression == expression` forms.

#### `perOrder(value, other) <> int`

Performs an order comparison of the two given values, using the per-type
order. Return values are the same as with `totOrder` (see which). As
with `perEq`, the two values are not required to be of the same type, and
should two arguments be considered "incomparable" this function should
terminate the runtime with an error.

Each type can specify its own per-type ordering comparison.
The default implementation calls through to `totOrder` (see which).

**Note:** This is the generic function which underlies the implementation
of all per-type ordering functions.

#### `totEq(value, other) <> . | void`

Performs a type-specific equality comparison of the two given
values, using the "total value ordering" order. When called, the two values
are guaranteed to be the same type. If a client calls with different-typed
values, it is a fatal error (terminating the runtime).

The return value is either `value` (or `other` really) if the two values
are in fact identical, or `void` if they are not.

Each type specifies its own total-order equality check. See specific types for
details. Derived data types all compare their values for equality by comparing
the payload value (if any). In addition, a default implementation checks
directly for trivial sameness and calls through to `totOrder` for anything
nontrivial.

**Note:** In order for the system to operate consistently, `totEq` must
always behave consistently with `totOrder`, in that for a given pair of
values, `totEq` must indicate equality if and only if `totOrder` would return
`0`. `totEq` exists at all because it is often possible to determine
equality much quicker than determining order.

**Note:** This is the generic function which underlies the implementation
of all cross-type equality comparison functions.

#### `totOrder(value, other) <> int`

Returns the type-specific order of the two given values, using the "total
value ordering" order. When called, the two values are guaranteed to be the
same type. If a client calls with different-typed values, it is a fatal error
(terminating the runtime).

The return value is one of `-1 0 1` indicating how the two values sort with
each other, using the reasonably standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Each type specifies its own total-order ordering. See specific types for
details. Derived data types all order their values by performing ordering
on the respective payload values, with a lack of payload counting as
"before" any non-void payload. In addition, a default implementation
checks directly for trivial sameness and compares identity ordering
for anything nontrivial; this latter check will fail if the type of
the values is not a "selfish" one.

**Note:** This is the generic function which underlies the implementation
of all cross-type ordering functions.


<br><br>
### Primitive Definitions

#### `dataOf(value, secret?) <> .`

Returns the data payload of the given arbitrary value, if any.
For a type-only value, this returns void.

For derived data values, if `secret` is passed, then this function
returns void.

For opaque values (including most core values), the given `secret` must match
the value's associated secret (associated with the type). If the secret
does not match (including if it was not passed at all), then this function
returns void.

#### `eq(value, other) <> logic`

Checks for equality, using the total order of values. Returns `value` if the
two given values are identical. Otherwise returns void.

This works by first checking the types of the two values. If they are
different, this returns void immediately. Otherwise, this calls `totEq` on
the two arguments. In the latter case, this function doesn't "trust" a
non-void return value of `totEq` and always returns the given `value`
argument, per se, to represent logical-true.

**Syntax Note:** Used in the translation of `expression \== expression` forms.

#### `hasType(value, type) <> logic`

Returns `value` if it has type `type`. Otherwise returns void.

In order to "have the type," `value` must either be an instance of type
`type` per se, or be an instance of a subtype of `type`.

#### `makeValue(type, value?) <> .`

Returns a derived value with the given type (a value of type `Type`)
and optional data payload value (an arbitrary value). These
equivalences hold for Samizdat Layer 0 source code:

```
@(type)         is equivalent to  v = makeValue(makeDerivedDataType(type));
@(type)(value)  is equivalent to  v = makeValue(makeDerivedDataType(type), value);
```

**Syntax Note:** Used in the translation of `@(type)(value)`
(and related) forms.

#### `order(value, other) <> int`

Returns the order of the two given values, using the total order of values.

The return value is one of `-1 0 1` indicating how the two values sort with
each other, just like `perOrder` and `totOrder`.

This function works by calling `totOrder` on the types of the two arguments
if they are different, or by calling `totOrder` on the arguments themselves
if they both have the same type.

**Note:** This is the function which underlies the implementation
of all cross-type ordering functions.

#### `get_type(value) <> type`

Returns the type of the given arbitrary `value`. The return value is always
of type `Type`.


<br><br>
### In-Language Definitions

#### `ge(value, other) <> logic`

Checks for a greater-than-or-equal relationship, using the total order of
values. Returns `value` if the first value orders after the second or is
identical to it. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \>= expression` forms.

#### `gt(value, other) <> logic`

Checks for a greater-than relationship, using the total order of values.
Returns `value` if the first value orders after the second. Otherwise
returns void.

**Syntax Note:** Used in the translation of `expression \> expression` forms.

#### `le(value, other) <> logic`

Checks for a less-than-or-equal relationship, using the total order of values.
Returns `value` if the first value orders before the second or is identical
to it. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \<= expression` forms.

#### `lt(value, other) <> logic`

Checks for a less-than relationship, using the total order of values.
Returns `value` if the first value orders before the second. Otherwise
returns void.

**Syntax Note:** Used in the translation of `expression \< expression` forms.

#### `ne(value, other) <> logic`

Checks for inequality, using the total order of values. Returns `value` if
the two given values are not identical. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \!= expression` forms.

#### `perGe(value, other) <> logic`

Per-type comparison, which calls `perOrder(value, other)` to
determine result. Returns `value` if it is considered greater than or equal
to `other`.

**Syntax Note:** Used in the translation of `expression >= expression` forms.

#### `perGt(value, other) <> logic`

Per-type comparison, which calls `perOrder(value, other)` to
determine result. Returns `value` if it is considered greater than `other`.

**Syntax Note:** Used in the translation of `expression > expression` forms.

#### `perLe(value, other) <> logic`

Per-type comparison, which calls `perOrder(value, other)` to
determine result. Returns `value` if it is considered less than or equal
to `other`.

**Syntax Note:** Used in the translation of `expression <= expression` forms.

#### `perLt(value, other) <> logic`

Per-type comparison, which calls `perOrder(value, other)` to
determine result. Returns `value` if it is considered less than `other`.

**Syntax Note:** Used in the translation of `expression < expression` forms.

#### `perNe(value, other) <> logic`

Per-type comparison, which calls `perEq(value, other)` to
determine result. Returns `value` if it is *not* considered equal to `other`.

**Syntax Note:** Used in the translation of `expression != expression` forms.

#### `totGe(value, other) <> logic`

Type-specific total-order comparison, which calls `totOrder(value, other)` to
determine result. Returns `value` if it is considered greater than or equal
to `other`. It is a fatal error (terminating the runtime) if the two
arguments are of different types.

#### `totGt(value, other) <> logic`

Type-specific total-order comparison, which calls `totOrder(value, other)` to
determine result. Returns `value` if it is considered greater than `other`.
It is a fatal error (terminating the runtime) if the two arguments are of
different types.

#### `totLe(value, other) <> logic`

Type-specific total-order comparison, which calls `totOrder(value, other)` to
determine result. Returns `value` if it is considered less than or equal
to `other`. It is a fatal error (terminating the runtime) if the two
arguments are of different types.

#### `totLt(value, other) <> logic`

Type-specific total-order comparison, which calls `totOrder(value, other)` to
determine result. Returns `value` if it is considered less than `other`.
It is a fatal error (terminating the runtime) if the two arguments are of
different types.

#### `totNe(value, other) <> logic`

Type-specific total-order comparison, which calls `totEq(value, other)` to
determine result. Returns `value` if it is *not* considered equal to `other`.
It is a fatal error (terminating the runtime) if the two arguments are of
different types.

**Syntax Note:** Used in the translation of `expr**` forms.

#### `typeNameOf(value) <> .`

Returns the name of the given `value`'s type. This function is the equivalent
to `nameOf(get_type(value))`.
