Samizdat Layer 0: Core Library
==============================

Constants
---------

The definitions in this section are all simple constant values, not
functions.

<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `false`

The boolean value false. It is defined as `@["boolean": 0]`.

#### `true`

The boolean value true. It is defined as `@["boolean": 1]`.

#### `null`

A value used when no other value is suitable, but when a value is
nonetheless required. It is defined as `@null`, that is, a valueless
token with type tag `"null"`.

#### `nullBox`

A value used in place of a box, when the would-be set value of the box
is never needed. It is defined as `@nullBox`, that is, a valueless
token with type tag `"nullBox"`.

#### `ENTITY_MAP`

Map of entity names to their string values. This is a map from strings to
strings, where the keys are XML entity names (such as `"lt" "gt" "zigrarr"`)
and the corresponding values are the strings represented by those entity
names (such as, correspondingly, `"<" ">" "⇝"`).

The set of entities is intended to track the XML spec for same, which
as of this writing can be found at
<http://www.w3.org/TR/xml-entity-names/bycodes.html>.
