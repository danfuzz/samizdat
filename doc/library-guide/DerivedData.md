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

#### `makeData(cls, value?) -> .`

Returns a derived data value with the given class (a value of class `Class`)
and optional data payload value (which must be a symbol table if present).
If `value` is not specified, it defaults to `@{}` (the empty symbol table).
These equivalences hold for Samizdat source code:

```
@type(value)    is equivalent to  makeData(@@type, value)
@(type)(value)  is equivalent to  makeData(type, value)
```

It is a fatal error (terminating the runtime) to pass for `cls` something
other than a derived data class or for `value` to be something other than
a symbol table.

**Syntax Note:** Used in the translation of `@(type)(value)` and related forms.

#### `makeDerivedDataClass(name) -> class`

Returns a `Class` instance which represents a derived data class
with the given `name`. `name` is an arbitrary symbol. The following
equivalences hold for Samizdat source code:

```
@@name    is equivalent to  makeDerivedDataClass(@name)
@@(name)  is equivalent to  makeDerivedDataClass(name)
```

Two different calls to this function are guaranteed to return the same exact
class when given identical `name` arguments.

**Note:** This is used in the translation of `@@name` and `@@(name)`forms.


<br><br>
### In-Language Definitions

(none)
