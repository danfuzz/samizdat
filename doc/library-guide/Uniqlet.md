Samizdat Layer 0: Core Library
==============================

Uniqlet
-------

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(uniqlet, other) <> uniqlet | void`

Calls `totEq`.

#### `perOrder(uniqlet, other) <> int`

Calls `totOrder`.

#### `totEq(uniqlet1, uniqlet2) <> uniqlet | void`

Performs an identity comparison. Two uniqlets are only equal if they are
truly the same box.

#### `totOrder(uniqlet1, uniqlet2) <> int`

Performs an identity comparison. Uniqlets have a consistent, transitive, and
symmetric &mdash; but arbitrary &mdash; total order.


<br><br>
### Primitive Definitions

#### `makeUniqlet() <> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist).


<br><br>
### In-Language Definitions

(none)
