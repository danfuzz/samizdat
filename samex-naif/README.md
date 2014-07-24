Samizdat Layer 0
================

This is an implementation of the Samizdat Layer 0 language, written in
C.

See the C Code conventions document for general information about the
style in which this code is written. The following adds a bit of flavor
to that.

### Debugability

On the topic of "debugability," the code is written to be totally
deterministic. For example, the addresses of structures are never
used for things like "identity hashes," and the C stack is *not*
scanned conservatively. (The GC has a conservative aspect to it,
but not due to confusion between ints and pointers.)

### Directory and File Organization

The code is structured into "modules," with each module's code in a
directory with the module's name, and with "exports" from that module (if
any) in a header file directly under `include/` bearing the module's
name. For example, the code of the `util` module is in the `util/` directory,
and the `util` module exports functionality as defined in `include/util.h`.
Some modules have additional headers under a directory named
`include/module`. These typically get included by `include/module.h` and
are not intended for direct consumption by other modules.

Several modules define in-language types / classes / protocols. Instead of
being listed with the module header, each type is declared in its own header,
under the directory `include/type`. For example, the `List` class is declared
in `include/type/List.h`.

Here's a run-down of the defined modules, in dependency order (with
later-named modules depending only on earlier-named ones):

* `util` &mdash; Very basic utility functions, including console
  messaging, Unicode conversion, and about the simplest memory
  allocation facility.

* `dat` &mdash; "Low-layer" data and plubming model. This implements all the
  low-layer data types as well as the subtrate for such stuff as allocation
  and function calling. Also provides related utilities, such as assertions.
  Depends on `util`.

* `const` &mdash; Commonly-used in-model constants and related utilities.
  This includes things like strings used during parsing, and the like.
  Depends on `util` and `dat`.

* `io` &mdash; I/O functions. This implements a minimal set of I/O
  operations. Depends on `util`, `dat`, and `const`.

* `lang` &mdash; Language parsing and execution engine. This implements
  translation from source text to executable code trees, as well as
  the execution of same. This is also what implements the binding of
  primitive functions into execution contexts. This module does
  not implement any of the library itself. Depends on `util`, `dat`,
  and `const`.

* `lib` &mdash; Library bindings. This implements both primitive and
  in-language bindings. The former are the parts of the core library
  that need to be (or are most conveniently) implemented in C. The
  latter are what can be implemented in Samizdat Layer 0. Depends on
  everything above it.

* `main` &mdash; Where it all comes together. This implements the
  C `main()` function. Depends on everything else.


### Coding Conventions

#### Intra-File Arrangement

In addition to the other sections mentioned in the general conventions
document, this implementation has some sections labeled "Class Definition."

These sections are for code which defines an in-model class. There will
usually be a corresponding declaration of `TYPE_Name` in *some* header
(which one, depending on how far the class is exported); rarely, a class is
totally private to the file, in which case its `TYPE_Name` is declared as
`static`. Method implementation functions in this section are most typically
listed in alphabetical order, with an overall binding / initialization
function toward the bottom.

#### Variable and Function Naming

Beyond the general conventions, the following are used:

* `TYPE_ClassName` &mdash; Identifies a value of class `Class`.

* `GFN_methodName` &mdash; Identifies a generic function (value of class
  `Generic`).

* `STR_stringValue` &mdash; Identifies a value of class `String` with content
  `"stringValue"`. For strings with contents not directly representable in
  identifiers, the name is designed to be suggestive of the content. In
  particular, short sequences of nonalphabetic characters usually have
  variable names of the form `STR_CH_CHARNAMES`.

* `TOK_tokenName` &mdash; Identifies a payload-free derived data
  value whose class is `TYPE_tokenName`, where the class's name is
  `"tokenName"`.

* `get_fieldName` &mdash; Identifies a function which acts as a getter.
  This is meant to parallel Samizdat's getter function syntax.
