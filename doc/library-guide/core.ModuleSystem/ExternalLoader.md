Samizdat Layer 0: Core Library
==============================

core.ModuleSystem :: ExternalLoader
-----------------------------------

An `ExternalLoader` is an external module loader, which knows how to load
modules identified by "external" sources. An external loader will defer to
its given `nextLoader` for any module or resource requests it cannot find
directly. Notably, external loaders will never directly return any
resources, ever.

<br><br>
### Class Method Definitions

#### `class.new(path, globals, nextLoader) -> isa ExternalLoader`

This creates an external module loader.

`path` is the absolute filesystem path to a directory containing module
definition subdirectories. `nextLoader` is the loader to use to find
required modules that aren't defined within `path`'s hierarchy. `globals`
is the global variable environment to use when evaluating source.

If `path` does not exist, then as a special case, this method just returns
`nextLoader`. (This makes it easy to only construct a loader chain when
needed.) If `path` exists but is not a directory, this method terminates with
a fatal error.

**Note:** If this loader should not have a next module loader, then
`nextLoader` should be passed as `null`.


<br><br>
### Method Definitions: `Loader` protocol

#### `.readResource(source, format) -> . | void`

This always defers to the loder's `nextLoader`.

#### `.resolve(source) -> . | void`

This resolves and loads the module named by `source`.
