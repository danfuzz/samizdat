Samizdat Layer 0: Core Library
==============================

SymbolTable
-----------

A `SymbolTable` is a mapping from symbols to arbitrary other objects.
It is intentionally *not* a kind of `Collection`, as this class is intended
to be relatively "low level." That said, the class provides *some* of the
methods of `Collection`.


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

Compares two symbol tables for order. The size of the table is the major
order (smaller is earlier). After that, the keys are compared as sorted
lists. After that, corresponding values are compared in sorted-key order.

**Note:** Because two different anonymous symbols that happen to have the
same name are not considered ordered with respect to each other, it is
possible for two symbol tables to also be unordered with respect to each
other.


<br><br>
### Method Definitions

#### `.get(symbol) -> . | void`

Returns the value mapped to the given `symbol` (a symbol) in the given
symbol table. If there is no such mapping, then this returns void.

#### `.get_size() -> int`

Returns the number of mappings contained within `this`.

#### `.put(key, value) -> symbolTable`

Returns a symbol table just like `this`, except with a new mapping
for `key` to `value`. The result has a replacement for the existing
mapping for `key` if such a one existed, or has an additional mapping
in cases where `key` was not already bound.


<br><br>
### Primitive Definitions

#### `makeValueSymbolTable(keys*, value) -> symbolTable`

This makes a symbol table which maps any number of keys (including none)
to the same value. If no keys are specified, then this function returns
the empty symbol table. This function is meant to be exactly parallel to
`makeValueMap` (see which).

Note that the argument list is "stretchy" in front, which isn't really
representable in Samizdat syntax as presented.

#### `makeSymbolTable(mappings*) -> symbolTable`

This makes a symbol table from a series of mappings, given as pairs of
symbol-then-value arguments.


<br><br>
### In-Language Definitions

(none)
