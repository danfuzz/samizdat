Samizdat Layer 0
================

This is an implementation of the Samizdat Layer 0 language.

- - - - -

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

* dat &mdash; "Low-layer" data model. This implements the four
  Samizdat low-layer data types, providing constructors, accessors,
  and a handful of assertions. Depends on util.

* io &mdash; I/O functions. This implements a minimal set of I/O
  operations. Depends on util and dat.

* lang &mdash; Language parsing and execution engine. This implements
  translation from source text to executable code trees, as well as
  the execution of same. This is also what implements the binding of
  primitive functions into execution contexts. But this module does
  not implement any primitive functionality itself. Depends on util
  and dat.

* prim &mdash; Primitive bindings. This implements the parts of the
  core library that need to be (or are most conveniently) implemented
  in C. Depends on util, dat, lang, and io.

* main &mdash; Where it all comes together. This implements the
  C `main()` function. Depends on everything else.
