Samizdat Layer 0: Core Library
==============================

Value (the base class)
----------------------

<br><br>
### Method Definitions: `Value` protocol (applies to all values)

#### `.castToward(cls) -> . | void`

Returns a value representing `this` cast either to the given `cls` or
to a value which `cls` is known (or expected) to be able to cast to itself
from. If `this` does not know how to cast to / toward `cls`, this returns
void.

The default implementation of this method merely checks to see if `this` is
already of class `cls`. If so, it returns `this`; if not, it returns `void`.

**Note:** This method is used by the class methods `Class.typeCast()` and
`Class.typeCast()` as part of the more general casting mechanism.

#### `.crossEq(other) -> logic`

Performs a class-specific equality comparison of the two given
values, using the "cross-class ordering" order. When called by the system,
the two values are guaranteed to have the same direct class; however, it is
possible to call this function directly, so implementations must check to see
if `other` has the same class as `this`. If a client calls with
different-class values, it is a fatal error (terminating the runtime).

The return value is either `this` (or `other` really) if the two values
are in fact identical, or `void` if they are not.

Each class specifies its own total-order equality check. See specific classes
for details. Records compare their values for equality by comparing payload
values.

**Note:** In order for the system to operate consistently, `.crossEq()` must
always behave consistently with `.crossOrder()`, in that for a given pair of
values, `.crossEq()` must indicate equality if and only if `.crossOrder()`
would return `@same`. `.crossEq()` exists at all because it is often possible
to determine equality much quicker than determining order.

**Note:** This is the method which underlies the implementation
of all cross-class equality comparison functions.

#### `.crossOrder(other) -> isa Symbol | void`

Returns the class-specific order of the two given values, using the
"cross-class ordering" order. When called by the system, the two values are
guaranteed to have the same direct class; however, it is possible to call this
function directly, so implementations must check to see if `other` has the
same class as `this`. If a client calls with different-class values, it is a
fatal error (terminating the runtime).

The return value is one of `@less`, `@same`, or `@more` indicating how the two
values order with respect to each other:

* `@less` &mdash; The first value orders before the second value.

* `@same` &mdash; The two values are identical in terms of ordering.

* `@more` &mdash; The first value orders after the second value.

If two values have no defined order, this returns void.

Each class specifies its own total-order ordering. See specific classes for
details.

The default implementation of this method uses `eq()` to check for sameness.
It returns `@same` if `eq()` returns non-void, or void if not.

**Note:** This is the method which underlies the implementation
of all cross-class ordering functions.

#### `.debugString() -> isa String`

Returns a string representation of the given value, meant to aid in debugging.
This is in contrast to the functions in `core.Format` which are meant to
help format values for more useful consumption.

The class `Value` binds this to a function which returns a string consisting
of the class name, value name (result of call to `.debugSymbol()`) if
non-void, and low-level identifier (e.g. a memory address) of the value,
all wrapped in `@<...>`. Various of the core classes override this to provide
more useful information.

As a convention, overriders are encouraged to return *either* a string that
is either truly or very nearly Samizdat syntax, *or* a string surrounded
by `@<...>`.

**Note:** In general, it is a bad idea to call this function for any
purpose other than temporary debugging code.

#### `.debugSymbol() -> isa Symbol | void`

Some values have an associated symbolic name, or an optional associated name.
This method provides access to that name. If non-void, the result of this
call is expected to be a symbol.

The class `Value` binds this to a function which always returns void.

**Note:** In general, it is a bad idea to call this function for any
purpose other than temporary debugging code.

#### `.perEq(other) -> logic`

Performs a per-class equality comparison of the two given values, using the
per-class order. This should return `this` if the two values are to be
considered "equal," return void if the two values are to be considered
"unequal," or fail terminally if the two values are considered "incomparable."

Each class can specify its own per-class equality check, and the two arguments
are notably *not* required to be of the same class. The default implementation
calls through to the global function `eq` (see which).

**Note:** This is the method which underlies the implementation
of all per-class equality comparison functions.

**Syntax Note:** Used in the translation of `expression == expression` forms.

#### `.perOrder(other) -> isa Symbol | void`

Performs an order comparison of the two given values, using the per-class
order. Return values are the same as with `crossOrder` (see which). As
with `perEq`, the two values are not required to be of the same class, and
should two arguments be considered "incomparable" this function should
terminate the runtime with an error.

Each class can specify its own per-class ordering comparison.
The default implementation calls through to the global function `order`
(see which).

**Note:** This is the method which underlies the implementation
of all per-class ordering functions.
