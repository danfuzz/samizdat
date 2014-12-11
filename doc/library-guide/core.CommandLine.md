Samizdat Layer 0: Core Library
==============================

core.CommandLine
----------------

The `CommandLine` module provides, unsurprisingly, command-line processing
functionality. For convenience, it exports `runCommandLine` to the default
global variable environment.

<br><br>
### Functions

#### `parseArgs(args) -> isa SymbolTable`

Simple argument parser. This accepts a list of strings, taken to be
command-line arguments, and parses them into options and other arguments,
using a simplified version of the usual Posix rules for parsing.

The result map binds `options` to a map of `--`-prefixed options, and `args`
to a list of non-option arguments. The `options` map binds option names to
either `true` for flags (e.g., `--flag-name`) or a string value if assigned
(e.g., `--name=value`). An optional plain `--` marks the end of options. An
argument of just `-` (a single dash) is taken to be a regular argument and
not an option.

**Note:** This function rejects options that start with only a single dash.

#### `runCommandLine(args*) -> . | void`

Command-line evaluator. This implements standardized top-level command-line
parsing and evaluation. `args` are arbitrary arguments, which are parsed as
optional command-line options, a program file name, and additional arguments.

This loads the indicated file or directory, and runs it. This function
returns whatever was returned by the run (including void).

If given a directory, this treats it as a module and expects there to be
a `main` file which defines the primary exports, including in particular
a `main` function. The `main` function is invoked, passing it one or more
arguments: first the path to the module, followed by the "additional
arguments" (beyond the ones understood directly by this function).

If given a plain file (and not a module directory), then that file gets
evaluated similarly to a module, though it won't have available to it any
submodules (that is, it won't get module-like access to sibling files or
directories). If the file exports a `main`, then that is run in the same
manner as a module's `main`. If it does not export a main, then after
evaluation this function simply returns void.

The two file suffixes recognized are `.sam` for Samizdat source text, and
`.samb` for compiled Samizdat executables.

Currently recognized command-line options:

* None.
