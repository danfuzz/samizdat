Samizdat Layer 0: Core Library
==============================

core.Peg :: PegTokenSetComplement
---------------------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegTokenSetComplement`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeCharSetComplement(string*) -> rule`

Makes and returns a parser rule which matches any character *except*
one found in any of the given strings, consuming it upon success.
Each argument must be a string. The result of successful parsing is a
character-as-token of the parsed character.

This function exists primarily to aid in "by hand" parser implementation
in Samizdat Layer 0 (as opposed to using the higher layer syntax), as
it is merely a convenient wrapper for a call to `makeTokenSetComplement`
(see which).

This is equivalent to the syntactic form
`{: [! "string1" "string2" "etc."] :}`.

#### `makeTokenSetComplement(names*) -> rule`

Makes and returns a parser rule which matches a token whose name
matches none of any of the given tokens, consuming it upon success.
Each argument is taken to be a token name, which must be a symbol.
The result of successful parsing is whatever token was matched.

This is equivalent to the syntactic form `{: [! @token1 @token2 @etc] :}`.
