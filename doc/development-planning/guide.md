Development Guide
=================

Editor Support
--------------

Find syntax highlighter packages for Atom and TextMate / SublimeText in the
[the etc/ directory](../../etc).

Prerequisites
-------------

Samizdat intentionally limits its prerequisites, as a simplifying tactic.

Producing a Samizdat executable, per se, requires two items of its
environment:

* A C compiler / linker. The variant of C used in Samizdat is C99 with modest
  use of extensions that are available in both Clang and the Gnu C compiler.

* A C library to link with. It expects to link with a Posix-compatible library
  which exports a few additional commonly-supported functions, including
  notably those for dynamic code loading.

The automated build for Samizdat requires these additional items:

* Standard Posix command-line utilities, such as (but not limited to) `cp`,
  `mkdir`, and `find`.

* The Bash scripting language, version 3.2 or later.

To be clear, the Samizdat build process is fairly straightforward, and
it is not be particularly hard to compile it "manually," should the need
arise.

Build Process
-------------

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

### Quick-turnaround partial build

Building just the "pure interpreter" runtime only takes a few seconds.
The downside to this build is that it takes somewhere in the neighborhood
of ten to fifteen seconds after starting, before it will read in a
program. Even so, it can be useful to run in many cases, such as when
making modifications to the core runtime.

To make this build, just run Blur in the directory `samex-naif`.

```shell
$ cd samizdat
$ blur --in-dir=samex-naif
[...]
$
```

### Combining building and demo running.

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
