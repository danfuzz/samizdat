Samizdat Layer 0: Core Library
==============================

List
----

A `List` is a kind of `Sequence`.

<br><br>
### Class Method Definitions

#### `.new(values*) -> list`

Constructs a list consisting of the given values, in order.
These equivalences hold for Samizdat Layer 0 source code:

```
v = [v1];      is equivalent to  v = List.new(v1);
v = [v1, v2];  is equivalent to  v = List.new(v1, v2);
[etc.]
```

**Note:** The equivalence requires at least one argument, even though
the function is happy to operate given zero arguments.

**Syntax Note:** Used in the translation of `[item, ...]`,
`{key: value, ...}`, `switch`, and multiple-binding `if` forms.


<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> list | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> list | void`

Compares two lists. Two lists are equal if they have equal elements in
identical orders.

#### `.totalOrder(other) -> int`

Compares two lists for order. Lists order by pairwise corresponding-element
comparison, with a strict prefix always ordering before its longer brethren.


<br><br>
### Method Definitions: `Collection` and `Sequence` protocols

#### `.cat(more*) -> int`

Returns a list consisting of the concatenation of the elements
of all the argument lists, in argument order.

#### `.del(n) -> list`

Returns a list like the given one, but without the `n`th element.

#### `.get(key) -> . | void`

Defined as per the `Sequence` protocol.

#### `.get_size() -> int`

Returns the number of elements in the list.

#### `.keyList() -> list`

Defined as per the `Sequence` protocol.

#### `.nth(n) -> . | void`

Gets the nth element of the string.

#### `.nthMapping(n) -> map | void`

Gets the nth mapping of the list.

#### `.put(n, value) -> list`

Returns a list like the given one, but with the `n`th element replaced
with the given `value`, or added if `n == get_size(list)`.

#### `.reverse() -> list`

Returns a list like the one given, except with elements in the opposite
order.

**Syntax Note:** Used in the translation of `switch` forms.

#### `.sliceExclusive(start, end?) -> list`

Returns an end-exclusive slice of the given list.

#### `.sliceInclusive(start, end?) -> list`

Returns an end-inclusive slice of the given list.

#### `.valueList() -> list`

Defined as per the `Sequence` protocol. In this case, this function always
returns `this`, directly.



<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> list`

Filters the elements of `this` using the given filter function if supplied,
or just returns `this` if there is no filter function.

#### `.fetch() -> . | void`

Returns void on an empty list. Returns the sole element of a single-element
list. Terminates with an error in all other cases.

#### `.nextValue(box) -> generator | void`

On a non-empty list, calls `box.store(this[0])` and returns
`this[1..]`. On an empty list, this just returns void.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

(none)
