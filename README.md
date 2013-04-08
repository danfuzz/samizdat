Samizdat
========

Watch this space!

Plan of attack
--------------

* `c-api/sam-data.h` &mdash; Public interface to Samizdat data model.

  Written in C.

* `libsam-data-0` &mdash; Data structure C library, "minimal" edition.

  Has functions to (a) build up low-layer data values piece by piece,
  and (b) access that data in various straightforward ways. Doesn't
  do memory management at all. (That is, it allocates but never GCs.)

  Written in C, implements API defined by `sam-data.h`.

* `samizdat-0` &mdash; Layer 0 (minimal) language implementation.

  Tool that accepts input in the form of minimalistic source code, and
  runs it. The language is meant to be a "parti" of final Samizdat,
  that is, it embodies *just* the main thrusts of the language with
  very little embellishment. The language it accepts is called
  *Samizdat Layer 0*.

  The associated core library includes a conditional (implementation
  of a cascading `if...else if...`), simple file I/O, and not much
  else.

  Written in C using `libsam-data-0`.

* `samizdat-1` &hellip; `samizdat-N` &mdash; Layers
  1 through *n* language implementations.

  For `M` and `N` where `N == M + 1`, `samizdat-N` is written in
  the language defined by *Samizdat Layer M*. `samizdat-N`
  accepts the language *Samizdat layer N*, translating it to the
  same underlying form that `samizdat-0` executes.

* `sam-comp` &mdash; Tool to compile Samizdat source code.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  code to be linked and run with an associated library (details
  TBD).

  Written in *Samizdat Layer N* (for the ultimate value of `N`).

* ?

* Profit!
