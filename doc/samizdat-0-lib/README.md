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

Each section is divided into subsections: generic functions (one section
per protocol), primitive defintions, and one for in-language definitions.
The first sections are where generic functions are specified as a general
definition, as well as where particular types indicate which generics they
bind and with what specific meaning. From the perspective of "client"
code written in the language, there is no distinction between the final
two sections, but from the perspective of implementation, there is.
In particular, an implementation of *Samizdat 0* must provide the primitive
definitions, but it can rely on the canonical in-language library source
for the remainder, which is written in terms of the primitives.

In addition, functions that are used in the translation of syntactic
constructs are marked with a note along the lines of,
"**Syntax Note:** Used in the translation of `example` forms".

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
will be returned, or one of:

* `.` &mdash; Returns an arbitrary value.

* `void` &mdash; Returns void.

* `type | void` &mdash; Might or might not return a value. If a value, it
  will be of the indicated type (including `.` to indicate an arbitrarily
  typed value).

### Contents

* Types and Protocols
  * [Boolean](Boolean.md)
  * [Builtin](Builtin.md)
  * [Function](Function.md)
  * [ClosedRange](ClosedRange.md) (part of `Range` module)
  * [Collection (multiple-value containers)](Collection.md)
  * Generator &mdash; See [core::Generator](Generator.md) module.
  * [Generic](Generic.md)
  * [Int](Int.md)
  * [List](List.md)
  * [Map](Map.md)
  * [Module](Module.md)
  * [OpenRange](OpenRange.md) (part of `Range` module)
  * [String](String.md)
  * [Type](Type.md)
  * [Uniqlet](Uniqlet.md)
  * [Value (the base type)](Value.md)
  * [One-Off Generics](OneOff.md)

* Protocol Modules
  * [proto::Bitwise](Bitwise.md)
  * [proto::Box](Box.md)
  * [proto::Generator](Generator.md)
  * [proto::Number](Number.md)

* Implementation Modules
  * [core::Bitwise](Bitwise.md)
  * [core::Box](Box.md)
  * [core::EntityMap](EntityMap.md)
  * [core::Format (string formatting)](Format.md)
  * [core::FunctionForwarder](FunctionForwarder.md)
  * [core::Generator](Generator.md)
  * [core::Io0](Io0.md)
  * [core::Io1](Io1.md)
  * [core::ParseForwarder](ParseForwarder.md)
  * [core::Peg (parsing)](Peg.md)
  * [core::Range](Range.md)

* Other
  * [Constants](constants.md)
  * [Conditionals And Iteration](conditional.md)
  * [Meta-Library](meta.md)

- - - - -

```
Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
Licensed AS IS and WITHOUT WARRANTY under the Apache License,
Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
```
