Samizdat Layer 0: Core Library
==============================

Record
------

<br><br>
### Class Method Definitions

#### `class.new(name, data?) -> isa Record`

Returns a record with the given name (a symbol) and optional `data` payload.
If `data` is not specified, it defaults to `@{}` (the empty symbol table).
If `data` is specified, then it must either be a symbol table or a record.
If passed as a record, the record name of `data` is ignored; only its
bindings matter. These equivalences hold for Samizdat source code:

```
@x{}             is equivalent to  Record.new(@x)
@(expr){}        is equivalent to  Record.new(expr)
@x{value*}       is equivalent to  Record.new(@x, value)
@(expr){value*}  is equivalent to  Record.new(expr, value)
@x{key: value}   is equivalent to  Record.new(@x, @{key: value})
```

It is a fatal error (terminating the runtime) to pass for `name` anything
other than a symbol. It is also a fatal error to pass for `data` anything
other than a symbol table.

**Syntax Note:** Used in the translation of `@(type){...}` and related forms.


<br><br>
### Method Definitions: `Record` protocol

#### `.cat(more*) -> isa Record`

Returns a record consisting of the combination of the mappings of `this` and
the arguments, with the same name as `ths`. Arguments must each be a record
or a symbol table.

For any keys in common between the arguments, the lastmost argument's value
is the one that ends up in the result. Despite the `cat` name, strictly
speaking this isn't a linear concatenation, but it is as close as one can
get to it given the class's key uniqueness constraints.

#### `.del(symbols*) -> isa Record`

Returns a record with the same name and mappings as `this`, except without
any data bindings for the given `symbols`.

#### `.get(symbol) -> . | void`

Returns the value mapped to the given `symbol` (a symbol) in the given
record. If there is no such mapping, then this returns void.

#### `.get_data() -> isa SymbolTable`

Returns the data payload of the given record.

#### `.get_name() -> isa Symbol`

Returns the name (tag) of the given record.

#### `.hasName(name) -> isa Record`

Returns `this` if its name is as given, or void if not.


<br><br>
### Method Definitions: `Value` protocol

#### `.castToward(cls) -> . | void`

This class knows how to cast as follows:

* `Core` &mdash; Returns `this`.

* `SymbolTable` &mdash; Returns the data payload of `this`. **Note:** This
  cast exists so that it is possible to do interpolation of records when
  constructing records and symbol tables.

* `Value` &mdash; Returns `this`.

#### `.crossEq(other) -> logic`

Compares two records. Two records are equal if they have equal names and
equal data payloads.

#### `.crossOrder(other) -> isa Symbol`

Compares two records for order. Records order by name as the major order
and data payload as minor order.

#### `.perEq(other) -> logic`

Default implementation.

#### `.perOrder(other) -> isa Symbol`

Default implementation.
