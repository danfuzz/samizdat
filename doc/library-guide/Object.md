Samizdat Layer 0: Core Library
==============================

Object
------

The class `Object` provides data and behavior encapsulation. Encapsulation
is achieved by using a secret symbol (generally an unlisted symbol), which
is required as an argument to `makeObject()` and which becomes a bound
as a method to access a given class's private data.

<br><br>
### Primitive Definitions

#### `makeObject(cls, secret, value?) -> object`

Returns an object with the given class (a value of class `Class`)
and optional data payload value (which must be a symbol table).
If `value` is not specified, it defaults to `@{}` (the empty symbol table).

It is a fatal error (terminating the runtime) to pass for `cls` something
other than an object class, to pass a `secret` that doesn't match the
secret of the given `cls`, or to pass a `value` that is not a symbol table.

#### `makeObjectClass(name, secret) -> class`

Makes a new object class with the given `name` and `secret`. `name` and
`secret` must both a symbols. `secret` should generally be an unlisted
symbol, in order to have proper encapsulation.

**Note:** If you call this function twice with identical arguments, the
results will be two *different* classes with the same `name` and `secret`.


<br><br>
### In-Language Definitions

(none)
