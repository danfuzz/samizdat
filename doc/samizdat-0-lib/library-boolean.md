Samizdat Layer 0: Core Library
==============================

Booleans And Comparison
-----------------------

<br><br>
### Primitive Definitions

#### `lowOrder(value1, value2) <> int`

Returns the order of the two given values in the total order of
Samizdat low-layer values. This returns one of `-1 0 1` indicating
how the two values sort with each other, using the reasonably
standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Ordering is calculated as follows:

The major order is by type &mdash; `int < string < list <
map < token < uniqlet` &mdash; and minor order is type-dependant.

* Ints order by integer value, low to high.

* Lists and strings order by pairwise corresponding-element
  comparison, with a strict prefix always ordering before its
  longer brethren.

* Maps order by first comparing corresponding ordered lists
  of keys with the same rules as list comparison. If the key
  lists are identical, then the result is the comparison of
  corresponding lists of values, in key order.

* Tokens compare by type as primary, and value as secondary.
  With types equal, tokens without a value order earlier than
  ones with a value.

* Any given uniqlet never compares as identical to anything but
  itself. There is a total ordering of uniqlets, which is consistent,
  transitive, and symmetric &mdash; but otherwise arbitrary.

#### `lowOrderIs(value1, value2, check1, check2?) <> logic`

The two values are compared as with `lowOrder`. The int
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

#### `booleanFromLogic(logic?) <> boolean`

TODO: Document.

#### `eq(value1, value2) <> logic`

Checks for equality. Returns `value2` if the two given values are
identical. Otherwise returns void.

#### `le(value1, value2) <> logic`

Checks for a less-than-or-equal relationship. Returns `value2` if the
first value orders before the second or is identical to it.
Otherwise returns void.

#### `logicFromBoolean(boolean) <> logic`

TODO: Document.

#### `lt(value1, value2) <> logic`

Checks for a less-than relationship. Returns `value2` if the first value
orders before the second. Otherwise returns void.

#### `ge(value1, value2) <> logic`

Checks for a greater-than-or-equal relationship. Returns `value2` if the
first value orders after the second or is identical to it.
Otherwise returns void.

#### `gt(value1, value2) <> logic`

Checks for a greater-than relationship. Returns `value2` if the first value
orders after the second. Otherwise returns void.

#### `ne(value1, value2) <> logic`

Checks for inequality. Returns `value2` if the two given values are not
identical. Otherwise returns void.

#### `not(value) <> boolean`

Returns the opposite boolean value to the one given. If given `true`,
this returns `false`. If given `false`, this returns `true`. It is
an error (terminating the runtime) to pass any other value.
