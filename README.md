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

Find lots of documentation in [the doc/ directory](doc), including
a handful of [shout-outs](doc/notes/shout-outs.md) that may help understand
where the system is coming from (both literally and figuratively).


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


Development, Building and Testing
---------------------------------

### Editor Support

Find syntax highlighter packages for Atom and TextMate / SublimeText in the
[the etc/ directory](etc).

### Prerequisites

Samizdat intentionally limits its prerequisites, as a simplifying tactic.

Producing a Samizdat executable, per se, requires two items of its
environment:

* A C compiler / linker. The variant of C used in Samizdat is C99 with modest
  use of extensions that are available in both Clang and the Gnu C compiler.

* A C library to link with. It expects to link with a Posix-compatible library
  which exports a few additional commonly-supported functions, including
  notably those for dynamic code loading.

The automated build for Samizdat requires these additional items:

* The build makes use of standard Posix command-line utilities, such as
  (but not limited to) `cp`, `mkdir`, and `find`.

* The build makes use of the Bash scripting language, version 3.2 or later.

To be clear, the Samizdat build process is fairly straightforward, and
it is not be particularly hard to compile it "manually," should the need
arise.

### Build process

To build Samizdat from the console, run the
[Blur](https://github.com/danfuzz/blur) builder when cd'ed
to the root of the source directory. If you source (shell `.` command)
the file `env.sh`, then `blur` will be in your `PATH`. If not, then
`blur/blur` will work too.

```shell
$ git clone git@github.com:danfuzz/samizdat.git
[...]
$ cd samizdat
$ . env.sh
$ blur
[...]
$
```

As of this writing, the full build can take about ten to fifteen minutes on
mid-range personal computing hardware. Among its steps, the build consists of:

* Compilation of the core runtime, into an executable binary.
* Compilation of the Samizdat compiler, into a collection of loadable
  libraries. This is done by the Samizdat compiler itself, running using the
  baseline interpreter. The compiler emits C code, which is then compiled
  using the usual C compiler on the system.
* Compilation of the core library, into a collection of loadable
  libraries. This is done by the compiled Samizdat compiler, using C
  code as an intermediate form in the same manner as the previous item.

Once built, you can run `samex <path-to-script>`. `samex` and all the other
built binaries are deposited in the directory `out/final/bin`. If you used
`env.sh` this will be on your `PATH`.

You can also run the various demo / test cases, with the scripts
`demo/run <demo-number>` or `demo/run-all`. Demo numbers are of the form
`X-NNN` where `X` is a category and `NNN` is a sequence number. Each lives in
a directory named with its number suffixed with a suggestive summary, e.g.
`lib-001-bool`.

#### Quick-turnaround partial build

Building just the "pure interpreter" runtime only takes a few seconds.
The downside to this build is that it takes somewhere in the neighborhood
of 30 seconds to a minute after starting, before it will read in a
program. Even so, it can be useful to run in many cases, such as when
making modifications to the core runtime.

To make this build, just run Blur in the directory `samex-naif`.

```shell
$ cd samizdat
$ blur --in-dir=samex-naif
[...]
$
```

#### Combining building and demo running.

The options `--runtime=name --build` can be added to any demo `run` or
`run-all` command, to cause the named runtime to be built before running
the demo. Instead of `--build`, `--clean-build` causes the build to be
made from scratch. Supported names are `naif` (pure interpreter) and `tot`
(tree-compiled library).

In addition, the option `--compiler=name` can be added to any demo `run` or
`run-all` command, to cause the demo to be compiled with the named compiler
mode. And `--time` will cause the build / run to be timed. See `samtoc`
documentation for more info.

The "standard" shell command to build and run a full set of tests is:

```shell
$ cd samizdat
$ ./demo/run-all --runtime=tot --compiler=simple --clean-build --time
```

As of this writing, running this command takes about fifteen minutes on a
mid-range laptop of recent vintage.


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
