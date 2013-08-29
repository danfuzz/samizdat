Samizdat Layer 0: Core Library
==============================

Boolean
-------

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
