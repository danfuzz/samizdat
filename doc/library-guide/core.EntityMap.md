Samizdat Layer 0: Core Library
==============================

core.EntityMap
--------------

This module defines a map of XML-style entity names to corresponding strings.

<br><br>
### Constants

#### `ENTITY_MAP`

Table of entity names to their string values. This is a symbol table from
strings to strings, where the keys are XML entity names (such as `"lt"
"gt" "zigrarr"`) and the corresponding values are the strings represented
by those entity names (such as, correspondingly, `"<" ">" "⇝"`).

The set of entities is intended to track the XML spec for same, which
as of this writing can be found at
<http://www.w3.org/TR/xml-entity-names/bycodes.html>.

**Syntax Note:** Used during the parsing of string literal forms.
