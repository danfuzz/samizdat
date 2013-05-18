Samizdat Layer 0: Core Library
==============================

Booleans And Comparison
-----------------------

<br><br>
### Primitive Definitions

#### `lowOrder value1 value2 <> int`

Returns the order of the two given values in the total order of
Samizdat low-layer values. This returns one of `-1 0 1` indicating
how the two values sort with each other, using the reasonably
standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Ordering is calculated as follows:

The major order is by type &mdash; `int < string < list <
map < uniqlet < token` &mdash; and minor order is type-dependant.

* Ints order by integer value.

* Lists and strings order by pairwise corresponding-element
  comparison, with a strict prefix always ordering before its
  longer brethren.

* Maps order by first comparing corresponding ordered lists
  of keys with the same rules as list comparison. If the key
  lists are identical, then the result is the comparison of
  corresponding lists of values, in key order.

* Any given uniqlet never compares as identical to anything but
  itself. Any two uniqlets have a consistent, transitive, and
  commutative &mdash; but otherwise arbitrary &mdash; ordering.

* Tokens compare by type as primary, and value as secondary.
  With types equal, highlets without a value order earlier than
  ones with a value.

#### `lowOrderIs value1 value2 check1 check2? <> boolean`

The two values are compared as with `lowOrder`. The int
result of that comparison are checked for equality with
the one or two check values. If the comparison result is equal
to either check value, this function returns `true`. Otherwise
it returns `false`.

**Note:** This function exists in order to provide a primitive
comparison function that returns a boolean. Without it (or something
like it), there would be no way to define boolean-returning
comparators in-language.


<br><br>
### In-Language Definitions

#### `eq value1 value2 <> boolean`

Checks for equality. Returns `true` iff the two given values are
identical.

#### `le value1 value2 <> boolean`

Checks for a less-than-or-equal relationship. Returns `true` iff the
first value orders before the second or is identical to it.

#### `lt value1 value2 <> boolean`

Checks for a less-than relationship. Returns `true` iff the first value
orders before the second.

#### `ge value1 value2 <> boolean`

Checks for a greater-than-or-equal relationship. Returns `true` iff the
first value orders after the second or is identical to it.

#### `gt value1 value2 <> boolean`

Checks for a greater-than relationship. Returns `true` iff the first value
orders after the second.

#### `ne value1 value2 <> boolean`

Checks for inequality. Returns `true` iff the two given values are not
identical.

#### `not boolean <> boolean`

Returns the opposite boolean to the one given.

**Note:** Only accepts boolean arguments.
