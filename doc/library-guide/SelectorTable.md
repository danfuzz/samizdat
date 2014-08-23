Samizdat Layer 0: Core Library
==============================

SelectorTable
-------------

A `SelectorTable` is a mapping from selectors to arbitrary other objects.
It is intentionally *not* a kind of `Collection`. In particular, it is not
possible to iterate over the keys of an instance, merely to look up specific
selectors. The point of this is so that it's possible to use `SelectorTable`s
to hold what amounts to private mappings, by virtue of using anonymous
selectors as keys.


<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> selectorTable | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> selectorTable | void`

Compares two selector tables. Two selector tables are equal if they have
equal sets of mappings.

#### `.totalOrder(other) -> int`

Default implementation. Two non-identical selector tables are not considered
ordered with respect to each other.


<br><br>
### Method Definitions

#### `.get(sel) -> . | void`

Returns the value mapped to the given `sel` (a selector) in the given
selector table. If there is no such mapping, then this returns void.


<br><br>
### Primitive Definitions

#### `makeSelectorTable(mappings*) -> selectorTable`

This makes a selector table from a series of mappings, given as pairs of
selector-then-value arguments.


<br><br>
### In-Language Definitions

(none)
