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

<br><br>
### Function Definitions

#### `load(fqPath) <> module`

Loads a module from the directory named by the given fully-qualified path
(list of directory names). This loads the code and returns the constructed
module, but it does *not* cause the module to be findable via `moduleGet`.
It also does *not* check to see if the module has already been loaded.

This is the "early" version of module loading, which takes a few shortcuts
compared to the full module-loading semantics. This is what is used to load
most of the system modules, and it is not intended for use beyond that.

#### `moduleDef(modu) <> module`

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

#### `stringFromModuleName(fqName) <> string`

Returns a "human-oriented" name given a list-of-components fully-qualified
module name.
