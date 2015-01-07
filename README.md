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

[![Build Status](https://travis-ci.org/danfuzz/samizdat.svg?branch=master)](https://travis-ci.org/danfuzz/samizdat)

- - - - - - - - - -

Samizdat
========

This is the the Samizdat programming language, or "Sam" if you want to be
terse.

Samizdat is a high-level programming language somewhere down the
family lineage from all of [ALGOL](http://en.wikipedia.org/wiki/ALGOL),
[Lisp](http://en.wikipedia.org/wiki/LISP), and
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk). It is intended to be an
easy and compelling language to use for text processing, free-form data
structure manipulation, and general higher-level systems application
programming.

As a fundamental part of its philosophy, Samizdat eschews hidden state and
randomness, aiming for programs to be fully deterministic unless they
explicitly rely on external state (such as a filesystem or network link).
It also attempts to make immutable values the most attractive first choice for
data representation. This all makes Samizdat a "functional" language of sorts,
in the "Scheme camp" more than the "Haskell camp."

Find lots of documentation in [the doc/ directory](doc), including notably:

* a brief enumeration of [design highlights](doc/notes/design-highlights.md).
* an [overview](doc/development-planning/overview.md) of the main components
  of the system.
* a brief [development guide](doc/development-planning/guide.md).
* a [manifesto](doc/notes/manifesto.md) and a handful of
  [shout-outs](doc/notes/shout-outs.md) that may help understand where the
  system is coming from (both literally and figuratively).


TLDR Get Started
----------------

As of this writing, Samizdat is believed to build and run successfully
on OS X (compiled with Clang) and on normal-looking Linux distributions
(compiled with either Gcc or Clang).

```shell
## Get all set up.
$ git clone git@github.com:danfuzz/samizdat.git
$ cd samizdat
$ . env.sh

## Build samex-naif (the basic runtime).
$ blur --in-dir=samex-naif

## Build samex-tot (the somewhat better runtime), which takes much longer.
$ blur --in-dir=samex-tot

## Once a runtime is built, run a script.
$ samex path/to/script.sam

## Build everything from scratch, and run all tests.
$ ./demo/run-all --runtime=tot --compiler=simple --clean-build
```


TLDR Hello World
----------------

```
note("Hello, World!")
```


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
Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
Licensed AS IS and WITHOUT WARRANTY under the Apache License,
Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
```
