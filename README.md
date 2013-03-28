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

* `sam-dumb-comp` &mdash; tool to compile source code,
  "dumb" edition.

  Accepts input in the form of minimalistic "source code" which
  is not much more than a series of assignment statements
  `varName := dataStruct` where `dataStruct` is low-layer Samizdat
  data value syntax, along with minimal control functionality
  (function defs, function calls, conditional statement).
  Outputs C code that recreates top-level data values corresponding
  to the top-level definitions. When compiled and linked with
  `libsam-dumb-exec`, executes the code.

  Written in C using `libsam-dumb-data`.

* compiler written in sam-dumb that's slightly smarter, still outputs
  C code to be linked with `libsam-dumb-exec`.

* N more layers of same, with increasing expressivity, ending up at
  bona fide Samizdat syntax.

* execution support library that makes it possible to write an
  allocator / GC in Samizdat.

* ?

* Profit!
