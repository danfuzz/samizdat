Samizdat Layer 0: Core Library
==============================

core.ModuleSystem
-----------------

This module is the module which knows how to load modules. It is also
where much of the single-file loading logic resides, since that interacts
tightly with module loading.

**Note:** The constant `null` can be treated as a module loader. When used
as such, it "knows" the two modules `core.Io0` and `core.Lang0`. These are
set up as "bootstrap modules," as otherwise they would be their own
dependencies.


<br><br>
### Generic Function Definitions: `ModuleLoader` protocol

#### `moduleLoad(loader, fqName) <> .`

This loads the module named by `fqName`, which is expected to be a
fully-qualified module name as a string (e.g. `"core.Format"`). It returns
whatever is exported by the module. If the module doesn't export anything,
then this returns `{}` (the empty map).

This function will only ever load a given module once. If the same name
is requested more than once, whatever was returned the first time
is returned again, without re-evaluating the module.

It is an error (terminating the runtime) if `fqName` does not correspond to
a module known to this loader. It is also an error (terminating the runtime)
if `fqName` is not a valid module name.


<br><br>
### Generic Function Definitions: `IntraLoader` protocol

#### `intraLoad(loader, path) <> .`

This loads and evaluates an internal module (an "intra-module module") file.
`path` is expected to be a string identifying a relative file path within the
(outer) module's file hierarchy. The final name component in `path` must
*not* have a file suffix (such as `.sam` or `samb`).

This returns the map of exports from the loaded module. If the module doesn't
export anything, then this returns `{}` (the empty map).

This function will only ever load a given path once. If the same path
is requested more than once, whatever was returned the first time
is returned again, without re-evaluating the module.

It is an error (terminating the runtime) if the indicated `path` does not
correspond to an existing file. It is also an error (terminating the runtime)
if the indicated `path` failed to be loadable.

#### `intraRead(loader, path, format) <> string`

This reads an intra-module resource file, interpreting it as the given
`format` (a string name). `path` is expected to be a string identifying a
relative file path within the module's file hierarchy.

It is an error (terminating the runtime) if the indicated `path` does not
exist as a file, or if the given `format` is not recognized.

Recognized types include:

* `utf8` &mdash; Treat the file as UTF-8 encoded text, returning a string.

#### `intraType(loader, path) <> string | void`

This gets the type of an intra-module resource file named by the indicated
relative `path`. The return values are the same as for `$Io0::fileType`
(see which).

`path` is expected to be a string identifying a relative file path within the
module's file hierarchy.


<br><br>
### In-Language Definitions

#### `makeIntraLoader(path, globals, moduleLoader) <> IntraLoader`

This creates and returns an intra-module file loader, for which the `intra*`
family of functions produces useful results.

`path` is the absolute filesystem path to the main module directory.
`moduleLoader` is the loader to use to find required modules that aren't
defined within this module. `globals` is the global variable environment
to use when evaluating source.

**Note:** If this loader should not have a module loader, then
`moduleLoader` should be passed as `null`.

#### `makeModuleLoader(path, globals, nextModuleLoader) <> ModuleLoader`

This creates a module loader, for which the `moduleLoad` function produces
useful results.

`path` is the absolute filesystem path to a directory containing module
definition subdirectories. `nextModuleLoader` is the loader to use to find
required modules that aren't defined within `path`'s hierarchy. `globals`
is the global variable environment to use when evaluating source.

If `path` does not exist, then as a special case, this function just returns
`nextModuleLoader`. (This makes it easy to only construct a loader chain
when needed.) If `path` exists but is not a directory, this function
terminates with a fatal error.

**Note:** If this loader should not have a next module loader, then
`nextModuleLoader` should be passed as `null`.

#### `main(libraryPath, primitiveGlobals) <> {globals*}`

This is the main entrypoint for loading the entire system. As such, it's
not that useful for most code.

This constructs an intra-module loader for the given `libraryPath`, which is
expected to be the path to a core library implementation. It then loads
the `main` file of that library, and runs it, passing it the same two
arguments given to this function.

This returns whatever the library's `main` returns, which is generally
expected to be the library's full global environment, as a map.

#### `run(path, moduleLoader, args*) <> . | void`

This loads the `main` of the module at the given `path`, finds its
`main` binding, and runs it, handing it the given `args`.

This is a convenient wrapper which is equivalent to:

```
def globals = moduleLoad(moduleLoader, "core.Globals")::fullEnvironment();
def loader = makeIntraLoader(path, globals, moduleLoader);
def mainModule = intraLoad(loader, "main");
<> mainModule::main(args*)
```

except with more error checking.

**Note:** By convention, the first argument passed to a file, when invoked
from an interactive commandline or from a scripting environment, is the
filesystem path to itself. This function does *not* automatically add this
argument. Users of this function should add it to the given `args*` when
appropriate.

#### `runFile(path, moduleLoader, args*) <> . | void`

This runs a solo file at the given `path`. It works for both source text
and binary files, switching based on the file name suffix, `.sam` for text
and `.samb` for binary.

In the case of source text, an appropriate language module is loaded up
from the given `moduleLoader`.

In both cases, the global environment which the file is given is the
same as is used when loading modules, except that none of the `intra*`
functions are made available.

The direct result of evaluation of the file is a function of no arguments.
This is called. If that returns a map, then `main` is looked up in it,
and that `main` is called, passing it `args*`. The final result is whatever
is returned by the call to `main`.

If the initial function result is void, or isn't a map, or the map doesn't
bind `main`, then this function simply returns void.

**Note:** By convention, the first argument passed to a file, when invoked
from an interactive commandline or from a scripting environment, is the
filesystem path to itself. This function does *not* automatically add this
argument. Users of this function should add it to the given `args*` when
appropriate.
