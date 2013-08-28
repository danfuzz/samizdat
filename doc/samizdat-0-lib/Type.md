Samizdat Layer 0: Core Library
==============================

Types
----

<br><br>
### Generic Function Definitions: `Value` protocol

#### `totEq(type1, type2) <> map | void`

Compares two types. Two types are equal if they are the actual same type.

#### `totOrder(map1, map2) <> int`

Compares two types for order, as follows:

* Core (primitive) types order earlier than all other types.
* Transparent derived types order earlier than everything but core types.
* Opaque derived types order after all other types.

Within each category, types are ordered by name. For two different types
with the same category and name (which can happen only with opaque derived
types), the order is arbitrary but consistent.


<br><br>
### Primitive Definitions

#### `typeName(type) <> .`

Returns the name of the type. This is an arbitrary value associated with
a type, which is typically (but not necessarily) a string.


<br><br>
### In-Language Definitions

(none)
