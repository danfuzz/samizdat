Samizdat Layer 0: Core Library
==============================

List
----

A `List` is a kind of `Sequence`.

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(list, other) <> list | void`

Calls `totEq`.

#### `perOrder(list, other) <> int`

Calls `totOrder`.

#### `totEq(list1, list2) <> list | void`

Compares two lists. Two lists are equal if they have equal elements in
identical orders.

#### `totOrder(list1, list2) <> int`

Compares two lists for order. Lists order by pairwise corresponding-element
comparison, with a strict prefix always ordering before its longer brethren.


<br><br>
### Generic Function Definitions: `Collection` and `Sequence` protocols

#### `cat(list, more*) <> int`

Returns a list consisting of the concatenation of the elements
of all the argument lists, in argument order.

#### `del(list, n) <> list`

Returns a list like the given one, but without the `n`th element.

#### `get(list, key) <> . | void`

Defined as per the `Sequence` protocol.

#### `keyList(list, n) <> list`

Defined as per the `Sequence` protocol.

#### `nth(list, n) <> . | void`

Gets the nth element of the string.

#### `put(list, n, value) <> list`

Returns a list like the given one, but with the `n`th element replaced
with the given `value`, or added if `n == sizeOf(list)`.

#### `reverse(list) <> list`

Returns a list like the one given, except with elements in the opposite
order.

**Syntax Note:** Used in the translation of `switch` forms.

#### `sizeOf(list) <> int`

Returns the number of elements in the list.

#### `slice(list, start, end?) <> list`

Returns a slice of the given list.


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(list) <> list`

Returns `list`.

#### `filter(list, filterFunction) <> list`

Filters the elements of `list` using `filterFunction`.

#### `nextValue(list, box) <> generator | void`

On a non-empty list, calls `store(box, first(list))` and returns
`butFirst(list)`. On an empty list, calls `store(box)` and returns void.


<br><br>
### Primitive Definitions

#### `makeList(elems*) <> list`

Returns a list with the given elements (in argument order).
These equivalences hold for *Samizdat Layer 0* source code:

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
fn makeList(elems*) { <> elems };
```

**Syntax Note:** Used in the translation of `[item, ...]`,
`[key: value, ...]`, `switch`, and multiple-binding `if` forms.


<br><br>
### In-Language Definitions

(none)
