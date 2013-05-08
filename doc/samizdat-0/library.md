Samizdat Layer 0
================

Core Library
------------

The following sections define the core library for *Samizdat Layer 0*.

Each section covers definitions for one conceptual area, either
concerning a data type or some more nebulous concept.

Each section is divided into two subsections, one for primitive
defintions, and one for in-language definitions. From the perspective of
"client" code written in the language, there is no distinction between the
two sections, but from the perspective of implementation, there is.
In particular, an implementation of *Samizdat Layer 0* must provide the
primitive definitions, but it can rely on the canonical in-language library
source for the remainder, which is written in terms of the primitives.

Each function listed here is introduced with a "prototype" that has
the following form, meant to mimic how functions are defined in the
language:

```
functionName argument argument? argument* <> returnValue
```

* `functionName` &mdash; The name of the function.

* `argument` (with no suffix) &mdash; A required (non-optional) argument.

* `argument?` &mdash; An optional argument.

* `argument*` &mdash; Any number of optional arguments (including none)

* `returnValue` &mdash; The possible kind(s) of return value.

In an actual function specification, `argument`s are replaced with names
that indicate the type of value expected. Similarly, `returnValue`
is replaced with either the type returned, a more specific value that
will be returned, or one of `.` `~.` `. | ~.` as described in
[the section about data types](data-syntax-semantics.md).

### Contents

* [Constants](library-constants.md)
* [Ultraprimitives](library-ultra.md)
* [Conditionals And Iteration](library-conditional.md)
* [General Low-Layer Values](library-low.md)
* [Integers](library-integer.md)
* [Strings](library-string.md)
* [Lists](library-list.md)
* [Maps](library-map.md)
* [Highlets](library-highlet.md)
* [Functions And Code](library-code.md)
* [I/O](library-io.md)
* [Meta-Library](library-meta.md)
* [Booleans And Comparison](library-boolean.md)
* [String Formatting](library-format.md)
