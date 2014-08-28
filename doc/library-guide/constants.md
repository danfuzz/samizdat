Samizdat Layer 0: Core Library
==============================

Constants
---------

The definitions in this section are all simple constant values, not
functions.

**Note:** This section only covers constants that are not described
elsewhere in the library documentation.

<br><br>
### Method Definitions

(none)


<br><br>
### Primitive Definitions

#### Classes

Most of the classes described in this guide have corresponding global
variables with the same name. For example, `Bool` is bound to the class of
all boolean values.


<br><br>
### In-Language Definitions

#### `ENTITY_MAP`

Map of entity names to their string values. This is a map from strings to
strings, where the keys are XML entity names (such as `"lt" "gt" "zigrarr"`)
and the corresponding values are the strings represented by those entity
names (such as, correspondingly, `"<" ">" "⇝"`).

The set of entities is intended to track the XML spec for same, which
as of this writing can be found at
<http://www.w3.org/TR/xml-entity-names/bycodes.html>.

**Syntax Note:** Used during the parsing of string literal forms.
