Samizdat Layer 0: Core Library
==============================

Booleans
--------

<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `booleanFromLogic(logic?) <> boolean`

Bridge between boolean and logic values. If called with no argument,
this returns `true`. If called with any argument, this returns `false`.

This is useful for taking a logic result from a function call and turning
it into flag value, such as might be stored within a data structure.

#### `booleanNot(value) <> boolean`

Returns the opposite boolean value to the one given. If given `true`,
this returns `false`. If given `false`, this returns `true`. It is
an error (terminating the runtime) to pass any other value.

#### `logicFromBoolean(boolean) <> logic`

Bridge between boolean and logic values. If called with `true` this
returns void. If called with `false`, this returns `true`.

This is useful for taking a flag value and using it in a logical
expression (such as might be the expression checked in an `if` statement).
