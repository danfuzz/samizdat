Samizdat Layers 0&ndash;2 Library Guide
=======================================

The following sections define the core library for Samizdat Layers 0&ndash;2.

The core library isn't as minimal as the language itself, but it is still
intended to be more on the minimal side of things. Beyond the true
essentials, the library contains bindings that have proven to be useful
in practice for building programs in the language.

Each section covers definitions for one conceptual area, either
concerning a class, data type, module, or some more nebulous concept.

Each section is divided into subsections: classes, methods (one section
per protocol), functions, and constants. The classes section links to
classes defined in the area (generally as part of a module). The method
sections are where methods are specified as a general definition, as well as
where particular classes indicate which methods they bind and with what
specific meaning. The other two sections indicate functions and constants that
are exported from the module but not tied directly to any particular class.

In addition, methods and functions that are used in the translation of
syntactic constructs are marked with a note along the lines of,
"**Syntax Note:** Used in the translation of `example` forms."

Each method and function listed here is introduced with a "prototype" that
has the following form, meant to mimic how functions are defined in the
language:

```
name(argument, argument?, argument*) -> returnValue
```

* `name` &mdash; The name of the function or method. In the case of a
  method, it is prefixed with a dot (e.g. `.name() ...`). In the case of a
  class method, it is further prefixed with `class` literally (e.g.
  `class.name() ...`).

* `argument` (with no suffix) &mdash; A required (non-optional) argument.

* `argument?` &mdash; An optional argument.

* `argument*` &mdash; Any number of optional arguments (including none).

* `argument+` &mdash; Any positive number of optional arguments (that is,
  at least one).

* `returnValue` &mdash; The possible kind(s) of return value.

In an actual function specification, `argument`s are replaced with names
that indicate the class/type of value expected. Similarly, `returnValue`
is replaced with either the class/type returned, a more specific value that
will be returned, or one of:

* `.` &mdash; Returns an arbitrary value.

* `void` &mdash; Returns void.

* `is Name` &mdash; Returns an instance of the class `Name`.

* `is [Name*]` / `is [Name+]` &mdash; Returns a list of instances of the
  class `Name`. In the second case, the list will never be empty.

* `== value` &mdash; Returns a specific value.

* `x | y` &mdash; Returns either type `x` or type `y`.

* `logic` &mdash; Shorthand that means the same thing as `. | void` while
  implying that the expected use case is for conditional logic.

### Contents

* Global Classes
  * [Bool](Bool.md)
  * [Box](Box.md) (also includes Cell, Lazy, NullBox, Promise, and Result)
  * [Builtin](Builtin.md)
  * [Class](Class.md)
  * [Cmp](Cmp.md)
  * [Core](Core.md)
  * [If](If.md)
  * [Int](Int.md)
  * [List](List.md)
  * [Map](Map.md)
  * [Null](Null.md)
  * [Object](Object.md)
  * [Record](Record.md)
  * [Symbol](Symbol.md)
  * [SymbolTable](SymbolTable.md)
  * [String](String.md)
  * [Value (the base class/type)](Value.md)

* Protocols
  * [Bitwise](Bitwise.md)
  * [Box](Box.md)
  * [Collection](Collection.md)
  * [Function](Function.md)
  * [Generator](Generator.md)
  * [Number](Number.md)
  * [Sequence](Sequence.md)

* Implementation Modules
  * [core.Code](core.Code.md)
  * [core.CommandLine](core.CommandLine.md)
  * [core.EntityMap](core.EntityMap.md)
  * [core.Format (string formatting)](core.Format.md)
  * [core.FilePath](core.FilePath.md)
  * [core.Generator](core.Generator/README.md)
    * [FilterGenerator](core.Generator/FilterGenerator.md)
    * [ListWrapGenerator](core.Generator/ListWrapGenerator.md)
    * [NullGenerator](core.Generator/NullGenerator.md)
    * [OptGenerator](core.Generator/OptGenerator.md)
    * [ParaGenerator](core.Generator/ParaGenerator.md)
    * [RepeatGenerator](core.Generator/RepeatGenerator.md)
    * [SerialGenerator](core.Generator/SerialGenerator.md)
    * [ValueGenerator](core.Generator/ValueGenerator.md)
  * [core.Globals](core.Globals.md)
  * [core.Io0](core.Io0.md)
  * [core.Lang*](core.LangN.md)
  * [core.LangNode](core.LangNode.md)
  * [core.ModuleSystem](core.ModuleSystem/README.md)
    * [ExternalLoader](core.ModuleSystem/ExternalLoader.md)
    * [InternalLoader](core.ModuleSystem/InternalLoader.md)
  * [core.Peg (parsing)](core.Peg/README.md)
    * [BasicState](core.Peg/BasicState.md)
    * [CacheState](core.Peg/CacheState.md)
    * [PegAny](core.Peg/PegAny.md)
    * [PegChoice](core.Peg/PegChoice.md)
    * [PegCode](core.Peg/PegCode.md)
    * [PegEof](core.Peg/PegEof.md)
    * [PegFail](core.Peg/PegFail.md)
    * [PegLookaheadFailure](core.Peg/PegLookaheadFailure.md)
    * [PegLookaheadSuccess](core.Peg/PegLookaheadSuccess.md)
    * [PegMain](core.Peg/PegMain.md)
    * [PegRepeat](core.Peg/PegRepeat.md)
    * [PegResult](core.Peg/PegResult.md)
    * [PegSet](core.Peg/PegSet.md)
    * [PegSetComplement](core.Peg/PegSetComplement.md)
    * [PegSequence](core.Peg/PegSequence.md)
    * [PegThunk](core.Peg/PegThunk.md)
  * [core.Range](core.Range/README.md)
    * [ClosedRange](core.Range/ClosedRange.md)
    * [OpenRange](core.Range/OpenRange.md)

* Other
  * [Constants](constants.md)
  * [Meta-Control](meta-control.md)

- - - - -

```
Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
Licensed AS IS and WITHOUT WARRANTY under the Apache License,
Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
```
