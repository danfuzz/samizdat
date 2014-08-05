Samizdat Layer 0: Core Library
==============================

Object
------


<br><br>
### Generic Function Definitions: `Object` protocol

#### `objectDataOf(obj, secret) -> . | void`

Returns the data payload of the given object, if any, but only if `obj`'s
secret matches the given one. If the value has no data payload, this returns
void.

It is an error (terminating the runtime) to call this function with a `secret`
that doesn't match.


<br><br>
### Primitive Definitions

#### `makeObject(cls, secret, value?) -> object`

Returns an object with the given class (a value of class `Class`)
and optional data payload value (an arbitrary value).

It is a fatal error (terminating the runtime) to pass for `cls` something
other than an object class, or to pass a `secret` that doesn't match the
secret of the given `cls`.

#### `makeObjectClass(name, secret) -> class`

Makes a new object class with the given `name` and `secret`. `name` must
be a string.

**Note:** If you call this function twice with identical arguments, the
results will be two *different* classes with the same `name` and `secret`.


<br><br>
### In-Language Definitions

(none)
