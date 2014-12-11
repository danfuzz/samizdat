Samizdat Layer 0: Core Library
==============================

Class
-----

These are the instance methods on `Class`, which means that they &mdash;
along with the default methods on `Value` &mdash; are the default *class*
methods for all classes.

The instance methods defined here are the instance methods on `Class`, which
means that they &mdash; along with the default methods on `Value` &mdash; are
the default *class* methods for all classes.

<br><br>
### Class Method Definitions

**Note:** These are the class methods on the class `Class`, and not class
methods in general.

#### `class.of(value) -> isa Class`

Returns the class of the given arbitrary `value`. The return value is always
of class `Class`.

#### `class.typeAccepts(cls, value) -> . | void`

Type compatibility check, with "soft" failure. This calls `cls.accepts(value)`,
returning `value` if that call returns non-void, or returning void otherwise.

**Note:** This is defined as a class method and not an instance method, so
that the overall behavior can be guaranteed by the system, including the
guarantee that a non-void return value is always the passed `value`.
Individual types are allowed to define `.accepts()` to add their particular
contribution to the behavior.

#### `class.typeCast(cls, value) -> . | void`

Type cast operation, with "soft" failure. This attempts to cast (convert in a
maximally data-preserving fashion) the given `value` to the indicated class
`cls`. If the cast can be performed, this returns the so-cast value. If not,
this returns void.

This function operates by first checking to see if `value` is already of
a matching class, and returning it directly if so.

If not, this function calls `value.castToward(cls)` to give `value` "first
dibs" on conversion. If it results in a value of an appropriate class, then
that value is then returned.

If not, this function then calls `cls.castFrom(value)`, passing it the
non-void result of the previous step (if any) or the original value (if not).
If this call results in a value of an appropriate class, then that value is
then returned.

If not, this function returns void.

**Note:** This is defined as a class method and not an instance method, so
that the overall behavior can be guaranteed by the system, including the
type guarantee on non-void return values. Individual classes are allowed to
define `.castToward()` and `class.castFrom()` to add their particular
contribution to the behavior.


<br><br>
### Method Definitions: `Class` protocol

#### `.castFrom(value) -> . | void`

Returns an instance of `this` which is the casted version of the given
`value`. If `value` cannot be cast to `this` class, then this returns
void.

The default implementation of this method merely checks to see if `value` is
already of the class. If so, it returns `value`; if not, it returns `void`.

**Note:** This method is used by the class methods `Class.typeCast()` and
`Class.typeCast()` as part of the more general casting mechanism.

#### `.get_name() -> isa Symbol`

Returns the name of the class, as a symbol.

**Note:** It is possible for two different classes to have the same name,
so `cls1.get_name() == cls2.get_name()` does *not* imply that `cls1 == cls2`.

#### `.get_parent() -> isa Class | void`

Returns the parent class (that is, the superclass) of the given class. This
returns a class for all classes except `Value`. For `Value`, this returns
void.

#### `.accepts(value) -> . | void`

Returns `value` if it is of `this` class (including being of a sublass
of `this`), or void if not.


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> logic`

Compares two classes. Two different classes are never equal.

#### `.crossOrder(other) -> isa Symbol | void`

This is identical to `.perEq()`, except it first asserts that `other` has the
same direct class as `this` (which in practice is only true of metaclasses).

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol`

Compares two classes for order, as follows:

* Core classes order earlier than other classes. The core classes are
  `Class`, `Core`, `Metaclass`, `Object`, `Value`, their metaclasses, and
  any class that inherits from either `Core` or `Core`'s metaclass.
* Within each category (core or other), classes are ordered by name.
* Given two different classes with the same name (which can happen only with
  non-core classes), the result of ordering is void (that is, the order
  undefined).

This last bit means it is okay to use any mix of classes as the keys in a map,
*except* that it is invalid to use two different non-core classes that have
the same name. This restriction is in place because there is no consistent and
stable way to order such classes.

**Note:** This method intentionally differs from `.crossEq()` in that
it will provide an order for most pairs of classes in practice, even when
their direct (concrete) classes differ. This is done as a way to make the
fact that regular classes each have a unique direct metaclass easy to ignore
in most code, since most of the time that arrangement either doesn't matter
or would cause trouble if not ignored.
