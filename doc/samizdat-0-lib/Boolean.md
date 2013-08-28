Samizdat Layer 0: Core Library
==============================

Booleans
--------

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### Constant: `false`

The boolean value false. It is defined as `@[Boolean: 0]`.

#### Constant: `true`

The boolean value true. It is defined as `@[Boolean: 1]`.

#### `booleanNot(value) <> boolean`

Returns the opposite boolean value to the one given. If given `true`,
this returns `false`. If given `false`, this returns `true`. It is
an error (terminating the runtime) to pass any other value.

#### `logicFromBoolean(boolean) <> logic`

Bridge between boolean and logic values. If called with `true` this
returns `true`. If called with `false`, this returns void.

This is equivalent to applying the `**` operator to the argument.

This is useful for taking a flag value and using it in a logical
expression (such as might be the expression checked in an `if` statement).

**Syntax Note:** Used in the translation of `expression**` forms.
