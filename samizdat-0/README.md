Samizdat Layer 0
================

This is an implementation of the *Samizdat Layer 0* language, written in
C.

The goals of this implementation, in priority order, are:

* Correctness.
* Debugability.
* Readability.
* Smallness.
* Efficiency.

This program is written in C in a style which is meant to be
verifiably correct by inspection. That is, the implementation
prefers clarity and obviousness over trickiness and efficiency. In
addition, it uses a minimum of C library functionality and eschews
the use of macros *except* as guards on header files and to
reduce noisy boilerplate.

The language parser and runtime do the bare minimum of error
checking, attempting to fail fast in the face of any errors but not to
provide much in the way of meaningful messages.


Structure
---------

The code is structured into "modules", with each module's code in a
directory with the module's name, and with "exports" from that module
in a top-level header file bearing the module's name. For example,
the code of the "dat" module is in the `dat/` directory, and the
"dat" module exports functionality as defined in `dat.h`.

Here's a run-down of the defined modules, in dependency order (with
later-named modules depending only on earlier-named ones):

* util &mdash; Very basic utility functions, including console
  messaging, Unicode conversion, and about the simplest memory
  allocation facility.

* pb &mdash; "Lowest-layer" data and plumbing model. This implements just
  enough of the model to be able to do function definition and calling.
  Also provides related utilities, such as assertions. Depends on util.

* dat &mdash; "Low-layer" data model. This implements the various
  Samizdat low-layer data types, providing constructors, accessors,
  and a handful of assertions. Depends on pb and util.

* const &mdash; Commonly-used in-model constants and related utilities.
  This includes things like strings used during parsing, and the like.
  Depends on util, pb, and dat.

* io &mdash; I/O functions. This implements a minimal set of I/O
  operations. Depends on util, pb, dat, and const.

* lang &mdash; Language parsing and execution engine. This implements
  translation from source text to executable code trees, as well as
  the execution of same. This is also what implements the binding of
  primitive functions into execution contexts. This module does
  not implement any of the library itself. Depends on util, pb, dat,
  and const.

* lib &mdash; Library bindings. This implements both primitive and
  in-language bindings. The former are the parts of the core library
  that need to be (or are most conveniently) implemented in C. The
  latter are what can be implemented in Samizdat Layer 0. Depends on
  everything above it. It also bundles in code from the parallel directory
  `samizdat-0-lib`.

* main &mdash; Where it all comes together. This implements the
  C `main()` function. Depends on everything else.


Coding Conventions
------------------

C code is formatted in a close approximation of the "One True" brace and
spacing style, with four spaces per indentation level. Variable and function
names use `lowercaseInitialCamelCase`. Structural types use
`UppercaseInitialCamelCase`. Numeric constants use `ALL_CAPS_WITH_UNDERSCORES`.

A "Hungarianesque" prefix is used to identify aspects of some globals:

* `TYPE_TypeName` &mdash; Identifies a value of type `Type`.

* `GFN_methodName` &mdash; Identifies a generic function (value of type
  `Generic`).

* `STR_NAME` &mdash; Identifies a value of type `String`.

* `TOK_NAME` &mdash; Identifies a transparent derived value whose type is
  `STR_NAME`.

* `theName` (that is `the` as prefix) &mdash; Identifies a variable as
  static (file scope).

* `MODULE_CONSTANT` &mdash; Identifies the module to which a constant
  applies.

* `moduleFunctionName` &mdash; Identifies the module to which a function
  belongs.

* `typeFunctionName` &mdash; Identifies the type to which a function applies,
  generally as its first argument. As an exception, the name pattern
  `targetFromSource` generally indicates a function that takes a value of
  type `Source` yielding a value of type `target`.

Source files are generally split into sections. Within each section,
types and variables are typically listed before functions. The following is
the usual order for sections:

* Private code, labeled something "Helper Definitions" &mdash; This is all
  meant to be scoped totally to the file in which it occurs. Functions in
  this section are most typically listed in bottom-up (def before use) order.

* Intra-module exports, labeled "Module Definitions" &mdash; Definitions
  in this section should also have a corresponding declaration in a file
  called `impl.h` in the same directory. Functions in this section are most
  typically listed in alphabetical order.

* Public exports, labeled "Exported Definitions" &mdash; This is any
  code which is expected to be accessed directly by other modules. Definitions
  in this section should also have a corresponding declaration in a top-level
  (one directory up) file named `module.h`, where `module` is the name of
  the module. Functions in this section are most typically listed in
  alphabetical order.

* Type definition code, labled "Type Definition" &mdash; This is code
  which defines an in-model type. There will usually be a corresponding
  declaration of `TYPE_Name` in *some* header (which one, depending on how
  far the type is exported); rarely, a type is totally private to the file,
  in which case its `TYPE_Name` is declared as `static`. Method implementation
  functions in this section are most typically listed in alphabetical order,
  with an overall binding / initialization function toward the bottom.
