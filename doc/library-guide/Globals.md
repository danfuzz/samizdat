Samizdat Layer 0: Core Library
==============================

core.Globals
------------

The `Globals` module is the provider of the standard global variable
environment used when evaluating Samizdat source in the usual manner.

<br><br>
### In-Language Definitions

#### `earlyEnvironment() -> map`

Returns a map of the simplest constant and function definitions. This
function is used internally during system bootstrap.

#### `fullEnvironment() -> map`

Returns a map of all of the standard global variable bindings.

Find details of the bindings as noted by the documentation on other modules.
