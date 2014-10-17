Samizdat Layer 0: Core Library
==============================

Value (the base class)
----------------------

<br><br>
### Method Definitions: `Value` protocol (applies to all values)

#### `.castToward(cls) -> . | void`

Returns a value representing `this` cast either to the given `cls` or
to a value which `cls` is known (or expected to) be able to cast to itself
from. If `this` does not know how to cast to / toward `cls`, this returns
void.

The default implementation of this method merely checks to see if `this` is
already of class `cls`. If so, it returns `this`; if not, it returns `void`.

**Note:** This method is used by the global functions `cast()` and `optCast()`
as part of the more general casting mechanism.


#### `.debugString() -> string`

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

#### `.debugSymbol() -> symbol | void`

Some values have an associated symbolic name, or an optional associated name.
This method provides access to that name. If non-void, the result of this
call is expected to be a symbol.

The class `Value` binds this to a function which always returns void.

**Note:** In general, it is a bad idea to call this function for any
purpose other than temporary debugging code.

#### `.perEq(other) -> . | void`

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

#### `.perOrder(other) -> int | void`

Performs an order comparison of the two given values, using the per-class
order. Return values are the same as with `totalOrder` (see which). As
with `perEq`, the two values are not required to be of the same class, and
should two arguments be considered "incomparable" this function should
terminate the runtime with an error.

Each class can specify its own per-class ordering comparison.
The default implementation calls through to the global function `order`
(see which).

**Note:** This is the method which underlies the implementation
of all per-class ordering functions.

#### `.totalEq(other) -> . | void`

Performs a class-specific equality comparison of the two given
values, using the "total value ordering" order. When called by the system,
the two values are guaranteed to have the same direct class; however, it is
possible to call this function directly, so implementations must check to see
if `other` has the same class as `this`. If a client calls with
different-class values, it is a fatal error (terminating the runtime).

The return value is either `this` (or `other` really) if the two values
are in fact identical, or `void` if they are not.

Each class specifies its own total-order equality check. See specific classes
for details. Records compare their values for equality by comparing payload
values.

**Note:** In order for the system to operate consistently, `totalEq` must
always behave consistently with `totalOrder`, in that for a given pair of
values, `totalEq` must indicate equality if and only if `totalOrder` would
return `0`. `totalEq` exists at all because it is often possible to determine
equality much quicker than determining order.

**Note:** This is the method which underlies the implementation
of all cross-class equality comparison functions.

#### `.totalOrder(other) -> int | void`

Returns the class-specific order of the two given values, using the "total
value ordering" order. When called by the system, the two values are
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


<br><br>
### Primitive Definitions

#### `cast(cls, value) -> .`

"Hard" cast operation. This is like `optCast()`, except that this terminates
the runtime with an error if the cast could not complete, instead of returning
void.

#### `classOf(value) -> class`

Returns the class of the given arbitrary `value`. The return value is always
of class `Class`.

#### `eq(value, other) -> logic`

Checks for equality, using the total order of values. Returns `value` if the
two given values are identical. Otherwise returns void.

This works by first checking the classes of the two values. If they are
different, this returns void immediately. Otherwise, this calls `totalEq` on
the two arguments. In the latter case, this function doesn't "trust" a
non-void return value of `totalEq` and always returns the given `value`
argument, per se, to represent logical-true.

**Syntax Note:** Used in the translation of `expression \== expression` forms.

#### `maybeCast(cls, value) -> . | void`

"Soft" cast operation. This attempts to cast (convert in a maximally
data-preserving fashion) the given `value` to the indicated class `cls`.

This function operates by first checking to see if `value` is already of
a proper class, and returning it directly if so.

If not, this function calls `value.castToward(cls)` to give `value` "first
dibs" on conversion. If it results in a value of an appropriate class, then
that value is then returned.

If not, this function then calls `cls.castFrom(value)`, passing it the
non-void result of the previous step (if any) or the original value (if not).
If this call results in a value of an appropriate class, then that value is
then returned.

If not, this function returns void.

#### `order(value, other) -> int`

Returns the order of the two given values, using the total order of values.

The return value is one of `@less` `@same` `@more` indicating how the two
values order with respect to each other, just like `perOrder` and
`totalOrder`.

This function works by calling `perOrder` on the classes of the two arguments
if they are different, or by calling `totalOrder` on the arguments themselves
if they both have the same class.

**Note:** This is the function which underlies the implementation
of all cross-class ordering functions.


<br><br>
### In-Language Definitions

#### `ge(value, other) -> logic`

Checks for a greater-than-or-equal relationship, using the total order of
values. Returns `value` if the first value orders after the second or is
identical to it. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \>= expression` forms.

#### `gt(value, other) -> logic`

Checks for a greater-than relationship, using the total order of values.
Returns `value` if the first value orders after the second. Otherwise
returns void.

**Syntax Note:** Used in the translation of `expression \> expression` forms.

#### `le(value, other) -> logic`

Checks for a less-than-or-equal relationship, using the total order of values.
Returns `value` if the first value orders before the second or is identical
to it. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \<= expression` forms.

#### `lt(value, other) -> logic`

Checks for a less-than relationship, using the total order of values.
Returns `value` if the first value orders before the second. Otherwise
returns void.

**Syntax Note:** Used in the translation of `expression \< expression` forms.

#### `ne(value, other) -> logic`

Checks for inequality, using the total order of values. Returns `value` if
the two given values are not identical. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \!= expression` forms.

#### `perGe(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to
determine result. Returns `value` if it is considered greater than or equal
to `other`.

**Syntax Note:** Used in the translation of `expression >= expression` forms.

#### `perGt(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to
determine result. Returns `value` if it is considered greater than `other`.

**Syntax Note:** Used in the translation of `expression > expression` forms.

#### `perLe(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to
determine result. Returns `value` if it is considered less than or equal
to `other`.

**Syntax Note:** Used in the translation of `expression <= expression` forms.

#### `perLt(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to
determine result. Returns `value` if it is considered less than `other`.

**Syntax Note:** Used in the translation of `expression < expression` forms.

#### `perNe(value, other) -> logic`

Per-class comparison, which calls `value.perEq(other)` to
determine result. Returns `value` if it is *not* considered equal to `other`.

**Syntax Note:** Used in the translation of `expression != expression` forms.

#### `totalGe(value, other) -> logic`

Class-specific total-order comparison, which calls `value.totalOrder(other)` to
determine result. Returns `value` if it is considered greater than or equal
to `other`. It is a fatal error (terminating the runtime) if the two
arguments are of different classes.

#### `totalGt(value, other) -> logic`

Class-specific total-order comparison, which calls `value.totalOrder(other)` to
determine result. Returns `value` if it is considered greater than `other`.
It is a fatal error (terminating the runtime) if the two arguments are of
different classes.

#### `totalLe(value, other) -> logic`

Class-specific total-order comparison, which calls `value.totalOrder(other)` to
determine result. Returns `value` if it is considered less than or equal
to `other`. It is a fatal error (terminating the runtime) if the two
arguments are of different classes.

#### `totalLt(value, other) -> logic`

Class-specific total-order comparison, which calls `value.totalOrder(other)` to
determine result. Returns `value` if it is considered less than `other`.
It is a fatal error (terminating the runtime) if the two arguments are of
different classes.

#### `totalNe(value, other) -> logic`

Class-specific total-order comparison, which calls `value.totalEq(other)` to
determine result. Returns `value` if it is *not* considered equal to `other`.
It is a fatal error (terminating the runtime) if the two arguments are of
different classes.
