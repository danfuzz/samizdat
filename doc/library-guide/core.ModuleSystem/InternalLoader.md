Samizdat Layer 0: Core Library
==============================

core.ModuleSystem :: InternalLoader
-----------------------------------

An `InternalLoader` is an module-internal file loader, which knows how
to load internal modules (private implementation files) and resource
files. An internal loader will defer to its given `nextLoader` for any module
or resource requests it cannot find directly.

<br><br>
### Class Method Definitions

#### `class.new(path, globals, nextLoader) -> isa InternalLoader`

This creates a module-internal file loader.

`path` is the absolute filesystem path to the main module directory.
`nextLoader` is the loader to use to find required modules that aren't
defined within this module. `globals` is the global variable environment
to use when evaluating source.

**Note:** If this loader should not have a module loader, then
`nextLoader` should be passed as `null`.


<br><br>
### Method Definitions: `Loader` protocol

#### `.readResource(source, format) -> . | void`

This reads and/or processes a resource file.

#### `.resolve(source) -> . | void`

This resolves and loads the module named by `source`.
