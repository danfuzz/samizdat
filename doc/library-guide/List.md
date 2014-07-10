Samizdat Layer 0: Core Library
==============================

List
----

A `List` is a kind of `Sequence`.

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(list, other) -> list | void`

Default implementation.

#### `perOrder(list, other) -> int`

Default implementation.

#### `totalEq(list1, list2) -> list | void`

Compares two lists. Two lists are equal if they have equal elements in
identical orders.

#### `totalOrder(list1, list2) -> int`

Compares two lists for order. Lists order by pairwise corresponding-element
comparison, with a strict prefix always ordering before its longer brethren.


<br><br>
### Generic Function Definitions: `Collection` and `Sequence` protocols

#### `cat(list, more*) -> int`

Returns a list consisting of the concatenation of the elements
of all the argument lists, in argument order.

#### `del(list, n) -> list`

Returns a list like the given one, but without the `n`th element.

#### `get(list, key) -> . | void`

Defined as per the `Sequence` protocol.

#### `get_size(list) -> int`

Returns the number of elements in the list.

#### `keyList(list) -> list`

Defined as per the `Sequence` protocol.

#### `nth(list, n) -> . | void`

Gets the nth element of the string.

#### `nthMapping(list, n) -> map | void`

Gets the nth mapping of the list.

#### `put(list, n, value) -> list`

Returns a list like the given one, but with the `n`th element replaced
with the given `value`, or added if `n == get_size(list)`.

#### `reverse(list) -> list`

Returns a list like the one given, except with elements in the opposite
order.

**Syntax Note:** Used in the translation of `switch` forms.

#### `sliceExclusive(list, start, end?) -> list`

Returns an end-exclusive slice of the given list.

#### `sliceInclusive(list, start, end?) -> list`

Returns an end-inclusive slice of the given list.

#### `valueList(list) -> list`

Defined as per the `Sequence` protocol. In this case, this function always
returns the given `list`, directly.



<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(list, optFilterFunction?) -> list`

Filters the elements of `list` using the given filter function if supplied,
or just returns `list` if there is no filter function.

#### `nextValue(list, box) -> generator | void`

On a non-empty list, calls `store(box, list[0])` and returns
`list[1..]`. On an empty list, calls `store(box)` and returns void.


<br><br>
### Primitive Definitions

#### `makeList(elems*) -> list`

Returns a list with the given elements (in argument order).
These equivalences hold for Samizdat Layer 0 source code:

```
v = [v1];      is equivalent to  v = makeList(v1);
v = [v1, v2];  is equivalent to  v = makeList(v1, v2);
[etc.]
```

**Note:** The equivalence requires at least one argument, even though
the function is happy to operate given zero arguments.

**Note:** Technically, this function could be defined in-language as the
following, but for practical reasons &mdash; e.g. and in particular,
expected ordering of human operations during the course of
bootstrapping an implementation, as well as efficiency of
implementation (without sacrificing clarity) &mdash; it makes sense to
keep this defined as a core primitive:

```
fn makeList(elems*) { return elems };
```

**Syntax Note:** Used in the translation of `[item, ...]`,
`{key: value, ...}`, `switch`, and multiple-binding `if` forms.


<br><br>
### In-Language Definitions

(none)
