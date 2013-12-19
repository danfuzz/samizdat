Samizdat Layer 0
================

This is an implementation of the Samizdat Layer 0 language, written in
C.

The goals of this implementation, in priority order, are:

* Correctness.
* Debugability.
* Readability.
* Smallness.
* Efficiency.

This program is written in C in a style which is meant to be
verifiably correct by inspection. That is, the implementation
prefers clarity and obviousness over trickiness and efficiency.

The language parser and runtime do the bare minimum of error
checking, attempting to fail fast in the face of any errors but not to
provide much in the way of meaningful messages.

On the topic of "debugability", the code is written to be totally
deterministic. For example, the addresses of structures are never
used for things like "identity hashes", and the C stack is *not*
scanned conservatively. (The GC has a conservative aspect to it,
but not due to confusion between ints and pointers.)


Directory and File Organization
-------------------------------

The code is structured into "modules", with each module's code in a
directory with the module's name, and with "exports" from that module (if
any) in a header file directly under `include/` bearing the module's
name. For example, the code of the `util` module is in the `util/` directory,
and the `util` module exports functionality as defined in `include/util.h`.
Some modules have additional headers under a directory named
`include/module`. These typically get included by `include/module.h` and
are not intended for direct consumption by other modules.

Several modules define in-language types. Instead of listed with the
module header, each type is declared in its own header, under the
directory `include/type`. For example, the `List` type is declared
in `include/type/List.h`.

Here's a run-down of the defined modules, in dependency order (with
later-named modules depending only on earlier-named ones):

* `util` &mdash; Very basic utility functions, including console
  messaging, Unicode conversion, and about the simplest memory
  allocation facility.

* `pb` &mdash; "Lowest-layer" data and plumbing model. This implements just
  enough of the model to be able to do function definition and calling.
  Also provides related utilities, such as assertions. Depends on `util`.

* `dat` &mdash; "Low-layer" data model. This implements the Samizdat
  low-layer data types not covered by `pb`, providing constructors, accessors,
  and a handful of assertions. Depends on `util` and `pb`.

* `const` &mdash; Commonly-used in-model constants and related utilities.
  This includes things like strings used during parsing, and the like.
  Depends on `util`, `pb`, and `dat`.

* `io` &mdash; I/O functions. This implements a minimal set of I/O
  operations. Depends on `util`, `pb`, `dat`, and `const`.

* `lang` &mdash; Language parsing and execution engine. This implements
  translation from source text to executable code trees, as well as
  the execution of same. This is also what implements the binding of
  primitive functions into execution contexts. This module does
  not implement any of the library itself. Depends on `util`, `pb`, `dat`,
  and `const`.

* `lib` &mdash; Library bindings. This implements both primitive and
  in-language bindings. The former are the parts of the core library
  that need to be (or are most conveniently) implemented in C. The
  latter are what can be implemented in Samizdat Layer 0. Depends on
  everything above it.

* `main` &mdash; Where it all comes together. This implements the
  C `main()` function. Depends on everything else.


Coding Conventions
------------------

### File Naming

Files that are primarily about a particular in-model type are given
the file name `TypeName.c`.

Other source files are given names that are meant to be indicative of their
purpose.

Within a module, a file named `impl.h` (if it exists) is an intra-module
"implementation detail" header file, which includes declarations for functions
and variables that are private to the module.

Header files named `something-def.h` (that is, `-def` suffix) consist
of "definitions" of some sort. Wherever these files are included, the
`#include` is preceded by per-file `#define`s to properly expand the
definitions contained in the file in question.

### File Length

Files of more than about 1,000 lines are generally considered to be
too long. The general aim is to split them into two or more files.
There are a couple exceptions, which unfortunately do not lend themselves
to splitting up.

### Intra-file Arrangement

Source files are generally split into sections. Within each section,
types and variables are typically listed before functions. The following is
the usual order for sections:

* Header &mdash; This starts with the standard copyright boilerplate and
  is followed by a comment that describes the general purpose of the
  file (usually a one-liner). After that are `#includes`, with `"local"`
  includes before `<system>` includes (separated by a blank line), and
  alphabetized in each subsection.

* Private code, labeled "Private Definitions" &mdash; This is all
  meant to be scoped totally to the file in which it occurs. Functions in
  this section are most typically listed in bottom-up (def before use) order.

* Intra-module exports, labeled "Module Definitions" &mdash; Definitions
  in this section should also have a corresponding declaration in the
  module's `impl.h` file. Functions in this section are most typically
  listed in alphabetical order.

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

### Spacing and Indentation

C code is formatted in a close approximation of the "One True" brace and
spacing style, with four spaces per indentation level.

Here are a couple snippets to indicate areas of potential ambiguity:

Function prototype continuation lines are double-indented to keep the
prototype and code visually separate:

```
void lotsOfParametersHere(zvalue arg1, zvalue arg2, zvalue arg3, zvalue arg4,
        zvalue arg5, zvalue arg6) {
    body();
    indented();
    here();
}
```

`switch` cases are indented a full level:

```
switch (something) {
    case THING1: {
        stuff();
        break;
    }
    case THING2: {
        stuff();
        break;
    }
}
```

Table-like code and other runs of similar lines are horizontally aligned:

```
DEF(blort,  POTION, "see in the dark");
DEF(borch,  SPELL,  "insect soporific");
DEF(fizmo,  SPELL,  "unclogs pipes");
DEF(ignatz, POTION, "unknown effect");
```

### Variable and Function Naming

Variable and function names use `lowercaseInitialCamelCase`. Structural
types use `UppercaseInitialCamelCase`. Other types use `zlowercaseInitial`
(that is, prefixed with `z` per se). Numeric constants and some special
valius use `ALL_CAPS_WITH_UNDERSCORES`.

A "Hungarianesque" name prefix is used to identify aspects of some variables
and functions:

* `TYPE_TypeName` &mdash; Identifies a value of type `Type`.

* `GFN_methodName` &mdash; Identifies a generic function (value of type
  `Generic`).

* `STR_NAME` &mdash; Identifies a value of type `String`.

* `TOK_NAME` &mdash; Identifies a transparent derived value whose type is
  `STR_NAME`.

* `theName` (that is, `the` as a prefix) &mdash; Identifies a variable as
  static (file scope).

* `MODULE_CONSTANT` (that is, `MODULE_` as a prefix) &mdash; Identifies the
  module to which a constant belongs.

* `moduleFunctionName` (that is, `module` as a prefix) &mdash; Identifies
  the module to which a function belongs.

* `typeFunctionName` (that is, `type` as a prefix) &mdash; Identifies the
  type to which a function applies, generally as its first argument.

Other naming conventions (which sometimes override the above):

* `targetFromSource` &mdash; Indicates a function that takes a value of
  type `Source` yielding a value of type `target`. These names are most
  often used for value conversion functions (that is, converting from one
  type to another, with no loss of data).

* `makeTypeName` &mdash; Indicates a function that creates a value of
  type `TypeName`. These are almost all for the creation of "identified"
  values (that is, values that have an identity beyond their underlying
  data).

* `assertSomething` &mdash; Indicates an assertion function of some sort.

* `assertTypeName` &mdash; Indicates an assertion function whose purpose
  is to assert that the argument is of the indicated type. Occasionally
  an additional suffix indicates some extra aspect being asserted (e.g.,
  `assertStringSize1`).

* `functionName0` (that is, `0` as a suffix) &mdash; Indicates a "helper"
  function for the function `functionName`. Rarely, other digits are used
  as well.

### Comments

Variables, functions, types, and elements of structures all have
documentation comments that start with `/**` as the usual indication
that they are intended to be "published" documentation.

Section and file banner comments take the form:

```
/*                            /*
 * Title Goes Here             * Title Goes Here
 */                    or      *
                               * Sometimes, additional notes will
                               * go here.
                               */
```

Most other comments, including multi-line comments, are `//`-prefixed.

Comment text is written using Markdown syntax, even when not marked `/**`.
Code samples are delimited with triple-backticks (Github flavor).

A general aim of commenting is to take a "DRY" attitude, with
specification files being the ultimate "source of truth" and header files
being more authoritative than (non-header) source files. Comments of the
form `/* Documented in header. */` and `/* Documented in spec. */`
are used liberally as an explicit indication that the so-marked item does
in fact have documentation elsewhere. (That is, it is an unintentional
oversight for an item to *not* have such a comment.)

### Macros

Macros are generally avoided, except for a specific couple of purposes:

* As standard-form guards around header files to prevent multiple inclusion.

* To help avoid particularly noisy boilerplate code, when no other means
  is available. For this purpose, they can be defined either in source or
  header files.

### Inline functions

Inline functions are generally avoided, except when they have a blatant
and measurable performance benefit, which can be achieved without making
the code significantly less clear.

Inlines are defined using the C99 standard idiom, namely:

* The primary definition of the inline function is located in a header
  file, marked just `inline` (and not `static`).

* One source (non-header) file contains a declaration of the same function
  with just `extern` (and not `inline`).

### External library usage

The code is meant to rely only on functionality which is either
defined by Posix or is nigh-ubiquitously available in Posix-providing
environments, without having to install additional packages.

The "gold standard" is that any given function be available in
both OS X and typical Linux userlands.
