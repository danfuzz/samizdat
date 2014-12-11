Samizdat Layer 0: Core Library
==============================

SymbolTable
-----------

A `SymbolTable` is a mapping from symbols to arbitrary other objects.
It is intentionally *not* a kind of `Collection`, as this class is intended
to be relatively "low level." That said, the class provides *some* of the
methods of `Collection`.


<br><br>
### Class Method Definitions

#### `class.new(args*) -> isa Map`

This makes a symbol table from a series of mappings, given as pairs of
symbol-then-value arguments. This function is meant to be exactly parallel to
`Map.new()` (see which).

**Syntax Note:** Used in the translation of `@{key: value, ...}` forms.

#### `class.singleValue(keys*, value) -> isa Map`

This makes a symbol table which maps any number of keys (including none)
to the same value. If no keys are specified, then this function returns
the empty symbol table. This function is meant to be exactly parallel to
`Map.singleValue()` (see which).

Note that the argument list is "stretchy" in front, which isn't
representable in real Samizdat syntax.

**Syntax Note:** Used in the translation of `@{key: value, ...}` forms.


<br><br>
### Method Definitions: `Value` protocol

#### `.crossEq(other) -> logic`

Compares two symbol tables. Two symbol tables are equal if they have
equal sets of mappings.

#### `.crossOrder(other) -> isa Symbol`

Compares two symbol tables for order. The size of the table is the major
order (smaller is earlier). After that, the keys are compared as sorted
lists. After that, corresponding values are compared in sorted-key order.

**Note:** Because two different unlisted symbols that happen to have the
same name are not considered ordered with respect to each other, it is
possible for two symbol tables to also be unordered with respect to each
other.

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol`

Default implementation.


<br><br>
### Method Definitions

#### `.cat(more*) -> isa SymbolTable`

Returns a symbol table consisting of the combination of the mappings of `ths`
and the arguments. Arguments are allowed to be symbol tables or anything which
can be cast to a symbol table, including notably records and maps.

For any keys in common between the arguments, the lastmost argument's value
is the one that ends up in the result. Despite the `cat` name, strictly
speaking this isn't a linear concatenation, but it is as close as one can
get to it given the class's key uniqueness constraints.

**Syntax Note:** Used in the translation of `@{key: value, ...}` forms.

#### `.del(symbols*) -> isa SymbolTable`

Returns a symbol table just like the given one, except that
the mappings for the given `symbols`, if any, are removed. If `this`
does not bind any of the given symbols, then this returns `this`.

#### `.get(symbol) -> . | void`

Returns the value mapped to the given `symbol` (a symbol) in the given
symbol table. If there is no such mapping, then this returns void.

#### `.get_size() -> isa Int`

Returns the number of mappings contained within `this`.
