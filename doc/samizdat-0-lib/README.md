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

* [Constants](constants.md)
* [Ultraprimitives](ultra.md)
* [General Low-Layer Values](low.md)
* [Conditionals And Iteration](conditional.md)
* [Booleans And Comparison](boolean.md)
* [Ints](int.md)
* [Strings](string.md)
* [Lists](list.md)
* [Maps](map.md)
* [Tokens](token.md)
* [Generators](generator.md)
* [Functions And Code](code.md)
* [I/O](io.md)
* [String Formatting And Argument Parsing](format.md)
* [Parsing](peg.md)
* [Meta-Library](meta.md)

- - - - -

```
Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
Licensed AS IS and WITHOUT WARRANTY under the Apache License,
Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
```
