Samizdat Layer 0: Core Library
==============================

core.ModuleSystem
-----------------

This module is the module which knows how to load modules. It is also
where much of the single-file loading logic resides, since that interacts
tightly with module loading.

Several of the functions in this module take a parameter called `source`.
A "source" identifies the name of an external module, a relative path
to an internal module, or a relative path to a resource. Sources must
be `@external` or `@internal` values with a string payload,
as described in the "Execution Trees" section of the language guide.

**Note:** The constant `null` can be treated as a module loader. When used
as such, it "knows" the modules `core.Code`, `core.Io0`, and `core.Lang0`.
These are set up as "bootstrap modules," as otherwise they would, in effect,
be their own dependencies.


<br><br>
### Classes

* [ExternalLoader](ExternalLoader.md)
* [InternalLoader](InternalLoader.md)

<br><br>
### Method Definitions: `Loader` protocol

#### `.readResource(source, format) -> . | void`

This reads and/or processes a resource file, interpreting it as the given
`format` (a symbol). `source` is expected to be a source specifier,
identifying the location of the resource.

It is an error (terminating the runtime) if the given `format` is not
recognized.

This returns void if the `source` is not found or if `format` does not
indicate a valid way to process the source.

See "Resource Import" in the language guide for more details on the
available `format`s.

#### `.resolve(source) -> . | void`

This resolves and loads the module (either an internal or external module)
named by `source`. `source` is expected to be a source specifier. This
returns a `@module` representing the loaded module, or void if `source`
did not correspond to a known module.

This function will only ever load a given module once. If the same name
is requested more than once, whatever was returned the first time
is returned again, without re-evaluating the module.

It is an error (terminating the runtime) if the indicated `source` correspends
to a known module but, for some reason, failed to be successfully loaded.


<br><br>
### Functions

#### `loadModule(loader, source) -> .`

This loads the module named by `source`, returning its `exports` map.

It is an error (terminating the runtime) if `source` does not correspond to
a module known to `loader`. It is also an error (terminating the runtime)
if `source` is not a valid source specifier.

**Note:** This function is implemented in terms of the `resolve()` method.

#### `loadResource(loader, source, format) -> .`

This reads and/or processes a resource file, interpreting it as the given
`format` (a symbol). `source` is expected to be a source specifier.

It is an error (terminating the runtime) if the given `format` is not
recognized. It is also an error (terminating the runtime) if the indicated
`source` cannot be processed per the indicated `format`. Notably, for the
most part it is an error if `source` does not exist as a file.

See "Resource Import" in the language guide for more details on the
available `format`s.

**Note:** This function is implemented in terms of the `readResource()`
method.

#### `main(libraryPath, primitiveGlobals) -> isa SymbolTable`

This is the main entrypoint for loading the entire system. As such, it's
not that useful for most code.

This constructs `InternalLoader` for the given `libraryPath`, which is
expected to be the path to a core library implementation. It then loads
the `main` file of that library, and runs it, passing it the same two
arguments given to this function.

This returns whatever the library's `main` returns, which is generally
expected to be the library's full global environment, as a symbol table.

#### `run(path, loader, args*) -> . | void`

This loads the `main` of the module at the given `path`, finds its
`main` binding, and runs it, handing it the given `args`.

This is a convenient wrapper which is equivalent to:

```
def source = @external{name: "core.Globals"};
def globals = loadModule(loader, source)::fullEnvironment();
def mainLoader = InternalLoader.new(path, globals, loader);
def mainModule = resolveMain(mainLoader)::exports;
return mainModule::main(args*)
```

except with more error checking.

**Note:** By convention, the first argument passed to a file, when invoked
from an interactive commandline or from a scripting environment, is the
filesystem path to itself. This function does *not* automatically add this
argument. Users of this function should add it to the given `args*` when
appropriate.

#### `runFile(path, loader, args*) -> . | void`

This runs a solo file at the given `path`. It works for both source text
and binary files, switching based on the file name suffix, `.sam` for text
and `.samb` for binary.

In the case of source text, an appropriate language module is loaded up
from the given `loader`.

In both cases, the global environment which the file is given is the
same as is used when loading modules, except that none of the provided
functions will handle module-internal sources.

The direct result of evaluation of the file is a function of no arguments.
This is called. If that returns a module, then `main` is looked up in it,
and that `main` is called, passing it `args*`. The final result is whatever
is returned by the call to `main`.

If the initial function result is void, or isn't a map, or the map doesn't
bind `main`, then this function simply returns void.

**Note:** By convention, the first argument passed to a file, when invoked
from an interactive commandline or from a scripting environment, is the
filesystem path to itself. This function does *not* automatically add this
argument. Users of this function should add it to the given `args*` when
appropriate.
