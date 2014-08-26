Samizdat Layer 0: Core Library
==============================

SymbolTable
-----------

A `SymbolTable` is a mapping from symbols to arbitrary other objects.
It is intentionally *not* a kind of `Collection`. In particular, it is not
possible to iterate over the keys of an instance, merely to look up specific
symbols. The point of this is so that it's possible to use `SymbolTable`s
to hold what amounts to private mappings, by virtue of using anonymous
symbols as keys.


<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> symbolTable | void`

Default implementation.

#### `.perOrder(other) -> int`

Default implementation.

#### `.totalEq(other) -> symbolTable | void`

Compares two symbol tables. Two symbol tables are equal if they have
equal sets of mappings.

#### `.totalOrder(other) -> int`

Default implementation. Two non-identical symbol tables are not considered
ordered with respect to each other.


<br><br>
### Method Definitions

#### `.get(sel) -> . | void`

Returns the value mapped to the given `sel` (a symbol) in the given
symbol table. If there is no such mapping, then this returns void.


<br><br>
### Primitive Definitions

#### `makeSymbolTable(mappings*) -> symbolTable`

This makes a symbol table from a series of mappings, given as pairs of
symbol-then-value arguments.


<br><br>
### In-Language Definitions

(none)
