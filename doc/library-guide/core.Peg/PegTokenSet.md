Samizdat Layer 0: Core Library
==============================

core.Peg :: PegTokenSet
-----------------------

An instance of this class is a parser rule which matches a token whose name
matches that of any of the given ones, consuming it upon success.
Each argument of the `.new()` constructor is taken to be a token name, which
must be a symbol. The result of successful parsing is whatever token was
matched.

This is equivalent to the syntactic form `{: [@token1 @token2 @etc] :}`.

If constructed with just a single name argument, this is equivalent to the
syntactic form `{: @token :}`.

There is also a variant constructor for character sets, which provides an
equivalent to the syntactic form `{: ["string1" "string2" "etc"] :}`.


<br><br>
### Class Method Definitions

#### `.new(names*) -> :PegTokenSet`

Creates an instance of this class.

#### `.newCharSet(strings*) -> :PegTokenSet`

Creates a character set instance of this class. Each argument must be
a string. The result of successful parsing is a character-as-token of the
parsed character.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
