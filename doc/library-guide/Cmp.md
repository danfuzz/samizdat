Samizdat Layer 0: Core Library
==============================

Cmp
---

`Cmp` is a utility class (an uninstantiable repository for class methods),
which implements all the standard comparison methods.

Comparison methods come in two flavors, cross-class and per-class.

Cross-class comparison is meant to succeed and provide a useful result when
given any two values, each of which can be of any class. This can be used,
for example, for sorting heterogeneous values. When performing cross-class
comparison, values of different classes are never considered to be equal,
and ordering between such values is based on the ordering of their classes.
When performing cross-class comparison on values of the *same* class, the
class gets to decide how to compare; that said, the general aim is to provide
as total an ordering of values as possible, even where such an ordering
doesn't make sense for the per-class semantics. (For example, a floating
point NaN value should have a defined cross-class order.)

Per-class comparison is (as its name hopefully implies) defined by each
class, and when performed is an asymmetrical operation in that the first
argument of a comparison method is the one asked what to do. Beyond that,
the semantics of per-class comparison are only meant to be useful and
meaningful within the context of the given first argument. So, for example,
it is valid to have comparisons work with second arguments of a different
class (e.g. comparing an int and a float), and it preferable to have
sensible per-class semantics over a fully-defined order. It is also valid
for a per-class comparison to be asymmetric, when that makes sense.


<br><br>
### Class Method Definitions

#### `class.eq(value, other) -> logic`

Checks for equality, using the cross-class order. Returns `value` if the
two given values are identical. Otherwise returns void.

This works by first checking the classes of the two values. If they are
different, this returns void immediately. Otherwise, this calls
`value.crossEq(other)`. In the latter case, this method doesn't "trust" a
non-void return value of `.crossEq()` and always returns the given `value`
argument, per se, to represent logical-true.

**Syntax Note:** Used in the translation of `expression \== expression` forms.

#### `class.ge(value, other) -> logic`

Checks for a greater-than-or-equal relationship, using the cross-class order.
Returns `value` if the first value orders after the second or orders as
"same." Otherwise returns void.

This works by calling `class.order()` and returning a result based on its
determination.

**Syntax Note:** Used in the translation of `expression \>= expression` forms.

#### `class.gt(value, other) -> logic`

Checks for a greater-than relationship, using the cross-class order. Returns
`value` if the first value orders after the second. Otherwise returns void.

This works by calling `class.order()` and returning a result based on its
determination.

**Syntax Note:** Used in the translation of `expression \> expression` forms.

#### `class.le(value, other) -> logic`

Checks for a less-than-or-equal relationship, using the cross-class order.
Returns `value` if the first value orders before the second or orders as
"same." Otherwise returns void.

This works by calling `class.order()` and returning a result based on its
determination.

**Syntax Note:** Used in the translation of `expression \<= expression` forms.

#### `class.lt(value, other) -> logic`

Checks for a less-than relationship, using the cross-class order. Returns
`value` if the first value orders before the second. Otherwise returns void.

This works by calling `class.order()` and returning a result based on its
determination.

**Syntax Note:** Used in the translation of `expression \< expression` forms.

#### `class.ne(value, other) -> logic`

Checks for inequality, using the cross-class order. Returns `value` if the two
given values are not identical. Otherwise returns void.

This is implemented similarly to `.eq()` (see which).

**Syntax Note:** Used in the translation of `expression \!= expression` forms.

#### `class.order(value, other) -> isa Symbol`

Returns the order of the two given values, using the cross-class order.

The return value is one of `@less` `@same` `@more` indicating how the two
values order with respect to each other.

This works by first checking the classes of the two values. If they are
different, this returns a value based on the per-class order of the classes.
Otherwise, this calls `value.crossOrder(other)` to determine the result.

**Note:** This is the method which underlies the implementation
of all cross-class ordering methods.

#### `class.perEq(value, other) -> logic`

Per-class comparison, which calls through to `value.perEq(other)` to
determine the result. Returns `value` if it is considered equal to `other`.

The main difference between this method and a straight call to `.perEq()` is
that this method doesn't "trust" a non-void return value of `.perEq()` and
always returns the given `value` argument, per se, to represent logical-true.

**Syntax Note:** Used in the translation of `expression == expression` forms.

#### `class.perGe(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to determine the
result. Returns `value` if it is considered greater than or equal to `other`.

**Syntax Note:** Used in the translation of `expression >= expression` forms.

#### `class.perGt(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to determine the
result. Returns `value` if it is considered greater than `other`.

**Syntax Note:** Used in the translation of `expression > expression` forms.

#### `class.perLe(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to determine the
result. Returns `value` if it is considered less than or equal to `other`.

**Syntax Note:** Used in the translation of `expression <= expression` forms.

#### `class.perLt(value, other) -> logic`

Per-class comparison, which calls `value.perOrder(other)` to determine the
result. Returns `value` if it is considered less than `other`.

**Syntax Note:** Used in the translation of `expression < expression` forms.

#### `class.perNe(value, other) -> logic`

Per-class comparison, which calls `value.perEq(other)` to determine the
result. Returns `value` if it is *not* considered equal to `other`.

**Syntax Note:** Used in the translation of `expression != expression` forms.
