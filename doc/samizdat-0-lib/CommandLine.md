Samizdat Layer 0: Core Library
==============================

core::CommandLine
-----------------

The `CommandLine` module provides, unsurprisingly, command-line processing
functionality.

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

(none)


#### `run(args*) <> . | void`

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

* `--layer=N` &mdash; Indicates which layer to use for parsing and
  evaluation. `N` must be one of: `0` `1` `2`
