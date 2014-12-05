Samizdat Layer 0: Core Library
==============================

List
----

A `List` is a kind of `Sequence`.

<br><br>
### Class Method Definitions

#### `class.new(values*) -> is List`

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

#### `.perEq(other) -> is List | void`

Default implementation.

#### `.perOrder(other) -> is Symbol`

Default implementation.

#### `.crossEq(other) -> is List | void`

Compares two lists. Two lists are equal if they have equal elements in
identical orders.

#### `.crossOrder(other) -> is Symbol`

Compares two lists for order. Lists order by pairwise corresponding-element
comparison, with a strict prefix always ordering before its longer brethren.


<br><br>
### Method Definitions: `Collection` and `Sequence` protocols

#### `.cat(more*) -> is Int`

Returns a list consisting of the concatenation of the elements
of `ths` and all the argument lists, in argument order.

#### `.del(ns*) -> is List`

Returns a list like the given one, but without the indicated elements
(by index). If a given index is repeated more than once, it has the same
effect as if it were only mentioned once.

#### `.get(key) -> . | void`

Defined as per the `Sequence` protocol.

#### `.get_size() -> is Int`

Returns the number of elements in the list.

#### `.keyList() -> is List`

Defined as per the `Sequence` protocol.

#### `.nth(n) -> . | void`

Gets the nth element of the string.

#### `.repeat(count) -> is List`

Returns a list consisting of `count` repetitions of the contents of `this`.
`count` must be a non-negative int.

#### `.reverse() -> is List`

Returns a list like the one given, except with elements in the opposite
order.

**Syntax Note:** Used in the translation of `switch` forms.

#### `.sliceExclusive(start, end?) -> is List`

Returns an end-exclusive slice of the given list.

#### `.sliceInclusive(start, end?) -> is List`

Returns an end-inclusive slice of the given list.

#### `.valueList() -> is List`

Defined as per the `Sequence` protocol. In this case, this function always
returns `this`, directly.



<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> is List`

Filters the elements of `this` using the given filter function if supplied,
or just returns `this` if there is no filter function.

#### `.fetch() -> . | void`

Returns void on an empty list. Returns the sole element of a single-element
list. Terminates with an error in all other cases.

#### `.nextValue(box) -> is List | void`

On a non-empty list, calls `box.store(this[0])` and returns
`this[1..]`. On an empty list, this just returns void.
