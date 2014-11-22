Overview of Existing Pieces
===========================

The following is an overview of the pieces of the system that currently
exist, though all are still incomplete works-in-progress.

* `samex-naif` &mdash; Simple interpreter, written in C.

  This is an interpreter for Layer 0 of the language, which can be paired
  with a companion library to interpret Layer 1 and Layer 2 as well.

  See [the language spec](../language-guide) and
  [the implementation's README](../../samex-naif/README.md) for more details.

* `samlib-naif` &mdash; In-language core library for Layers 0&ndash;2.

  This is an implementation of the non-primitive portion of the
  core library, sufficient for running code written in Layers 0&ndash;2.

  See [the library spec](../library-guide) and
  [the implementation's README](../../samlib-naif/README.md) for more details.

* `samex-tot` &mdash; Runtime build with "compiled" core library.

  This is the same code as `samex-naif` and `samlib-naif`, except that most
  of the library code has been compiled into a very simple binary form,
  where the code merely reconstructs interpretable trees (thus avoiding
  the overhead of parsing).

  This is built by running `samtoc` in "tree" mode over the `samlib-naif`
  source files.

* `samex` &mdash; Wrapper that dispatches to an appropriate runtime.

  This is a wrapper script which can dispatch to specific runtime versions
  (e.g. and i.e. `samex-naif` or `samex-tot`). The point is to keep
  version names out of other utility scripts.

* `compile-samex-addon` &mdash; Wrapper for the C compiler, to compile
  "addon" library code.

  This script knows how to call the C compiler with appropriate arguments
  for building "addon" libraries, which can subsequently be loaded by
  `samex-naif`.

* `samtoc` &mdash; Simple compiler to C, written in Layer 2.

  This is a compiler that accepts Layer 2, producing C source as output,
  which when compiled is suitable for loading as binary library files
  (modules or standalone binaries) by `samex-naif` or `samex-tot`.

  It has a handful of different compilation modes, of varying sophistication.

  See [the implementation's README](../../samtoc/README.md) for more details.

