Samizdat Layer 0: Core Library
==============================

Meta-Control
------------

These functions "break out" of the semantic model of Samizdat, in a manner
of speaking.

<br><br>
### Functions

#### `die(strings*) -> n/a  ## Terminates the runtime.`

Prints the given strings to the system console (as if by calling
`note(strings*`), and terminates the runtime with a failure status code (`1`).

#### `note(strings*) -> void`

Writes out a newline-terminated note to the system console or equivalent,
by concatenating all the strings together (with no separators).

This is intended for debugging, and as such this will generally end up
emitting to (something akin to) a standard-error stream.
