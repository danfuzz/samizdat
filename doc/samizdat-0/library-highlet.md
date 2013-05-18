Samizdat Layer 0: Core Library
==============================

Tokens
--------

<br><br>
### Primitive Definitions

#### `highletHasValue token <> boolean`

Returns `true` iff the given token has a non-void payload data value.

#### `highletType token <> .`

Returns the type tag value (an arbitrary value) of the given token.

#### `highletValue token notFound? <> . | !.`

Returns the payload data value (an arbitrary value) of the given
token, if any. If the token is valueless, returns the `notFound`
value if supplied, or returns void if not.


<br><br>
### In-Language Definitions

(none)
