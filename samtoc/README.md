Samtoc Compiler
===============

`samtoc` (that is, "SAMizdat TO C") is a compiler which takes individual
Samizdat source files, translating them into C code representing the same
source. When compiled with a C compiler, linked as a dynamic library, and
given the suffix `.samb` (that is, "SAMizdat Binary"), these files can be
loaded by any Samizdat runtime whose binary interface matches `samex-naif`.

`samtoc` can do full compilation to a library (as a development-time
convenience), or it can limit itself to producing C code. The
`compile-samex-addon` command, supplied as part of the `samex` wrapper
scripts, can be used to perform compilation of C code in the proper manner
(and is in fact what `samtoc` itself uses when asked to fully compile code).

As a command-line tool, `samtoc` accepts any number of files or directories
to process and takes the following options:

* `--help` &mdash; Emit a short help message.

* `--binary` &mdash; Compile all the way to a `.samb` binary addon library.
  This is only valid if a single source file is specified.

* `--core-dir=<dir-name>`, `--no-core-dir` &mdash; Indicates where to find the
  core library. This is what is searched when asked to import / resolve a
  module which is otherwise not found. This directory should be the "single
  module" directory of a core library or the `include` directory of a
  distribution. See below for details; e.g. it should point at
  `.../corelib` or `.../include/name` and *not* `.../corelib/modules` or
  `.../include/name/modules`.

  If `--no-core-dir` is specified, this suppresses any setting of the core
  library directory.

  If neither option is provided, this defaults to an appropriate directory for
  the runtime being compiled for.

* `--dir-selection` &mdash; Indicates that the file arguments take the form
  of (first argument) a directory whose contents are to be compiled, followed
  by (rest of the arguments) any number of files under the directory. Only
  the indicated files are processed. This option is meant to make it easier
  to implement partial module compilation in a build system.

  **Note:** In order to be recognized, the selection names must match
  the directory exactly as given, and must have no internal `.` or `..`
  components.

* `--external-dirs=<dir-name>:<dir-name>...` &mdash; Indicate what directories
  should be searched within when looking for external module linkage
  metainformation. These are searched after any captive modules directories
  and before the core library directory.

  Each listed directory should contain within it subdirectories each of which
  defines a module (with the subdirectory name as the module name), and/or
  contain module info files with names of the form
  `fully.qualified.name.saminfo`.

* `--in-dir=<dir-name>` &mdash; Indicate that all source files should be taken
  to be relative to the indicated directory.

* `--internal-dir=<dir-name>` &mdash; Indicate a directory which should be
  considered the base when looking for internal module linkage
  metainformation.

  This should be used when compiling a standalone module and naming its
  files explicitly (as opposed to just naming the directory). It indicates the
  base directory of the module being compiled. The contents of the directory
  are expected to be the source and resource files of the module's
  implementation.

* `--mode=<name>` &mdash; Specifies the compilation mode to use. See below.

* `--out-dir=<dir-name>` &mdash; Indicate that all output should be made
  under the indicated directory. Output names use the same relative paths as
  input names.

* `--output=<file-name>` &mdash; Specify the name of the output file. This is
  only valid if a single source file is given. `-` indicates that
  standard-output should be used.

* `--runtime=<name>` &mdash; Specifies which `samex` runtime to use. See
  `samex` docs for more details.

* `--` &mdash; Indicates the end of options. Any further arguments are taken
  to be file names.

Files (per se) are taken to be Samizdat source code. Directories are
expected to be in one of two forms:

* A single module implementation, consisting of:
  * A top-level `main.sam` file.
  * Zero or more helper internal sources, as siblings with `main.sam` or
    under arbitrary subdirectories.
  * An optional `modules` directory (in a form per the next bullet item).
    These are the "captive" external modules of the indicated single
    module.

* A `modules` directory, consisting of one or more directories that name
  external modules. Each such directory should take the form of a
  single module implementation (described in the previous bullet item).


Compilation Modes
-----------------

`samtoc` implements the following compilation modes (ordered from simplest
to most complicated):

### `--mode=interp-tree`

The result of compilation is a tree of function calls which reproduces the
parse tree form of the original source. When run, the tree is constructed
(using regular C function calls) and then evaluated using the same tree
evaluator used when interpreting code.

### `--mode=linkage`

The result of compilation is a `saminfo` file describing the input file's
exported characteristics as a module.

### `--mode=simple`

This is (approximately) the simplest possible "real" compilation of source.
The result is C code which *does not* rely on any parsing of text or tokens,
and which does not use an interpreter during execution (unless of course
the code it compiles contains calls to do the same, in which case it will
oblige accordingly).

This mode does not attempt to do any deep analysis of the code. In particular:

* It does not do anything clever with variable definition and reference.
  Every variable definition results in a "cell" (handle-like thing), allocated
  on the heap.

* It does not inline any library code. This means, for example, that almost
  all code creates "unnecessary" closures when run.

The main benefit of this mode is that it avoids the overhead of dispatching
to interpretation code for the various execution tree node types, as well as
the code to tease apart the structures of same. It also handles parsing
of incoming closure arguments much more efficiently than the interpreter
does, especially with regards to arguments that use repeat specifiers.
