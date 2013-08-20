Samizdat Layer 0: Core Library
==============================

Lists
-----

<br><br>
### Generic Function Definitions: `Collection` protocol

#### `size(list) <> int`

Returns the number of elements in the list.


<br><br>
### Primitive Definitions

#### `listCat(lists*) <> list`

Returns a list consisting of the concatenation of the elements
of all the argument lists, in argument order.

#### `listDelNth(list, n) <> list`

Returns a list just like the given one, except that the `n`th
(zero-based) element is deleted, if `n` is a valid int index into
the given list. If `n` is not a valid index (not an int, or with
a value out of range), then this returns the original list as the
result.

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

#### `listGet(list, key) <> . | void`

Collection-style element access. Returns the same thing as `listNth(list, key)`
if `key` is a valid int index into the list. If not, this returns void.

#### `listInsNth(list, n, value) <> list`

Returns a list just like the given one, except that the `n`th
(zero-based) element is set to be the given value (an arbitrary
value), and all elements at or beyond index `n` in the original
are shifted up by one index.

`n` must be non-negative and less than or equal to the size of the
list. If not, it is an error (terminating the runtime).

#### `listNth(list, n) <> . | void`

Returns the `n`th (zero-based) element of the given list, if `n` is
a valid int index into the list. Otherwise, if `n` is a non-negative int,
this returns void. Otherwise, this terminates the runtime with an error.

#### `listPutNth(list, n, value) <> list`

Returns a list just like the given one, except that the `n`th
(zero-based) element is set to be the given value (an arbitrary
value), replacing whatever was at that index in the original. If
`n` is the size of the original list, then this call behaves
exactly like `listAppend`.

`n` must be non-negative and less than or equal to the size of the
list. If not, it is an error (terminating the runtime).

#### `listReverse(list) <> list`

Returns a list consisting of the elements of the given list, but in the
opposite order.

**Syntax Note:** Used in the translation of `switch` forms.

#### `listSlice(list, start, end?) <> list`

Returns a list consisting of a "slice" of elements of the given
list, from the `start` index (inclusive) through the `end` index
(exclusive). `start` and `end` must both be ints, must be valid indices
into `list` (with the list size being a valid index), and must form a
range (though possibly empty) with `start <= end`. If `end` is not
specified, it defaults to the end of the list.

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

#### `listButFirst(list) <> . | void`

Returns a list consisting of all the elements of the given list
except for the first element (that is, all but the first element). If
the given list is empty, this returns void. This is similar to
`listSlice(list, 1)` or `listDelNth(list, 0)`, except that this function
behaves differently than either of those when the list is empty.

#### `listButLast(list) <> . | void`

Returns a list consisting of all the elements of the given list
except for the last element (that is, all but the last element). If
the given list is empty, this returns void. This is similar to
`listSlice(list, 0, sub(size(list), 1))` or
`listDelNth(list, sub(size(list), 1))`, except that this function
behaves differently than either of those when the list is empty.

#### `listFirst(list) <> . | void`

Returns the first element of the given list or void if the list is empty.
This is just a convenient shorthand for `listNth(list, 0)`.

#### `listLast(list) <> . | void`

Returns the last element of the given list or void if the list is empty.
This is just a convenient shorthand for
`listNth(list, sub(size(list), 1))`.
