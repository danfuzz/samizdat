Samizdat Layer 0
================

This is an implementation of the *Samizdat Layer 0* language, written in
C.

The goals of this implementation, in priority order, are:

* Correctness.
* Debugability.
* Readability.
* Smallness.
* Efficiency.

This program is written in C in a style which is meant to be
verifiably correct by inspection. That is, the implementation
prefers clarity and obviousness over trickiness and efficiency. In
addition, it uses a minimum of C library functionality and eschews
the use of macros *except* as guards on header files and to
reduce noisy boilerplate.

The language parser and runtime do the bare minimum of error
checking, attempting to fail fast in the face of any errors but not to
provide much in the way of meaningful messages.


Structure
---------

The code is structured into "modules", with each module's code in a
directory with the module's name, and with "exports" from that module
in a top-level header file bearing the module's name. For example,
the code of the "dat" module is in the `dat/` directory, and the
"dat" module exports functionality as defined in `dat.h`.

Here's a run-down of the defined modules, in dependency order (with
later-named modules depending only on earlier-named ones):

* util &mdash; Very basic utility functions, including console
  messaging, Unicode conversion, and about the simplest memory
  allocation facility.

* dat &mdash; "Low-layer" data model. This implements the various
  Samizdat low-layer data types, providing constructors, accessors,
  and a handful of assertions. Depends on util.

* const &mdash; Commonly-used in-model constants and related utilities.
  This includes things like strings used during parsing, and the like.
  Depends on util and dat.

* io &mdash; I/O functions. This implements a minimal set of I/O
  operations. Depends on util, dat, and const.

* lang &mdash; Language parsing and execution engine. This implements
  translation from source text to executable code trees, as well as
  the execution of same. This is also what implements the binding of
  primitive functions into execution contexts. This module does
  not implement any of the library itself. Depends on util, const, and dat.

* lib &mdash; Library bindings. This implements both primitive and
  in-language bindings. The former are the parts of the core library
  that need to be (or are most conveniently) implemented in C. The
  latter are what can be implemented in Samizdat Layer 0. Depends on
  util, dat, const, io, and lang. It also bundles in code from the
  parallel directory `samizdat-0-lib`.

* main &mdash; Where it all comes together. This implements the
  C `main()` function. Depends on everything else.
