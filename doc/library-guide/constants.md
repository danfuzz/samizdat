Samizdat Layer 0: Core Library
==============================

Constants
---------

The definitions in this section are all simple constant values, not
functions.

**Note:** This section only covers constants that are not described
elsewhere in the library documentation.

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

#### `Class`

The reified class of a value.

#### `Data`

The common superclass of all pure data values.

#### `DerivedData`

The common superclass of all derived data values.

#### `Int`

The class of int values.

#### `List`

The class of list values.

#### `Map`

The class of map values.

#### `String`

The class of string values.

#### `Uniqlet`

The class of uniqlet values.

#### `Value`

The common superclass of all values.


<br><br>
### In-Language Definitions

#### `null`

A value used when no other value is suitable, but when a value is
nonetheless required. It is defined as `@Null`, that is, a class-only
value with class tag `"Null"`.

#### `ENTITY_MAP`

Map of entity names to their string values. This is a map from strings to
strings, where the keys are XML entity names (such as `"lt" "gt" "zigrarr"`)
and the corresponding values are the strings represented by those entity
names (such as, correspondingly, `"<" ">" "⇝"`).

The set of entities is intended to track the XML spec for same, which
as of this writing can be found at
<http://www.w3.org/TR/xml-entity-names/bycodes.html>.

**Syntax Note:** Used during the parsing of string literal forms.
