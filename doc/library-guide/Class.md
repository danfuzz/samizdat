Samizdat Layer 0: Core Library
==============================

Class
-----

<br><br>
### Method Definitions: `Class` protocol

#### `get_name() -> symbol`

Returns the name of the class, as a symbol.

#### `get_parent() -> class | void`

Returns the parent class (that is, the superclass) of the given class. This
returns a class for all classes except `Value`. For `Value`, this returns
void.


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

* Core (primitive) classes order earlier than all other classes.
* Record classes order earlier than everything but core classes.
* Object classes order after all other classes.

Within each category, classes are ordered by name. Given two different
opaque classes with the same name (which can happen only with object
classes), the result is void (that is, the order undefined).

This last bit means it is okay to use any mix of core, record, and
object classes as the keys in a map, *except* that it is invalid
to use two different object classes that have the same name. This
restriction is in place because there is no consistent and stable way
to sort such classes.


<br><br>
### Primitive Definitions

#### `classAddMethod(cls, symbol, function) -> .`

Adds a new method to the given class, binding `symbol` to `function`.

**Note:** This function is temporary scaffolding for use while transitioning
from generic functions to symbols. Ultimately, a class definition should
be made such that all its methods come in one fell atomic swoop rather than
being added one at a time with a function such as this one (which also, FWIW,
doesn't bother with the class's secret either).


<br><br>
### In-Language Definitions

(none)
