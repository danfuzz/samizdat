Samizdat 0 Library Guide
========================

The following sections define the core library for *Samizdat 0*.

The core library isn't as minimal as the language itself, but it is still
intended to be more on the minimal side of things. Beyond the true
essentials, the library contains bindings that have proven to be useful
in practice for building programs in the language. A subset of the library
is defined to be "primitive", that is written into the lowest layer of
language implementation as primitive functions. The remainder of the library
is implemented in-language in terms of the primitive functions and the
language syntax.

Each section covers definitions for one conceptual area, either
concerning a data type or some more nebulous concept.

Each section is divided into two subsections, one for primitive
defintions, and one for in-language definitions. From the perspective of
"client" code written in the language, there is no distinction between the
two sections, but from the perspective of implementation, there is.
In particular, an implementation of *Samizdat 0* must provide the
primitive definitions, but it can rely on the canonical in-language library
source for the remainder, which is written in terms of the primitives.

Each function listed here is introduced with a "prototype" that has
the following form, meant to mimic how functions are defined in the
language:

```
functionName(argument, argument?, argument*) <> returnValue
```

* `functionName` &mdash; The name of the function.

* `argument` (with no suffix) &mdash; A required (non-optional) argument.

* `argument?` &mdash; An optional argument.

* `argument*` &mdash; Any number of optional arguments (including none).

* `argument+` &mdash; Any positive number of optional arguments (that is,
  at least one).

* `returnValue` &mdash; The possible kind(s) of return value.

In an actual function specification, `argument`s are replaced with names
that indicate the type of value expected. Similarly, `returnValue`
is replaced with either the type returned, a more specific value that
will be returned, or one of `.` `!.` `. | !.` as described in
[the section about data types](data-syntax-semantics.md).

### Contents

* [Constants](library-constants.md)
* [Ultraprimitives](library-ultra.md)
* [General Low-Layer Values](library-low.md)
* [Conditionals And Iteration](library-conditional.md)
* [Booleans And Comparison](library-boolean.md)
* [Ints](library-int.md)
* [Strings](library-string.md)
* [Lists](library-list.md)
* [Maps](library-map.md)
* [Tokens](library-token.md)
* [Functions And Code](library-code.md)
* [I/O](library-io.md)
* [String Formatting And Argument Parsing](library-format.md)
* [Parsing](library-peg.md)
* [Meta-Library](library-meta.md)
