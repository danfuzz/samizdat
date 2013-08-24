Samizdat Layer 0: Core Library
==============================

Lists
-----

A `List` is a kind of `Sequence`.

<br><br>
### Generic Function Definitions: `Value` protocol

#### `perEq(list1, list2) <> list | void`

Compares two lists. Two lists are equal if they have equal elements in
identical orders.

#### `perOrder(list1, list2) <> int`

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
with the given `value`, or added if `n == size(list)`.

#### `reverse(list) <> list`

Returns a list like the one given, except with elements in the opposite
order.

**Syntax Note:** Used in the translation of `switch` forms.

#### `size(list) <> int`

Returns the number of elements in the list.

#### `slice(list, start, end?) <> list`

Returns a slice of the given list.


<br><br>
### Primitive Definitions

#### `listFilter(function, list) <> list`

Processes each element of a list using a filter function, collecting
the results into a new list, and returning that list. The filter function
is called once for each list element (in order), passing the function a
single argument of the element in question.

If the function returns void for any given call, then no item is added for
the corresponding element. This means the size of the result may be
smaller than the size of the argument.

**Note:** This is a special-case version of the more general function
`collectFilter` (see which).

#### `makeList(rest*) <> list`

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
fn makeList(rest*) { <> rest };
```

**Syntax Note:** Used in the translation of `[item, ...]`,
`[key: value, ...]`, and `switch` forms.


<br><br>
### In-Language Definitions

(none)
