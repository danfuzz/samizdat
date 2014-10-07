Samizdat Layer 0: Core Library
==============================

Class
-----

<br><br>
### Method Definitions: `Class` protocol

#### `.get_name() -> symbol`

Returns the name of the class, as a symbol.

**Note:** It is possible for two different classes to have the same name,
so `cls1.get_name() == cls2.get_name()` does *not* imply that `cls1 == cls2`.

#### `.get_parent() -> class | void`

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

* Core (primitive) classes order earlier than other classes.
* Within each category (core or other), classes are ordered by name.
* Given two different classes with the same name (which can happen only with
  non-core classes), the result of ordering is void (that is, the order
  undefined).

This last bit means it is okay to use any mix of classes as the keys in a map,
*except* that it is invalid to use two different non-core classes that have
the same name. This restriction is in place because there is no consistent and
stable way to order such classes.

**Note:** This method goes beyond the usual contract for `.totalEq()` in that
it will accept any two values, even when their direct (concrete) classes
differ. This is done as a way to make the fact that regular classes each have
a unique direct metaclass easy to ignore in most code, since most of the time
that arrangement either doesn't matter or would cause trouble if not ignored.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
