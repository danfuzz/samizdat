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
code or resources.

In addition, the module directory optionally contains additional module
definitions. Each module definition is in a subdirectory named
`modules/fully.qualified.name`, where `fully.qualified.name` is the
name of the module.

In Samizdat, all sorts of code is wrapped up into modules. In particular:

* An application is a module, which often defines other modules within itself.
* The core library is a module, which defines the recognizable library
  modules (such as `core.Format`) within itself.

### Importing and exporting

TODO. There is not yet any syntactic support either for importing modules,
or for exporting particular bindings.

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

The core library is loaded as an `IntraLoader`, as are application modules.
In the case of an application module, its "next" loader is the core library.

As a final note, though the default module system is implemented in terms
of the filesystem, all of the behavior of the system is based on generic
functions. These functions can be bound to other types, in order to
provide other interesting and useful arrangements. For example, it is
possible (and may eventually be desirable) to construct a module or
intra-module loader which only depends on immutable data as input.

#### Top-level variable environment in a module

In addition to the normal global variable environment, a module file
when evaluated has additional bindings to allow for a module to load
itself as well as other modules.

* `moduleLoad(path) <> module` &mdash; This loads the module indicated
  by `path` in the same loader which loaded this module. `path` is a
  "module path," that is, a list of strings which represents the fully-
  qualified name of the module.

* `moduleLoader` &mdash; This is a reference to the module loader which
  loaded this module.

* `intraExists(path) <> logic` &mdash; This checks for the existence of an
  intra-module file named by the indicated relative path (a string). It
  returns `path` itself if the file exists or void if not.

* `intraLoad(path) <> . | void` &mdash; This loads and evaluates the
  intra-module file named by the indicated relative path (a string).

* `intraReadUtf8(path) <> string` &mdash; This reads the intra-module file
  named by the indicated relative path, interpreting it as UTF-8 encoded
  text.
