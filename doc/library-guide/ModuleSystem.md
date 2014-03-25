Samizdat Layer 0: Core Library
==============================

core::ModuleSystem
------------------

This module is the module which knows how to load modules!

It provides a couple different versions of loading, useful in a couple
different contexts.


<br><br>
### Generic Function Definitions: `ModuleLoader` protocol

#### `moduleLoad(loader, path) <> .`

This loads the module named by `path`, which is expected to be a list of
strings (e.g. `["core", "Format"]`). It returns whatever is exported by
the module. If the module doesn't export anything, then this returns
`{}` (the empty list).

This function will only ever load a given path once. If the same path
is requested more than once, whatever was returned the first time
is returned again, without re-evaluating the module.

It is an error (terminating the runtime) if `path` does not correspond to
a module known to this loader. It is also an error (terminating the runtime)
if `path` is not a valid module name path (list of strings).


<br><br>
### Generic Function Definitions: `IntraLoader` protocol

#### `localExists(loader, path) <> path | void`

This indicates if the given intra-module file exists and is a regular
(non-directory, non-special) file. It returns the given `path` for
logical-true and void for logical-false.

#### `localLoad(loader, path) <> . | void`

This loads and evaluates an intra-module file. The `path` is expected to be
a list of strings identifying a relative file path within the module's file
hierarchy. The final name in `path` should *not* have a file suffix (such
as `.sam` or `samb`).

This function will only ever load a given path once. If the same path
is requested more than once, whatever was returned the first time
is returned again, without re-evaluating the module.

It is an error (terminating the runtime) if the indicated `path` does not
correspond to an existing file. It is also an error (terminating the runtime)
if the indicated `path` failed to be loadable.

#### `localRead(loader, path) <> .`

This reads an intra-module file. The `path` is expected to be a list of
strings identifying a relative file path within the module's file
hierarchy.

It is an error (terminating the runtime) if the indicated `path` does not
exist as a file.


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

**Note:** If this loader should not have a next module loader, then
`nextModuleLoader` should be passed as `null`.

#### `run(path, globals, moduleLoader, args*) <> . | void`

This loads the `main` of the module at the given `path`, finds its
`main` binding, and runs it, handing it the given `args`.

This is a convenient wrapper which is equivalent to:

```
def loader = makeIntraLoader(path, globals, moduleLoader);
def mainModule = intraLoad(loader, ["main"]);
<> mainModule::main(args*)
```

except with more error checking.
