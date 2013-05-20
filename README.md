Samizdat
========

This is the source for the Samizdat programming language, or "Sam" if
you want to be terse.

Samizdat is a high-level programming language somewhere down the
family lineage from all of [ALGOL](http://en.wikipedia.org/wiki/ALGOL),
[Lisp](http://en.wikipedia.org/wiki/LISP), and
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk). It is
intended that the language be an attractive target for use in
text processing, free-form data structure manipulation, and general
systems programming.

Find lots of documentation in [the doc/ directory](doc), including
a handful of [shout-outs](doc/shout-outs.md) that may help understand
where the system is coming from (both literally and figuratively).


Overview
--------

The following is an overview of the pieces of the system that currently
exist (if incompletely), followed by pieces that are merely planned.

### Existing pieces

* `samizdat-0` &mdash; *Samizdat Layer 0* interpreter.

  This is an interpreter for the *Samizdat Layer 0* language. See
  [the spec](doc/samizdat-0/README.md) and
  [the implementation's README](samizdat-0/README.md) for more details.

* `samizdat-0-lib` &mdash; *Samizdat Layer 0* in-language core library.

  This is an implementation of the non-primitive portion of the
  *Samizdat Layer 0* core library. See [the spec](doc/samizdat-0/README.md)
  and [the implementation's README](samizdat-0-lib/README.md) for more details.

* `samizdat-1` &mdash; *Samizdat Layer 1* interpreter.

  This is an interpreter for the *Samizdat Layer 1* language. See
  [the spec](doc/samizdat-1/README.md) and
  [the implementation's README](samizdat-1/README.md) for more details.

  *Samizdat Layer 1* is a strict superset of *Samizdat Layer 0*, adding
  new syntax in support of parser definition.

### Planned pieces

* `samizdat-2` &hellip; `samizdat-N` &mdash; *Samizdat Layers 2
  through N* interpreters.

  For `M` and `N` where `N == M + 1`, `samizdat-N` is written in the
  language implemented by *Samizdat Layer M*. `samizdat-N` accepts the
  language *Samizdat layer N*, translating it to the same underlying
  executable parse tree form that `samizdat-0` executes.

* `sam` &mdash; *Samizdat* interpreter.

  Tool that accepts input written in *Samizdat* (per se), and runs
  it.

  Written in *Samizdat Layer N* (for the ultimate value of `N`).

* `samc` &mdash; Tool to compile Samizdat source code.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  code to be linked and run with an associated library (details
  TBD).

  Written in *Samizdat*. Used to produce (at least) two executables:

  * Run using interpreted `sam` and given `sam`'s source as input, in
    order to produce a compiled version of `sam`.

  * Run using compiled `sam` and given `samc`'s source as input, in
    order to produce a compiled version of `samc`.

* `samizdat-0.sam` &mdash; *Samizdat Layer 0* interpreter.

  This is a maximally-reifying implementation of *Samizdat Layer 0*,
  meant to be both an "executable specification" for *Samizdat Layer
  0* (to help enable the implementation of *Samizdat Layer 0* in even
  yet other languages), as well as an implementation that can be
  relied upon for further iterative development of the other layers of
  the language implementation.

  Written in Samizdat. Run and compiled-and-run with `sam` and `samc`
  (respectively), for purposes of verification, validation, and
  further language iteration.

* ?

* Profit!
