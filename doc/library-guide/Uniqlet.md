Samizdat Layer 0: Core Library
==============================

Uniqlet
-------

<br><br>
### Method Definitions: `Value` protocol

#### `perEq(uniqlet, other) -> uniqlet | void`

Default implementation.

#### `perOrder(uniqlet, other) -> int`

Default implementation.

#### `totalEq(uniqlet1, uniqlet2) -> uniqlet | void`

Performs an identity comparison. No two uniqlets are alike.

#### `totalOrder(uniqlet1, uniqlet2) -> int`

Performs an identity comparison. No two uniqlets are alike, and two
different uniqlets have no defined order.


<br><br>
### Primitive Definitions

#### `makeUniqlet() -> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist).


<br><br>
### In-Language Definitions

(none)
