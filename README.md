Samizdat
========

Watch this space!

Plan of attack
--------------

* `interface/sam-data.h` &mdash; Public interface to Samizdat data model

  Written in C.

* `interface/sam-exec.h` &mdash; Public interface to Samizdat
  execution model.

  Written in C.

* `libsam-data-0` &mdash; Data structure C library, "minimal" edition.

  Has functions to (a) build up low-layer data values piece by piece,
  and (b) dump C code based on such data values to recreate them using
  the same library. Doesn't do memory management at all. (That is,
  it allocates but never GCs.)

  Written in C, implements API defined by `sam-data.h`.

* `libsam-exec-0` &mdash; Execution engine library, "minimal" edition.

  Has function to execute a given low-layer data structure using
  as-minimal-as-possible semantics and support library. Has local
  variables and recursion, can read stdin and write stdout, and
  not much else.

  Written in C, linked with `libsam-data-0`, implements API defined
  by `sam-exec.h`.

* `sam-comp-0` &mdash; Tool to compile source code, "minimal" edition.

  Accepts input in the form of minimalistic "source code" which is not
  much more than a series of assignment statements `varName :=
  dataStruct` where `dataStruct` is low-layer Samizdat data value
  syntax, along with minimal control functionality (function defs,
  function calls, conditional statement). This language is called
  *Samizdat Layer 0*. Outputs C code that recreates top-level data
  values corresponding to the top-level definitions. When compiled and
  linked with `libsam-exec-0`, executes the code.

  Written in C using `libsam-data-0`.

* `sam-comp-1` &hellip; `sam-comp-N` &mdash; Tools to
  compile source code, layers 1 through *n*.

  For `M` and `N` where `N == M + 1`, `sam-comp-N` is written in
  the language defined by *Samizdat Layer M*. Compiler `sam-comp-N`
  accepts the language *Samizdat layer N* and outputs C code to be linked
  and run with `libsam-exec-0`.

* `sam-comp-prelim` &mdash; Tool to compile Samizdat source code.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  code to be linked and run with `libsam-exec-0`.

  Written in *Samizdat Layer N* (for the ultimate value of `N`).

* `libsam-magic` &mdash; "Magic" execution support library.

  Library that includes "magic" type functions that make it
  possible to write an allocator / GC in Samizdat. Canonical
  magic function: convert int to reference.

  Written in C.

* `libsam-data` &mdash; Data structure library, including GC.

  Implements API defined by `sam-data.h`.

  Written in Samizdat, compiled with `sam-comp-prelim` to link with
  `libsam-magic`.

* `libsam-exec` &mdash; Execution library.

  Implements API defined by `sam-exec.h`.

  Written in Samizdat, compiled with `sam-comp-prelim` to link with
  `libsam-magic` and `libsam-data`.

* `sam-comp` &mdash; Tool to compile Samizdat, relinked.

  Same (truly identical) code as `sam-comp-prelim`. The difference is
  in the linking. Written in Samizdat, compiled with `sam-comp-prelim`
  to link with `libsam-exec` (et al).

* ?

* Profit!
