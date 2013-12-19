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

* `samex-naif` &mdash; Simple interpreter, written in C.

  This is an interpreter for *Layer 0* of the language, which can be paired
  with a companion library to interpret *Layer 1* and *Layer 2* as well.

  See [the spec](doc/language-guide) and
  [the implementation's README](samizdat-0/README.md) for more details.

* `samlib-naif` &mdash; In-language core library for *Layers 0&mdash;2*.

  This is an implementation of the non-primitive portion of the
  core library, sufficient for running code written in
  *Layers 0&ndash;2*.

  See [the spec](doc/language-guide) and
  [the implementation's README](samizdat-0/README.md) for more details.

* `samex-tot` &mdash; Runtime build with "compiled" core library.

  This is the same code as `samex-naif` and `samlib-naif`, except that
  the library code has been all compiled into a very simple binary form,
  where the code merely reconstructs interpretable trees (thus avoiding
  the overhead of parsing).

  This is built by running `samtoc` in "tree" mode over most (but not all)
  of the `samlib-naif` sources.

* `samex` &mdash; Wrapper that dispatches to an appropriate runtime.

  This is a wrapper script which can dispatch to specific runtime versions
  (e.g. and i.e. `samex-naif` or `samex-tot`). The point is to keep
  version names out of other utility scripts.

* `compile-samex-addon` &mdash; Wrapper for the C compiler, to compile
  "addon" library code.

  This script knows how to call the C compiler with appropriate arguments
  for building "addon" libraries, which can subsequently be loaded by
  `samex-naif`.

### Pieces in progress

* `samtoc` &mdash; Simple compiler to C, written in *Layer 2*.

  This is a compiler that accepts *Layer 2*, producing C source as output,
  which is suitable for loading as binary library files (modules or
  standalone binaries) by `samex-naif`.

  It has a couple different compilation modes, of varying sophistication.

### Planned pieces

* `samizdat-3` &hellip; `samizdat-N` &mdash; *Samizdat Layers 3
  through N* interpreters.

  For `M` and `N` where `N == M + 1`, `samizdat-N` is written in the
  language implemented by *Samizdat Layer M*. `samizdat-N` accepts the
  language *Samizdat layer N*, translating it to the same underlying
  executable parse tree form that is specified by *Layer 0* and
  executed by `samizdat-naif`.

* `samex-deft` &mdash; *Samizdat* interpreter.

  Tool that accepts input written in *Samizdat* (per se), and runs
  it.

  Written in *Samizdat Layer N* (for the ultimate value of `N`).

* `samtoc-solo` &mdash; Tool to compile Samizdat source code.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  code to be linked and run with an associated library (details
  TBD).

  Written in *Samizdat*. Used to produce (at least) two executables:

  * Run using interpreted `sam` and given `sam`'s source as input, in
    order to produce a compiled version of `sam`.

  * Run using compiled `sam` and given `samc`'s source as input, in
    order to produce a compiled version of `samc`.

* `samizdat-naif.sam` &mdash; *Samizdat Layer 0* interpreter.

  This is a maximally-reifying implementation of *Samizdat Layer 0*,
  meant to be both an "executable specification" for *Samizdat Layer
  0* (to help enable the implementation of *Samizdat Layer 0* in even
  yet other languages), as well as an implementation that can be
  relied upon for further iterative development of the other layers of
  the language implementation.

  Written in Samizdat. Run and compiled-and-run with `samex` and `samtoc-solo`
  (respectively), for purposes of verification, validation, and
  further language iteration.

* ?

* Profit!
