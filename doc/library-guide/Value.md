Samizdat Layer 0: Core Library
==============================

Value (the base class)
----------------------

<br><br>
### Method Definitions: `Value` protocol (applies to all values)

#### `.debugString() -> string`

Returns a string representation of the given value, meant to aid in debugging.
This is in contrast to the functions in `core.Format` which are meant to
help format values for more useful consumption.

The class `Value` binds this to a function which returns a string consisting
of the class name, value name (result of call to `.debugSymbol()`) if
non-void, and low-level identifier (e.g. a memory address) of the value.
Various of the core classes override this to provide more useful information.

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
the two values are guaranteed to have the same class; however, it is possible
to call this function directly, so implementations must check to see if
`other` has the same class as `this`. If a client calls with different-class
values, it is a fatal error (terminating the runtime).

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
guaranteed to have the same class; however, it is possible to call this
function directly, so implementations must check to see if `other` has the
same class as `this`. If a client calls with different-class values, it is a
fatal error (terminating the runtime).

The return value is one of `-1`, `0`, or `1` indicating how the two values
sort with each other, using the reasonably standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

If two values have no defined order, this returns void.

Each class specifies its own total-order ordering. See specific classes for
details. Records all order their values by performing ordering
on the respective payload values.

The default implementation of this method uses `eq()` to check for sameness.
It returns `0` if it sameness and returns void if not.

**Note:** This is the method which underlies the implementation
of all cross-class ordering functions.


<br><br>
### Primitive Definitions

#### `eq(value, other) -> logic`

Checks for equality, using the total order of values. Returns `value` if the
two given values are identical. Otherwise returns void.

This works by first checking the classes of the two values. If they are
different, this returns void immediately. Otherwise, this calls `totalEq` on
the two arguments. In the latter case, this function doesn't "trust" a
non-void return value of `totalEq` and always returns the given `value`
argument, per se, to represent logical-true.

**Syntax Note:** Used in the translation of `expression \== expression` forms.

#### `get_class(value) -> class`

Returns the class of the given arbitrary `value`. The return value is always
of class `Class`.

#### `hasClass(value, cls) -> logic`

Returns `value` if it has class `cls`. Otherwise returns void.

In order to "have the class," `value` must either be an instance of class
`cls` per se, or be an instance of a subclass of `cls`.

#### `order(value, other) -> int`

Returns the order of the two given values, using the total order of values.

The return value is one of `-1 0 1` indicating how the two values sort with
each other, just like `perOrder` and `totalOrder`.

This function works by calling `totalOrder` on the classes of the two arguments
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

#### `get_className(value) -> symbol | void`

Returns the name of the given `value`'s class. This function is the equivalent
to `className(get_class(value))`.

#### `get_classNameString(value) -> string | void`

Returns the name of the given `value`'s class, as a string. This function is
the equivalent to `classNameString(get_class(value))`.

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

**Syntax Note:** Used in the translation of `expr**` forms. This is used
instead of `ne`, because the latter wouldn't result in an error when not
passed a boolean value. This is used instead of `perNe`, as this is the
more primitive operation.
