Samizdat Layer 0: Core Library
==============================

Ordering / Comparison
---------------------

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

#### `totalOrder(value1, value2) <> int`

Returns the order of the two given values in the total order of
Samizdat values. This returns one of `-1 0 1` indicating
how the two values sort with each other, using the reasonably
standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Ordering is calculated as follows:

The "majorest" order is by type (see below for details). The minor order
is according to `perOrder` called on the arguments. See that generic
function for more details.

#### `totalOrderIs(value1, value2, check1, check2?) <> logic`

The two values are compared as with `totalOrder`. The int
result of that comparison are checked for equality with
the one or two check values. If the comparison result is equal
to either check value, this function returns `value2`. Otherwise
it returns void.

**Note:** This function exists in order to provide a primitive
comparison function that returns a logic result. Without it (or something
like it), there would be no way to define logic-semantics
comparators in-language.


<br><br>
### In-Language Definitions

#### `eq(value1, value2) <> logic`

Checks for equality, using the total order of values. Returns `value2` if the
two given values are identical. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \== expression` forms.

#### `le(value1, value2) <> logic`

Checks for a less-than-or-equal relationship, using the total order of values.
Returns `value2` if the first value orders before the second or is identical
to it. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \<= expression` forms.

#### `lt(value1, value2) <> logic`

Checks for a less-than relationship, using the total order of values.
Returns `value2` if the first value orders before the second. Otherwise
returns void.

**Syntax Note:** Used in the translation of `expression \< expression` forms.

#### `ge(value1, value2) <> logic`

Checks for a greater-than-or-equal relationship, using the total order of
values. Returns `value2` if the first value orders after the second or is
identical to it. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \>= expression` forms.

#### `gt(value1, value2) <> logic`

Checks for a greater-than relationship, using the total order of values.
Returns `value2` if the first value orders after the second. Otherwise
returns void.

**Syntax Note:** Used in the translation of `expression \> expression` forms.

#### `ne(value1, value2) <> logic`

Checks for inequality, using the total order of values. Returns `value2` if
the two given values are not identical. Otherwise returns void.

**Syntax Note:** Used in the translation of `expression \!= expression` forms.
