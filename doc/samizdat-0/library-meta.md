Samizdat Layer 0: Core Library
==============================

Meta-Library
------------

<br><br>
### Primitive Definitions

#### `LIBRARY`

A map of bindings of the entire *Samizdat Layer 0*
primitive library, except for the binding of `LIBRARY` itself (which can't
be done in that the data model doesn't allow self-reference).

**Note:** This is a constant map value, not a function.

**Note:** This binding as the *primitive* library is only available when
the in-language core library is first being loaded. When non-library code
is loaded, its `LIBRARY` binding is the full core library, including both
primitives and in-language definitions.


<br><br>
### In-Language Definitions

#### `LIBRARY`

A map of bindings of the entire *Samizdat Layer 0*
library, except for the binding of `LIBRARY` itself (which can't
be done in that the data model doesn't allow self-reference).

**Note:** This is a constant map value, not a function.

#### `makeLibrary map <> map`

Takes a library binding map and returns one that is just like the
one given, except that the key `"LIBRARY"` is bound to the given
map. This makes a `LIBRARY` binding into a form suitable for
passing as the library / global context argument to evaluation
functions (such as `sam0Eval`), in that callees can rightfully expect
there to be a binding for `LIBRARY`.
