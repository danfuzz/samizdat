Samizdat Layer 0: Core Library
==============================

Type
----

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(type, other) <> type | void`

Calls `totalEq`.

#### `perOrder(type, other) <> int`

Calls `totalOrder`.

#### `totalEq(type1, type2) <> map | void`

Compares two types. Two types are equal if they are the actual same type.

#### `totalOrder(map1, map2) <> int`

Compares two types for order, as follows:

* Core (primitive) types order earlier than all other types.
* Derived data types order earlier than everything but core types.
* Opaque derived types order after all other types.

Within each category, types are ordered by name. For two different types
with the same category and name (which can happen only with opaque derived
types), the order is arbitrary but consistent.


<br><br>
### Primitive Definitions

#### `makeDerivedDataType(name) <> type`

Returns a `Type` instance which represents a derived data type
with the given `name`. `name` can be an arbitrary value but is most
typically a string. The following equivalences hold for Samizdat
source code:

```
@@name    is equivalent to  makeDerivedDataType("name")
@@(name)  is equivalent to  makeDerivedDataType(name)
```

**Note:** This is used in the translation of `@@name` and `@@(name)`forms.

#### `typeName(type) <> . | void`

Returns the name of the type. This is an arbitrary value associated with
a type, which is typically (but not necessarily) a string.

#### `typeParent(type) <> type | void`

Returns the parent type (that is, the supertype) of the given type. This
returns a type for all types except `Value`.


<br><br>
### In-Language Definitions

(none)
