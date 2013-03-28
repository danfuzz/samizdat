Samizdat
========

Watch this space!

Plan of attack
--------------

* `libsam-dumb-data` &mdash; data structure C library, "dumb" edition.

  Has functions to (a) build up low-layer data values piece by piece,
  and (b) dump C code based on such data values to recreate them using
  the same library. Doesn't do memory management at all. (That is,
  it allocates but never GCs.)

  Written in C.

* `libsam-dumb-exec` &mdash; execution engine library, "dumb" edition.

  Has function to execute a given low-layer data structure using
  as-minimal-as-possible semantics and support library. Has local
  variables and recursion, can read stdin and write stdout, and
  not much else.

  Written in C using `libsam-dumb-data`.

* `sam-dumb-comp-1` &mdash; tool to compile source code,
  "dumb" edition, first layer.

  Accepts input in the form of minimalistic "source code" which
  is not much more than a series of assignment statements
  `varName := dataStruct` where `dataStruct` is low-layer Samizdat
  data value syntax, along with minimal control functionality
  (function defs, function calls, conditional statement).
  Outputs C code that recreates top-level data values corresponding
  to the top-level definitions. When compiled and linked with
  `libsam-dumb-exec`, executes the code.

  Written in C using `libsam-dumb-data`.

* `sam-dumb-comp-2` &hellip; `sam-dumb-comp-N` &mdash; tools to
  compile source code, "dumb" edition, layers two through *n*.

  For `M` and `N` where `N == M + 1`, `sam-dumb-comp-N` is written in
  the language defined by `sam-dumb-M`. Compiler `sam-dumb-comp-N`
  accepts the language `sam-dumb-N` and outputs C code to be linked
  and run with `libsam-dumb-exec`.

* `sam-comp` &mdash; tool to compile Samizdat.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  C code to be linked and run with `libsam-dumb-exec`.

  Written in `sam-dumb-N` (for the ultimate value of `N`).

* `libsam-magic` &mdash; "magic" execution support library

  Library that includes "magic" type functions that make it
  possible to write an allocator / GC in Samizdat. Canonical
  magic function: convert int to reference.

  Written in C.

* `libsam-data` &mdash; Non-dumb data structure library, including GC.

  Written in Samizdat, compiled with `sam-comp`.

* ?

* Profit!
