Samizdat Layer 0: Core Library
==============================

Uniqlets
--------

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
