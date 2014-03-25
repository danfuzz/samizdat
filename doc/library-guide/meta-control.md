Samizdat Layer 0: Core Library
==============================

Meta-Control
------------

These functions "break out" of the semantic model of Samizdat, in a manner
of speaking.

<br><br>
### Primitive Definitions

#### `die(string?) <> n/a ## Terminates the runtime.`

Prints the given string to the system console (as if with `note`)
if supplied, and terminates the runtime with a failure status code (`1`).

#### `note(string) <> void`

Writes out a newline-terminated note to the system console or equivalent.
This is intended for debugging, and as such this will generally end up
emitting to (something akin to) a standard-error stream.


<br><br>
### In-Language Definitions

(none)
