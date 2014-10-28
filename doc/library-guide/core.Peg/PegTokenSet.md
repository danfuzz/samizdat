Samizdat Layer 0: Core Library
==============================

core.Peg :: PegTokenSet
-----------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegTokenSet`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeCharSet(strings*) -> rule`

Makes and returns a parser rule which matches any character of any of
the given strings, consuming it upon success. Each argument must be
a string. The result of successful parsing is a character-as-token of the
parsed character.

This function exists primarily to aid in "by hand" parser implementation
in Samizdat Layer 0 (as opposed to using the higher layer syntax), as
it is merely a convenient wrapper for a call to `makeTokenSet` (see
which).

This is equivalent to the syntactic form `{: ["string1" "string2" "etc"] :}`.

#### `makeToken(name) -> rule`

Makes and returns a parser rule which matches any token with the same
name as given. `name` must be a symbol. Upon success, the rule consumes and
yields the matched token.

This is also used to match single characters in tokenizers.

This is equivalent to the syntactic form `{: @token :}` or `{: "ch" :}`
(where `ch` represents a single character).

#### `makeTokenSet(names*) -> rule`

Makes and returns a parser rule which matches a token whose name
matches that of any of the given ones, consuming it upon success.
Each argument is taken to be a token name, which must be a symbol.
The result of successful parsing is whatever token was matched.

This is equivalent to the syntactic form `{: [@token1 @token2 @etc] :}`.

