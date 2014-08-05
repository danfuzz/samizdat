Samizdat Layer 0: Core Library
==============================

Class
-----

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(cls, other) -> class | void`

Default implementation.

#### `perOrder(cls, other) -> int`

Default implementation.

#### `totalEq(cls1, cls2) -> map | void`

Compares two classes. Two different classes are never equal.

#### `totalOrder(cls1, cls2) -> int | void`

Compares two classes for order, as follows:

* Core (primitive) classes order earlier than all other classes.
* Derived data classes order earlier than everything but core classes.
* Opaque derived classes order after all other classes.

Within each category, classes are ordered by name. Given two different
opaque classes with the same name (which can happen only with opaque
derived classes), the result is void (that is, the order undefined).

This last bit means it is okay to use any mix of core, derived data, and
opaque derived classes as the keys in a map, *except* that it is invalid
to use two different opaque derived classes that have the same name. This
restriction is in place because there is no consistent and stable way
to sort such classes.


<br><br>
### Primitive Definitions

#### `className(cls) -> . | void`

Returns the name of the class. This is an arbitrary value associated with
a class, which is typically (but not necessarily) a string.

#### `classParent(cls) -> class | void`

Returns the parent class (that is, the superclass) of the given class. This
returns a class for all classes except `Value`. For `Value`, this returns
void.


<br><br>
### In-Language Definitions

(none)
