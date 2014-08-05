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

#### `totalOrder(map1, map2) -> int`

Compares two classes for order, as follows:

* Core (primitive) classes order earlier than all other classes.
* Derived data classes order earlier than everything but core classes.
* Opaque derived classes order after all other classes.

Within each category, classes are ordered by name. For two different classes
with the same category and name (which can happen only with opaque derived
classes), the order is arbitrary but consistent.


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
