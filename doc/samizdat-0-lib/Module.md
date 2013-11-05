Samizdat Layer 0: Core Library
==============================

core::Module
------------

A `Module` is a derived value with a map payload, consisting of
mappings for `info` and `exports`, each of which is expected to
itself be a map. `Module` implements the `Collection` protocol, effectively
by delegating to the `exports`.

Module values in-hand can be bound into the system with `moduleDef`. This
is used during system bootstrap.

More commonly, modules can be found in stable storage and initialized
with `moduleUse`.


<br><br>
### Generic Function Definitions: `Collection` protocol

#### `cat(modu, more*) <> module`

Returns a module with the same `info` as the one given, but with
additional and/or replaced `exports` mappings from the `more`
modules.

#### `del(modu, key) <> module`

Returns a module just like the given `modu`, except that
the `exports` mapping for the given `key` is removed.

#### `get(modu, key) <> . | void`

Returns the `exports` element of the module that corresponds to the given
`key`.

#### `keyList(modu) <> list`

Returns the list of keys mapped by the module's `exports`.

#### `nth(modu, n) <> . | void`

Returns the nth (zero-based) `exports` element of the module.

#### `put(modu, key, value) <> module`

Returns a module just like the given `modu`, except that
the `exports` mapping for the given `key` is to the given `value`.

#### `sizeOf(modu) <> int`

Returns the number of elements in the given module's `exports`.

#### `slice(modu, start, end?) <> module`

Returns a module just like the given one, except with only the given slice
of the original's `exports`.


<br><br>
### Generic Function Definitions: One-Offs

#### `nameOf(modu) <> . | void`

Returns the `name` mapping in the module's `info`.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `moduleUse(searchInfo)` <> module

Gets a module, as if by `moduleGet`, if it has already been defined; or
attempts to load it from storage if it has *not* already been defined.

The `searchInfo` map indicates the required information about the module.
The `name` binding of `searchInfo` is required and should be a list-of-strings
representing the fully-qualified module name.

This will fail with a terminal error if the module isn't defined and
cannot be loaded.

TODO: Right now, only the `name` in the `searchInfo` is checked.
