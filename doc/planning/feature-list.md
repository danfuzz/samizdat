Feature List
============

Existing
--------

* Language
  * Subset of language syntax:
    * Basic class definition.
    * Basic function definition.
    * Control constructs.
    * Parser definition.

* Runtime
  * Interpreted runtime, written in C:
    * Basic allocator / garbage collector.
    * Basic type / class semantics:
      * System defined "special" classes.
      * In-language defined "regular" classes.
      * Support for class and instance methods.
    * Full support for single-thread semantics.
    * Full support for dynamic typing.

* Library
  * Module system.
  * Basic immutable-data data types:
    * Booleans
    * Integers (limited range).
    * Lists.
    * Maps (of arbitrary keys to arbitrary values).
    * Null.
    * Records (name-tagged name-value mappings).
    * Strings (of characters).
    * Symbols (runtime-significant identifiers).
    * Symbol tables.
  * Small set of utility functionality:
    * PEG-based combinator library.
    * Numeric and character ranges.
    * File path parsing.
    * Limited commandline parsing.
    * Value formatting (stringification).
  * Basic I/O support (enough to build an off-line compiler).

* Compiler
  * Minimal "compiler" which produces executable tree nodes, as C code.
  * Basic compiler which doesn't rely on executable tree node interpretation.


Required
--------

* Language
  * Full language syntax:
    * Full class definition.
    * Full function definition.

* Runtime
  * Complete compiled runtime, written in Samizdat:
    * Full type / class semantics:
      * Protocols (a/k/a interfaces).
      * Utilities (uninstantiable classes, used as "method repositories").
      * General inheritance mechanism.
      * Gradual typing annotations for formals and variables.
    * Modern allocator / garbage collector.
    * Full support for multiple threads (actor model).
    * Built-in native compilation (JIT or JIT-like):
      * Used for evaluating executable tree nodes.
      * x86 back-end.
      * ARM back-end.
  * Basic interpreted runtime, written in Samizdat.

* Library
  * Full numeric support:
    * Integers (unbounded range).
    * Floating point (64-bit, IEEE 754).
  * Full complement of basic data types:
    * Sets (of arbitrary values).
    * Binary data blobs.
  * Rich I/O library.
  * Rich networking library.
  * Posix coverage library.
  * Modern parser support (PEG+packrat and/or GLL).

* Compiler
  * Full compiler to C / native executable code:
    * Produces decent code.
    * Produces debuggable code.
    * Can be used for self-hosting.
    * Emits useful error messages.
