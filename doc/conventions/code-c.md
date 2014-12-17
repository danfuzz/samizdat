Conventions, Decisions, and Guidelines
======================================

Code: C
-------

C code in this project should be written to satisfy the following goals,
in priority order:

* Correctness.
* Debugability.
* Readability.
* Smallness.
* Efficiency.

C code should be verifiably correct by inspection, to the extent possible.
That is, clarity and obviousness are preferred over trickiness and efficiency.

C code should also aim to be totally deterministic, to aid in debugability.

[This section is a mildly edited version of the description of the style
of the `samex-naif` codebase, and could use some editing to make it more
proscriptive.]

### File Organization

Groups of related files should be placed together in "module" directories.
Each module should have an `#include` file accessible to other modules.

### File Naming

Files that are primarily about a particular type are given the file name
`TypeName.c`.

Other source files should be given names that are meant to be indicative of
their purpose.

Within a module, a file named `impl.h` (if it exists) is an intra-module
"implementation detail" header file, which includes declarations for functions
and variables that are private to the module.

Header files named `something-def.h` (that is, `-def` suffix) consist
of "definitions" of some sort. Wherever these files are included, the
`#include` is preceded by per-file `#define`s to properly expand the
definitions contained in the file in question.

### Intra-file Arrangement

Source files are generally split into sections. Within each section,
types and variables are typically listed before functions. The following is
the usual order for sections:

* Header &mdash; This starts with the standard copyright boilerplate and
  is followed by a comment that describes the general purpose of the
  file (usually a one-liner). After that are `#includes`, with `<system>`
  includes before `"whole-program"` before `"local"` includes, each subsection
  separated by a blank line, and alphabetized within each subsection.

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

### Spacing and Indentation

C code is formatted in a close approximation of the "One True" brace and
spacing style, with four spaces per indentation level.

Here are a couple snippets to indicate areas of potential ambiguity:

#### Table-Like Code

Table-like code and other runs of similar lines are horizontally aligned:

```c
DEF(blort,  POTION, "see in the dark");
DEF(borch,  SPELL,  "insect soporific");
DEF(fizmo,  SPELL,  "unclogs pipes");
DEF(ignatz, POTION, "unknown effect");
```

#### Function Prototypes

Function prototype continuation lines are double-indented to keep the
prototype and code visually separate:

```c
void lotsOfParametersHere(zvalue arg1, zvalue arg2, zvalue arg3, zvalue arg4,
        zvalue arg5, zvalue arg6) {
    body();
    indented();
    here();
}
```

#### Switch Statements

`switch` cases are indented a full level, and case bodies are surrounded
with braces.

```c
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

If multiple `case` labels apply to the same code, then it is okay to list
them on the same line, wrapping to multiple lines if needed.

```c
switch (something) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': {
        iAmDecimal();
    }
    case 'a': case 'b': case 'c': case 'd': case 'e':  case 'f': {
        iAmHex();
    }
}
```

If a series of `case`s have a parallel structure and would fit on a single,
line, then it is acceptable to format them in a "table-like" manner. Braces
are still required.

```c
switch (something) {
    case THING1:         { doOneThing();      break; }
    case THING2:         { doAnotherThing();  break; }
    case SEVERAL_THINGS: { doSeveralThings(); break; }
    default: {
      die("Unknown how to deal with this particular thing");
    }
}
```

#### Inline structs

Inline structs should be written *without* spaces just inside the braces,
e.g.:

```c
return stringFromZstring((zstring) {size, array});
```

### Variable and Function Naming

Variable and function names use `lowercaseInitialCamelCase`. Structural
types use `UppercaseInitialCamelCase`. Other types use `zlowercaseInitial`
(that is, prefixed with `z` per se). Numeric constants and some special
valius use `ALL_CAPS_WITH_UNDERSCORES`.

A "Hungarianesque" name prefix is used to identify aspects of some variables
and functions:

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
  type `TypeName`.

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

Other comments, including multi-line comments, are `//`-prefixed.

Section and file banner comments take the form:

```c
//                            //
// Title Goes Here            // Title Goes Here
//                     or     //
                              // Sometimes, additional notes will
                              // go here.
                              //
```
Comment text is written using Markdown syntax, even when not marked `/**`.
Code samples are delimited with triple-backticks (GitHub flavor).

A general aim of commenting is to take a "DRY" (Don't Repeat Yourself)
attitude, with specification files being the ultimate "source of truth" and
header files being more authoritative than (non-header) source files.
Function header comments of the form `// Documented in header.` and
`// Documented in spec.` are used liberally as an explicit indication
that the so-marked item does in fact have documentation elsewhere. (That is,
it is an unintentional oversight for an item to *not* have such a comment.)

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

#### System headers

It is acceptable to define the "feature test macro" `_XOPEN_SOURCE` to have
the value `700`, before `#include`ing a system header file, in cases where
doing so would enable some required / desired functionality. When doing so,
it is important to note in a comment why the macro is being defined.

For example:

```c
// Required for `lstat()` and `readlink()` when using glibc.
#define _XOPEN_SOURCE 700

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
```

Similarly, if absolutely necessary, it is acceptable to define `_GNU_SOURCE`.
As of this writing, the only known reason to do that is to guarantee proper
access to the `*asprintf` library functions.
