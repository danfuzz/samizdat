NOTICE!
=======

This project is an incomplete work-in-progress.

* Much of the documentation is more aspirational than factual.
* There are tons of rough edges and *many* things missing from both the
  design and implementation.
* The code is aimed *way* more at ease-of-understanding and
  ease-of-modification than efficiency. Generated parser performance is
  particularly embarrassing.
* Nearly everything is subject to change.

- - - - - - - - - -

Samizdat
========

This is the source for the Samizdat programming language, or "Sam" if
you want to be terse.

Samizdat is a high-level programming language somewhere down the
family lineage from all of [ALGOL](http://en.wikipedia.org/wiki/ALGOL),
[Lisp](http://en.wikipedia.org/wiki/LISP), and
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk). It is intended that the
language be an attractive target for use in text processing, free-form data
structure manipulation, and general higher-level systems programming.

As a fundamental part of its philosophy, Samizdat eschews hidden state and
randomness, aiming for it to be the case that a given piece of code will run
completely identically on multiple different runs. It also aims for the use
of immutable data as an attractive first recourse. This all makes it a
"functional" language of sorts, in the "Scheme camp" moreso than the
"Haskell camp."

Find lots of documentation in [the doc/ directory](doc), including notably:

* a brief [development guide](doc/development-planning/guide.md)
* a handful of [shout-outs](doc/notes/shout-outs.md) that may help understand
  where the system is coming from (both literally and figuratively).


TLDR Get Started
----------------

```shell
## Get all set up.
$ git clone git@github.com:danfuzz/samizdat.git
$ cd samizdat
$ . env.sh

## Build and run everything.
$ ./demo/run-all --runtime=tot --compiler=simple --clean-build --time

## Once built, run a script.
$ samex path/to/script.sam
```


Overview
--------

The following is an overview of the pieces of the system that currently
exist, though all are still incomplete works-in-progress.

* `samex-naif` &mdash; Simple interpreter, written in C.

  This is an interpreter for Layer 0 of the language, which can be paired
  with a companion library to interpret Layer 1 and Layer 2 as well.

  See [the language spec](doc/language-guide) and
  [the implementation's README](samex-naif/README.md) for more details.

* `samlib-naif` &mdash; In-language core library for Layers 0&ndash;2.

  This is an implementation of the non-primitive portion of the
  core library, sufficient for running code written in Layers 0&ndash;2.

  See [the library spec](doc/library-guide) and
  [the implementation's README](samlib-naif/README.md) for more details.

* `samex-tot` &mdash; Runtime build with "compiled" core library.

  This is the same code as `samex-naif` and `samlib-naif`, except that most
  of the library code has been compiled into a very simple binary form,
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

* `samtoc` &mdash; Simple compiler to C, written in Layer 2.

  This is a compiler that accepts Layer 2, producing C source as output,
  which when compiled is suitable for loading as binary library files
  (modules or standalone binaries) by `samex-naif` or `samex-tot`.

  It has a handful of different compilation modes, of varying sophistication.

  See [the implementation's README](samtoc/README.md) for more details.


More Info
---------

### Authors and Acknowledgments

See [AUTHORS.md](AUTHORS.md).

### Changelog

See [CHANGELOG.md](CHANGELOG.md).

### Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

### Copyright and License

See [LICENSE.md](LICENSE.md).

```
Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
Licensed AS IS and WITHOUT WARRANTY under the Apache License,
Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
```
