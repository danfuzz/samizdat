Samizdat Layer 0: Core Library
==============================

Object
------


<br><br>
### Method Definitions: `Object` protocol

#### `.objectDataOf(secret) -> . | void`

Returns the data payload of the given object, if any, but only if `this`'s
secret matches the given one. If the value has no data payload, this returns
void.

It is an error (terminating the runtime) to call this function with a `secret`
that doesn't match.


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

Makes a new object class with the given `name` and `secret`. `name` must
be a symbol.

**Note:** If you call this function twice with identical arguments, the
results will be two *different* classes with the same `name` and `secret`.


<br><br>
### In-Language Definitions

(none)
