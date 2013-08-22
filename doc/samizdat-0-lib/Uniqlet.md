Samizdat Layer 0: Core Library
==============================

Uniqlets
--------

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(uniqlet1, uniqlet2) <> uniqlet | void`

Performs an identity comparison. Two uniqlets are only equal if they are
truly the same box.

#### `perOrder(uniqlet1, uniqlet2) <> int`

Performs an identity comparison. Uniqlets have a consistent, transitive, and
symmetric &mdash; but arbitrary &mdash; total order.


<br><br>
### Primitive Definitions

#### `makeUniqlet() <> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist). This equivalence holds for *Samizdat Layer 0* source
code:

```
v = @@;  is equivalent to  v = makeUniqlet();
```


<br><br>
### In-Language Definitions

(none)
