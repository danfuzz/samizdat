Samizdat Layer 0: Core Library
==============================

core::Module0
-------------

The `Module0` module defines the basic mechanism for defining and accessing
modules. In addition to exporting module-scoped bindings (as usual for
a module), this also defines some functions in the global context, both
for convenience as well as necessarily. In the latter case, this specifically
breaks the recursion of having to have a module in order to look up a
module.

The globally exported functions are:

* `moduleGet`
* `moduleDef`
* `moduleUse`

<br><br>
### Function Definitions

#### `getAllModules() <> list`

Gets a list of all currently loaded modules.

#### `getExports(modu) <> map`

Gets a map of all the definitions exported from the given module,
as a mapping of names to values.

#### `getGlobals(modu) <> map`

Gets a map of all the global variables exported from the given module,
as a mapping of names to values.

#### `getInfo(modu) <> map`

Gets a map of the metainformation about the given module, as a
mapping of names to values.

#### `load(globals, fqPath) <> module`

Loads a module from the directory named by the given fully-qualified path
(list of directory names), using the given `globals` for global bindings.

This loads the code and returns the constructed module, but it does *not*
cause the module to be findable via `moduleGet`. It also does *not* check to
see if the module has already been loaded.

This is the "raw" version of module loading, which takes a few shortcuts
compared to the full module-loading semantics. It is not intended to be
used directly.

#### `moduleDef(modu) <> void`

Defines a new module, for later retrieval via `moduleGet` or `moduleUse`.
There must not already be a defined module with the same name as the given
one.

#### `moduleGet(searchInfo) <> module | .`

Gets an already-defined module whose `info` matches the given `searchInfo`.
Returns the matching module if it is found, or returns void if there was
no such already-defined module.

The `searchInfo` map indicates the required information about the module.
The `name` binding of `searchInfo` is required and should be a list-of-strings
representing the fully-qualified module name.

TODO: Right now, only the `name` in the `searchInfo` is checked.

#### `moduleUse(searchInfo, modulePaths?)` <> module

See the module `core::Module` for the full description.

In the `Module0` version of this function, it requires that the
indicated module already have been loaded in order to be found.
In addition, only the `name` in the `searchInfo` is checked.

#### `stringFromModuleName(fqName) <> string`

Returns a "human-oriented" name given a list-of-components fully-qualified
module name.
