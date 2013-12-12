Samizdat Layer 0: Core Library
==============================

core::CommandLine
-----------------

The `CommandLine` module provides, unsurprisingly, command-line processing
functionality. For convenience, it exports `runCommandLine` to the default
global variable environment.

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `parseArgs(args) <> map`

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

#### `runCommandLine(args*) <> . | void`

Command-line evaluator. This implements standardized top-level command-line
parsing and evaluation. `args` are arbitrary arguments, which are parsed as
optional command-line options, a program file name, and additional arguments.

This parses the indicated file, as implied by its recognized suffix
(or lack thereof), evaluates the result of parsing, and then calls that
evaluated result as a function, passing it first the "componentized"
path to itself (see `Io1::pathFromFlat`), and then any additional
arguments that were passed to this function. This function returns whatever
was returned by the inner function call (including void).

Currently recognized command-line options:

* `--suffix=name` &mdash; Provides a file name suffix to use when interpreting
  the file name. Used to override the default of which language layer is
  used for parsing and evaluation. Must be `sam`, `samb`, or `samN` where `N`
  is a digit.
