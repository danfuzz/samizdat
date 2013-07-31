Samizdat Layer 0: Core Library
==============================

Ordering / Comparison
---------------------

<br><br>
### Primitive Definitions

#### `coreOrder(value1, value2) <> int`

Returns the type-specific order of the two given values. This is like
`totalOrder` except that this function will terminate the runtime with an
error if the two values are of different types. Values are different types
if either (a) one is a core value and the other is a derived value, or
(b) `typeOf` on the two values differ.

This function is intended only to be used as part of the core
library's message bindings.

#### `coreOrderIs(value1, value2, check1, check2?) <> logic`

Type-specific order check. This is like `totalOrderIs` except that
this function will terminate the runtime with an error if the two values
are of different types, with the same rules as for `coreOrder`.

This function is intended only to be used in the implementation of
`coreEq` and related in-language library functions, as part of
the core library's message bindings.

#### `totalOrder(value1, value2) <> int`

Returns the order of the two given values in the total order of
Samizdat values. This returns one of `-1 0 1` indicating
how the two values sort with each other, using the reasonably
standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Ordering is calculated as follows:

The "majorest" order is that core (primitive) values order before
derived values. Within each major category, major order is by
type name order (e.g. `"Int"` before `"String"`). Minor order is
type-dependant.

* Ints order by integer value, low to high.

* Lists and strings order by pairwise corresponding-element
  comparison, with a strict prefix always ordering before its
  longer brethren.

* Maps order by first comparing corresponding ordered lists
  of keys with the same rules as list comparison. If the key
  lists are identical, then the result is the comparison of
  corresponding lists of values, in key order.

* Derived values compare by type as primary (per above), and data payload
  as secondary. With types equal, derived values without a payload order
  earlier than ones with a payload.

* All other core values (uniqlets and functions) never compare as identical to
  anything but themselves. Within each type, there is a total ordering of
  values. The ordering is consistent, transitive, and symmetric;
  but otherwise arbitrary.

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

#### `coreEq(value1, value2) <> logic`

Comparison, requiring identical types. This is just like `eq`
(see which), except that if the two arguments are of different type, this
function terminates the runtime with an error. It is also a terminal error
if either argument is a derived value with the type of a core value.

#### `coreLe(value1, value2) <> logic`

Comparison, requiring identical types. This is just like `le`
(see which), except that if the two arguments are of different type, this
function terminates the runtime with an error. It is also a terminal error
if either argument is a derived value with the type of a core value.


#### `coreLt(value1, value2) <> logic`

Comparison, requiring identical types. This is just like `lt`
(see which), except that if the two arguments are of different type, this
function terminates the runtime with an error. It is also a terminal error
if either argument is a derived value with the type of a core value.

#### `coreGe(value1, value2) <> logic`

Comparison, requiring identical types. This is just like `ge`
(see which), except that if the two arguments are of different type, this
function terminates the runtime with an error. It is also a terminal error
if either argument is a derived value with the type of a core value.

#### `coreGt(value1, value2) <> logic`

Comparison, requiring identical types. This is just like `gt`
(see which), except that if the two arguments are of different type, this
function terminates the runtime with an error. It is also a terminal error
if either argument is a derived value with the type of a core value.

#### `coreNe(value1, value2) <> logic`

Comparison, requiring identical types. This is just like `ne`
(see which), except that if the two arguments are of different type, this
function terminates the runtime with an error. It is also a terminal error
if either argument is a derived value with the type of a core value.

#### `eq(value1, value2) <> logic`

Checks for equality, using the total order of values. Returns `value2` if the
two given values are identical. Otherwise returns void.

#### `le(value1, value2) <> logic`

Checks for a less-than-or-equal relationship, using the total order of values.
Returns `value2` if the first value orders before the second or is identical
to it. Otherwise returns void.

#### `lt(value1, value2) <> logic`

Checks for a less-than relationship, using the total order of values.
Returns `value2` if the first value orders before the second. Otherwise
returns void.

#### `ge(value1, value2) <> logic`

Checks for a greater-than-or-equal relationship, using the total order of
values. Returns `value2` if the first value orders after the second or is
identical to it. Otherwise returns void.

#### `gt(value1, value2) <> logic`

Checks for a greater-than relationship, using the total order of values.
Returns `value2` if the first value orders after the second. Otherwise
returns void.

#### `ne(value1, value2) <> logic`

Checks for inequality, using the total order of values. Returns `value2` if
the two given values are not identical. Otherwise returns void.
