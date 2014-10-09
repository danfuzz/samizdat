Samizdat Layer 0: Core Library
==============================

Class
-----

These are the instance methods on `Class`, which means that they &mdash;
along with the default methods on `Value` &mdash; are the default *class*
methods for all classes.

<br><br>
### Method Definitions: `Class` protocol

#### `.castFrom(value) -> . | void`

Returns an instance of `this` which is the casted version of the given
`value`. If `value` cannot be cast to `this` class, then this returns
void.

The default implementation of this method merely checks to see if `value` is
already of the class. If so, it returns `value`; if not, it returns `void`.

**Note:** This method is used by the global functions `cast()` and `optCast()`
as part of the more general casting mechanism.

#### `.get_name() -> symbol`

Returns the name of the class, as a symbol.

**Note:** It is possible for two different classes to have the same name,
so `cls1.get_name() == cls2.get_name()` does *not* imply that `cls1 == cls2`.

#### `.get_parent() -> class | void`

Returns the parent class (that is, the superclass) of the given class. This
returns a class for all classes except `Value`. For `Value`, this returns
void.

#### `.accepts(value) -> . | void`

Returns `value` if it is of `this` class (including being of a sublass
of `this`), or void if not.


<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> class | void`

Default implementation.

#### `.perOrder(other) -> int`

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

**Note:** This method intentionally differs from `.totalEq()` in that
it will provide an order for most pairs of classes in practice, even when
their direct (concrete) classes differ. This is done as a way to make the
fact that regular classes each have a unique direct metaclass easy to ignore
in most code, since most of the time that arrangement either doesn't matter
or would cause trouble if not ignored.

#### `.totalEq(other) -> class | void`

Compares two classes. Two different classes are never equal.

#### `.totalOrder(other) -> int | void`

This is identical to `.perEq()`, except it first asserts that `other` has the
same direct class as `this` (which in practice is only true of metaclasses).


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
