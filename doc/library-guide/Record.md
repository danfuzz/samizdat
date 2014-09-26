Samizdat Layer 0: Core Library
==============================

Record
------

<br><br>
### Method Definitions

#### `.dataOf() -> symbolTable`

Returns the data payload of the given record.

#### `.get_name() -> symbol`

Returns the name (tag) of the given record.

#### `.hasName(name) -> record`

Returns `this` if its name is as given, or void if not.


### Primitive Definitions

#### `makeRecord(clsOrName, value?) -> .`

Returns a record with the given class (a value of class `Class`) or name
(a symbol), and optional data payload value (which must be a symbol table if
present). If `value` is not specified, it defaults to `@{}` (the empty symbol
table). These equivalences hold for Samizdat source code:

```
@x(value)    is equivalent to  makeRecord(@x, value)
@(x)(value)  is equivalent to  makeRecord(x, value)
```

It is a fatal error (terminating the runtime) to pass for `clsOrName`
anything other than a record class or a symbol. It is also a fatal error
to pass for `value` anything other than a symbol table.

**Syntax Note:** Used in the translation of `@(type)(value)` and related forms.

#### `makeRecordClass(name) -> class`

Returns a `Class` instance which represents a record class
with the given `name`. `name` is an arbitrary symbol.

Two different calls to this function are guaranteed to return the same exact
class when given identical `name` arguments.

**Note:** This function is vestigial and will be removed in an upcoming
patch.


<br><br>
### In-Language Definitions

(none)
