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

The module directory optionally contains additional module-internal files
in an arbitrary subdirectory hierarchy. Module-internal files can be either
internal modules (single files of code, which aren't exposed externally) or
resources.

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
  a particular application) can have a name that consists of just a single
  capitalized component.
* All independently published modules should have names with at least two
  components (with details as follows).
* All regular implementation modules in the core library have two components,
  with first being `core`. For example, `core.Format`.
* All modules that are part of a published library should have a first
  component that names the library in a manner that is intended to be concise
  but unambiguous. If the library has no interesting structure, then it should
  have a second component that is identical to the first, except that the
  first letter is capitalized. For example, `funLib.FunLib` (a module with
  no further structure) or `funLib.Constants` (a module with structure).

Internal modules and resource files are named as partial paths, with
components separated by slashes (`/`), and always introduced with a dot-slash
(`./`). Unlike normal (Posix-like) filesystem paths, components must adhere
to identifier syntax, except for the last component of a resource, which can
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

`import` statements must always be the first statements in a file, before
any other definitions.

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
modules, except that the format of file &mdash; that is, how to interpret the
contents &mdash; must be specified before the resource name, in the form
`@formatName` (similar to a symbol).

As with the other imports, an explicit variable name to bind is optional.
If no name is supplied, then the default is the final component name of
the resource, minus any extension.

The following formats are understood by the system:

* `@type` &mdash; Identify the file type of the resource. Return values are
  the same as for `core.Io0::fileType` (see which for details).

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

The basic form is just `export varName1, varName2, ...`, to cause the contents
of the variables named `varName1`, `varName2`, and so on, to be exported from
the module, bound to the key `"varName"`.

As a conveniend short-hand, `export` can be used as a prefix on immutable
variable definitions, function definitions, and `import` statements, to
export the variables so-defined.

**Note:** The thing that is exported is the value of a variable, and not
the "cell" containing a variable. So, it is generally a bad idea to export
a variable that was defined with `var` and not `def`.

Examples:

```
## Export a previously-defined variable.
def blort = ...;
export blort;

## Export a couple previously-defined variables.
def blort = ...;
def frotz = ...;
export blort, frotz;

## Define and export a variable, together.
export def blort = ...;

## Define and export a regular function, together.
export fn blort() { ... };

## Export a whole imported module. It is exported as `$submodule`.
export import ./submodule;

## Export a whole imported module. It is exported as `blort`.
export blort = import ./submodule;

## Export some bindings from another module. The exported names are the same
## as the imported ones.
export import ./submodule :: fizmo, frotz;

## Export all the bindings from another module. The exported names are the
## same as the imported ones.
export import ./submodule :: *;
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
by `ModA`, per se. That is, `ModA` might itself contain a module library; this
is `ModA`'s "captive" module library, in that the library is not visible
beyond the definition of `ModA`. In terms of directory hierarchy, if `ModA` is
in `/x/y/modules/ModA`, then the system will look for a captive module in
`/x/y/modules/ModA/modules/ModB`.

If `ModA` doesn't define `ModB` directly as a captive module, then the system
will look for a sibling definition of `ModB` if applicable (that is, if
`ModA` is part of a library). In terms of directory hierarchy, if `ModA` is in
`/x/y/modules/ModA`, then the system will look for a sibling module
in `/x/y/modules/ModB`.

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

There are two classes which interplay to cause module loading to happen.
Both classes bind a method `resolve`.

One class is `ExternalLoader`, which gets instantiated with two main
pieces of information, (a) a filesystem path to a directory containing
module definitions, and (b) a reference to the "next" `ExternalLoader` to
use. `ExternalLoader` defines a `resolve` method, which is the thing that
looks for a module in its designated directory, and then calls on the
"next" loader if that fails. The recursion bottoms out in a definition of
`resolve` on `null`, which always fails.

The other class is `InternalLoader`, which gets instantiated with two pieces
of information, (a) a filesystem path to a directory containing the definition
of *one* module, and (b) a reference to the "next" loader to use (said next
loader typically being an `ExternalLoader`). When instantiated,
`InternalLoader` makes a `ExternalLoader` which points to a `modules`
directory under its given filesystem path, and that `ExternalLoader` is the
one that's used directly by the `InternalLoader` implementation to find
captive external modules. The `InternalLoader`'s filesystem path is used
directly for module-internal files. `InternalLoader` defines a `resolve`
method, which handles internal sources directly and defers to its "next"
loader for all other requests.

The core library is loaded as an `InternalLoader`, as are application modules.
In the case of an application module, its "next" loader is the core library.

As a final note, though the default module system is implemented in terms
of the filesystem, all of the behavior of the system is based on method
bindings. These methods can be bound to other classes, in order to
provide other interesting and useful arrangements. For example, it is
possible (and might eventually be desirable) to construct a loader which
depends only upon immutable data as input.

#### Example filesystem layout

```
/path/to/castingApp
  main.sam                   application's main file
  appHelp.sam                internal module for the applicaton itself
  modules/
    Blort/                   application's captive `Blort` module
      main.sam
      darkness.sam           internal module file for `Blort` module
    Frotz/                   application's captive `Frotz` module
      main.sam
      modules/
        Fizmo/               `Frotz`'s captive `Fizmo` module
          main.sam
        Igram/               `Frotz`'s captive `Igram` module
          main.sam
```

#### Top-level variable environment in a module

In addition to the normal global variable environment as defined by the
module `core.Globals`, a module file when evaluated has additional bindings
to allow for a module to load the other parts of itself, as well as load
other modules.

* `loadModule(source) -> map` &mdash; This loads and evaluates the indicated
  module, returning the `exports` map. `source` is a source specifier
  (either an `@external` or `@internal` value) representing the origin of
  the module. For an `@internal` source, the final file name component
  must *not* have a suffix; the module system handles finding the
  appropriately-suffixed file.

* `loadResource(source, format) -> . | void` &mdash; This reads and/or
  processes the resource file named by the indicated `source`, interpreting it
  as indicated by the `format`. `source` is as with `loadModule`, except that
  the final file component is left as-is (and not suffixed automatically).
  See "Resource import" above for details about `format`.

* `thisLoader()` &mdash; This is a function which returns a reference to
  the module loader which loaded this module.
