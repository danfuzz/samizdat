Samizdat Layer 0: Core Library
==============================

Class
-----

<br><br>
### Method Definitions: `Class` protocol

#### `.get_name() -> symbol`

Returns the name of the class, as a symbol.

#### `.get_parent() -> class | void`

Returns the parent class (that is, the superclass) of the given class. This
returns a class for all classes except `Value`. For `Value`, this returns
void.

#### `.hasName(name) -> class`

Returns `this` if its name is as given, or void if not. Bear in mind that
it is possible for two different classes to have the same name.


<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> class | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> class | void`

Compares two classes. Two different classes are never equal.

#### `.totalOrder(other) -> int | void`

Compares two classes for order, as follows:

* Core (primitive) classes order earlier than other classes.
* Within each category (core or other), classes are ordered by name.
* Given two different classes with the same name (which can happen only with
  non-core classes), the result of ordering is void (that is, the order
  undefined).

This last bit means it is okay to use any mix of classes as
the keys in a map, *except* that it is invalid to use two different non-core
classes that have the same name. This restriction is in place because there is
no consistent and stable way to order such classes.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
