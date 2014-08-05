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

#### `totalOrder(cls1, cls2) -> int`

Compares two classes for order, by name. No other aspects of the classes
are considered. This means, for example, you can have one class as a key
in a map, and it will be found if you search for a different class that
happens to have the same name.


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
