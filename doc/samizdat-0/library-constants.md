Samizdat Layer 0: Core Library
==============================

Constants
---------

The definitions in this section are all simple constant values, not
functions.

<br><br>
### Primitive Definitions

#### `false`

The boolean value false.

**Note:** Technically, this value could be defined in-language as
`false = [:@boolean @0:]`. However, as a practical matter the
lowest layer of implementation needs to refer to this value, so
it makes sense to allow it to be exported as a primitive.

#### `true`

The boolean value true.

**Note:** Technically, this value could be defined in-language as
`true = [:@boolean @1:]`. However, as a practical matter the
lowest layer of implementation needs to refer to this value, so
it makes sense to allow it to be exported as a primitive.


<br><br>
### In-Language Definitions

#### `null`

A value used when no other value is suitable, but when a value is
nonetheless required. It can also be written as `[:@null:]`.
