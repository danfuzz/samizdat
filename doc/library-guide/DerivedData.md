Samizdat Layer 0: Core Library
==============================

DerivedData
-----------

<br><br>
### Method Definitions

#### `.dataOf() -> . | void`

Returns the data payload of the given derived data value, if any. If the value
has no data payload, this returns void.

It is an error (terminating the runtime) to call this function on something
other than a derived data value.

### Primitive Definitions

#### `makeDerivedDataClass(name) -> class`

Returns a `Class` instance which represents a derived data class
with the given `name`. `name` is an arbitrary selector. The following
equivalences hold for Samizdat source code:

```
@@name    is equivalent to  makeDerivedDataClass(@.name)
@@(name)  is equivalent to  makeDerivedDataClass(name)
```

Two different calls to this function are guaranteed to return the same exact
class when given identical `name` arguments.

**Note:** This is used in the translation of `@@name` and `@@(name)`forms.


<br><br>
### In-Language Definitions

(none)
