Samizdat Layer 0: Core Library
==============================

Object
------

The class `Object` provides data and behavior encapsulation. Encapsulation
is achieved by using a secret symbol (generally an unlisted symbol), which
becomes bound as both a class and instance method:

* As a class method, it is used to instantiate an object, taking a single
  optional data payload argument, which must be a symbol table if present.
  If not passed, the data of the constructed instance is `@{}` (the empty
  symbol table).

* As an instance method, it takes no arguments and returns the data
  payload.


<br><br>
### Class Method Definitions

#### `subclass(name, secret, classMethods?, instanceMethods?) -> class`

Makes a new object class with the given `name` and `secret`. `name` and
`secret` must both a symbols. `secret` should generally be an unlisted
symbol, in order to have proper encapsulation.

The two `*Methods` arguments, if present, must be symbol tables that map
method names to functions. If either is not passed, it is equivalent to
passing `@{}` (the empty symbol table).

**Note:** If you call this function twice with identical arguments, the
results will be two *different* classes with the same `name`.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
