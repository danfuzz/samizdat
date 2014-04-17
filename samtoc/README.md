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

As a command-line tool, `samtoc` accepts any number of files to process and
takes the following options:

* `--help` &mdash; Emit a short help message.

* `--binary` &mdash; Compile all the way to a `.samb` binary addon library.
  This is only valid if a single source file is specified.

* `--output=<file-name>` &mdash; Specify the name of the output file. This is
  only valid if a single source file is given. `-` indicates that
  standard-output should be used.

* `--in-dir=<dir-name>` &mdash; Indicate that all source files should be taken
  to be relative to the indicated directory.

* `--out-dir=<dir-name>` &mdash; Indicate that all output should be made
  under the indicated directory. Output names use the same relative paths as
  input names.

* `--mode=<name>` &mdash; Specifies the compilation mode to use. See below.

* `--` &mdash; Indicates the end of options. Any further arguments are taken
  to be file names.


Compilation Modes
-----------------

`samtoc` implements the following compilation modes (ordered from simplest
to most complicated):

### `--mode=interp-text`

The result of compilation is the original source text embedded in
a string in the output. When run, the string is passed to the parser and
then evaluated in a manner identical to how the file would have been procesed
had it not been "compiled" at all.

### `--mode=interp-tree`

The result of compilation is a tree of function calls which reproduces the
parse tree form of the original source. When run, the tree is constructed
(using regular C function calls) and then evaluated using the same tree
evaluator used when interpreting code.

The salient difference between this mode and `text` mode is that this mode
avoids the runtime steps of string tokenization and parsing a stream of tokens
into a parse tree, as these steps are taken at "compile" time.

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
