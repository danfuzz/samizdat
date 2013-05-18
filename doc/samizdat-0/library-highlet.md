Samizdat Layer 0: Core Library
==============================

Tokens
--------

<br><br>
### Primitive Definitions

#### `highletHasValue highlet <> boolean`

Returns `true` iff the given highlet has a non-void payload data value.

#### `highletType highlet <> .`

Returns the type tag value (an arbitrary value) of the given highlet.

#### `highletValue highlet notFound? <> . | !.`

Returns the payload data value (an arbitrary value) of the given
highlet, if any. If the highlet is valueless, returns the `notFound`
value if supplied, or returns void if not.


<br><br>
### In-Language Definitions

(none)
