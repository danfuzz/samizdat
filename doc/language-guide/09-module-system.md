Samizdat Language Guide
=======================

Module System
-------------

### Overview

Samizdat has a simple and straightforward module system, meant to enable
a few useful patterns of code combination, without getting too much in the
way of things.

### What is a module?

A module in Samizdat is a collection of code which exports a set of
named bindings. Other code can import a module's bindings individually
or en masse.

In terms of code in files, a module consists of a directory which contains
at least one file, called `main`. The `main` file is responsible for
exporting whatever it is that the module exports.

The module directory optionally contains additional "intra-module" files
in an arbitrary subdirectory hierarchy. Intra-module files may be either
internal modules (single files of code) or resources.

In addition, the module directory optionally contains additional *external*
module definitions (which take the form as external modules in other contexts).
Each external module definition is in a subdirectory named
`modules/fully.qualified.name`, where `fully.qualified.name` is the name of
the module.

In Samizdat, all sorts of code is wrapped up into modules. In particular:

* An application is a module, which often defines other modules within itself.
* The core library is a module, which defines the recognizable library
  modules (such as `core.Format`) within itself.

### Naming

External modules are referred to by their "fully-qualified" name. A
fully-qualified name consists of a series of one or more components (each
component adhering to the same syntax as identifiers / variable names),
separated by dots (`.`). There are a couple of conventions for these names:

* The initial components of a module name start with a lower case letter.
* The final component of a module name is generally capitalized.
* Modules that are not independently published (e.g. are only used within
  a particular application) may have a name that consists of just a single
  capitalized component.
* All independently published modules should have names with at least two
  components (with details as follows).
* All regular implementation modules in the core library have two components,
  with first being `core`. For example, `core.Format`.
* All protocol modules in the core library have two components, with the
  first being `proto`. For example, `proto.Collection`.
* All modules that are part of a published library should have a first
  component that names the library in a manner that is intended to be concise
  but unambiguous. If the library has no interesting structure, then it should
  have a second component that is identical to the first, except that the
  first letter is capitalized. For example, `funLib.FunLib` (a module with
  no further structure) or `funLib.Constants` (a module with structure).

Internal modules and resource files are named as partial paths, with
components separated by slashes (`/`), and always introduced with a dot-slash
(`./`). Unlike normal (Posix-like) filesystem paths, components must adhere
to identifier syntax, except for the last component of a resource, which may
optionally take the form of a pair of identifiers separated by a dot (`.`).

By convention, the final component of internal module names and resource
files starts with a lower-case letter.

When referring to a module or resource in code, it is typical for
the bindings to be referenced as a variable whose name matches the final
component of the module, prefixed with a dollar sign (`$`). For example,
the module `core.Format` would typically be referred to as `$Format` in
code, and the resource file file `./etc/template.txt` would be referred to as
`$template`.

### Importing

A top-level module definition is allowed to import a few different things.
Import statements begin with an `import` keyword; beyond that, the syntax
depends on what is being imported.

TODO: The syntax for `import` is complete, but there is no semantic
implementation, yet.

#### Full module import

To import a module as a whole, indicate the name of the module after the
`import` keyword, optionally preceded by the name of a variable to bind it to
and `=`. If no name is supplied, the default is the final component name of
the module, prefixed with a dollar sign (`$`). With this form of import, the
bound variable can be used as a map to refer to the module's individual
bindings.

Examples:

```
## Default variable name, external module.
import core.Format;
def x = $Format::source("frobozz");

## Explicit variable name, external module.
import fmt = core.Format;
def x = $Format::source("frobozz");

## Default variable name, internal module.
import ./aux/utils;
def x = $utils::doSomething("frobozz");
```

#### Module selection import

It is possible to import some or all of the bindings of another module
directly as variables in the module being defined. This is done by
adding a selection specifier after the module name in an `import` statement.

A selection specifier consists of a double-colon (`::`), followed by either
a star (`*`) to indicate a wildcard import of all bindings, or by a
comma-delimited list of names to import.

If a name to bind is specified, then it is treated as a *prefix*, and it
must be followed by a `*`, which mnemonically indicates what's being done.
If no name is specified, then there is no prefixing done.

Examples:

```
## Import `blort`, `frotz`, and `quaff`, no prefix.
import zork.potions :: blort, frotz, quaff;
quaff(blort);
quaff(frotz);

## Import `blort`, `frotz`, and `quaff`, with a prefix.
import pot_* = zork.potions :: blort, frotz, quaff;
pot_quaff(pot_blort);
pot_quaff(pot_frotz);

## Assuming the same exports as above, do a wildcard import, no prefix.
import zork.potions :: *;
quaff(blort);
quaff(frotz);

## Assuming the same exports as above, do a wildcard import, with a prefix.
import pot_* = zork.potions :: *;
pot_quaff(pot_blort);
pot_quaff(pot_frotz);
```

#### Resource import

Resources are arbitrary files that are bundled with a module. Resources
can only be defined as internal files. (That is, there is no such thing as
an external resource, per se.)

The syntax for importing resources is similar to that of importing internal
modules, except that the type of file &mdash; that is, how to interpret the
contents &mdash; must be specified before the resource name, in the form
`@typeName` (similar to a payload-free derived value).

As with the other imports, an explicit variable name to bind is optional.
If no name is supplied, then the default is the final component name of
the resource, minus any extension.

The following types are understood by the system:

* `@utf8` &mdash; Interpret the resource file as UTF-8 encoded text.

Examples:

```
## Default name.
import @utf8 ./files/template.txt;
note($template);

## Explicit name.
import text = @utf8 ./files/template.txt;
note($template);
```

### Exporting

A top-level module definition can indicate bindings to export by using the
`export` keyword.

The basic form is just `export varName`, to cause the contents of the variable
named `varName` to be exported from the module, bound to the key `"varName"`.
**Note:** The thing that is exported is the value of a variable, and not
the "cell" containing a variable. So, it is generally a bad idea to export
a variable that was defined with `var` and not `def`.

As a conveniend short-hand, `export` may be used as a prefix on immutable
variable definitions and function definitions (both regular and generic
functions) to export the variables so-defined.

Examples:

```
## Export a previously-defined variable.
def blort = ...;
export blort;

## Define and export a variable, together.
export def blort = ...;

## Define and export a regular function, together.
export fn blort() { ... };

## Define and export a generic function, together.
export fn .blort() { ... };
```

### Finding a module

When code of a module asks to import another module, the system uses the
"loading heritage" of the requesting module in order to figure out what
to actually load.

To avoid confusion, in the following discussion, we will talk about a
module `ModA` which is trying to load a module `ModB`.

`ModA` itself was loaded by some module loader. In the case where `ModA`
is an application, then it doesn't have any "sibling" modules. In the
case where `ModA` is a library module, then the other modules of the
library are its "sibling" modules.

The first step in loading `ModB` is to look at the modules defined
by `ModA`, per se. That is, `ModA` may itself contain a module library.
In terms of directory hierarchy, if `ModA` is in `/x/y/modules/ModA`, then the
system will look for `/x/y/modules/ModA/modules/ModB`.

If `ModA` doesn't define `ModB` directly, then the system will look
for a sibling definition of `ModB` if applicable (that is, if `ModA` is
part of a library). In terms of directory hierarchy, if `ModA` is in
`/x/y/modules/ModA`, then the system will look for `/x/y/modules/ModB`.

If the sibling search fails (or wasn't applicable), then the next loader
to be checked is the one which was "in scope" when `ModA`'s module loader was
created. If that fails, then the loader scope is unwrapped once again, and
so on, until the core library's module loader is consulted.

If, having exausted all other possibilities, the core library's loader fails
to find `ModB`, then the system declares that `ModB` has no definition, and
the runtime terminates with an error.

Once a module has been found and loaded, the loader (or loaders) that
were used in the process note the result of loading. If asked to re-load the
same module, they simply return the previously-stored value.

### Pedantic details

Most of the description in this section is meant to be an "in practice"
outline of what the module system looks like. As such, it elides over a
few details, which are discussed in this section.

#### Module loading

The implementation of module loading actually much simpler than the
description might have you believe.

There are two types which interplay to cause module loading to happen.

One type is `ModuleLoader`, which gets instantiated with two main
pieces of information, (a) a filesystem path to a directory containing
module definitions, and (b) a reference to the "next" `ModuleLoader` to
use. `ModuleLoader` defines a `moduleLoad` method, which is the thing that
looks for a module in its designated directory, and then calls on the
"next" loader if that fails. The recursion bottoms out in a definition of
`moduleLoad` on `null`, which always fails.

The other type is `IntraLoader`, which gets instantiated with two pieces
of information, (a) a filesystem path to a directory containing the definition
of *one* module, and (b) a reference to the "next" `ModuleLoader` to
use. When instantiated, `IntraLoader` makes a `ModuleLoader` which points
to a `modules` directory under its given filesystem path, and that
`ModuleLoader` is the one that's used directly by the intra-module code.
The `IntraLoader`'s filesystem path is used directly for intra-module files.

The core library is loaded as an `IntraLoader`, as are application modules.
In the case of an application module, its "next" loader is the core library.

As a final note, though the default module system is implemented in terms
of the filesystem, all of the behavior of the system is based on generic
functions. These functions can be bound to other types, in order to
provide other interesting and useful arrangements. For example, it is
possible (and may eventually be desirable) to construct a module or
intra-module loader which only depends on immutable data as input.

#### Example filesystem layout

```
/path/to/castingApp
  main.sam                   application's main file
  appHelp.sam                intra-module file for the applicaton itself
  modules/
    Blort/                   application's `Blort` module
      main.sam
      darkness.sam           intra-module file for `Blort` module
    Frotz/                   application's `Frotz` module
      main.sam
      modules/
        Fizmo/               private `Fizmo` module used by `Frotz`
          main.sam
        Igram/               private `Igram` module used by `Frotz`
          main.same
```

#### Top-level variable environment in a module

In addition to the normal global variable environment as defined by the
module `core.Globals`, a module file when evaluated has additional bindings
to allow for a module to load the other parts of itself, as well as load
other modules.

* `moduleLoad(path) <> module` &mdash; This loads the module indicated
  by `path` in the same loader which loaded this module. `path` is a
  "module path," that is, a list of strings which represents the fully-
  qualified name of the module. For example, `["core", "Lang0"]`.

* `moduleLoader` &mdash; This is a reference to the module loader which
  loaded this module.

* `intraLoad(path) <> . | void` &mdash; This loads and evaluates the
  intra-module file as code. `path` is a string representing a relative
  filesystem path, e.g. `foo/bar`. The final file name in `path` should
  *not* have a suffix; the module system handles finding the
  appropriately-suffixed file.

* `intraReadUtf8(path) <> string` &mdash; This reads the intra-module file
  named by the indicated relative path, interpreting it as UTF-8 encoded
  text. `path` is as with `intraLoad`, except that the final file is left
  as-is (and not suffixed automatically).

* `intraType(path) <> string | void` &mdash; This gets the type of an
  intra-module file named by the indicated relative path (a string).
  `path` is as with `intraLoad`, except that the final file is left
  as-is (and not suffixed automatically). The return value is the same
  as for `$Io0::fileType` (see which).
