Samizdat Layer 0: Core Library
==============================

Object
------

The class `Object` provides data and behavior encapsulation. Encapsulation
is achieved by using secret symbols (generally unlisted symbols), which
become bound as both class and instance methods:

* As a class method, the `new` secret is used to instantiate an object, taking
  a single optional data payload argument, which must be a symbol table if
  present. If not passed, the data of the constructed instance is `@{}` (the
  empty symbol table).

* As an instance method, the `access` secret is used to get the data payload
  of an object. It takes one optional argument.


<br><br>
### Class Method Definitions

#### `class.subclass(name, config, classMethods?, instanceMethods?) -> isa Class`

Makes a new object class with the given `name` and `secrets`. `name` must
be a symbol. `config` must be a symbol table that maps any of `access` and
`new`.

The two `*Methods` arguments, if present, must be symbol tables that map
method names to functions. If either is not passed, it is equivalent to
passing `@{}` (the empty symbol table).

**Note:** If you call this function twice with identical arguments, the
results will be two *different* classes with the same `name`.


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> logic`

Compares two objects. This is true if both have the same payload.

**Note:** As per the usual contract for this method, it is invalid to call
this method on two values of different classes.

#### `.crossOrder(other) -> isa Symbol`

Compares two objects for order. This returns the result of ordering the two
objects' respective data payloads.

**Note:** As per the usual contract for this method, it is invalid to call
this method on two values of different classes.
