Samizdat Layer 0: Core Library
==============================

core.Peg :: PegSetComplement
----------------------------

An instance of this class is a parser rule which matches a token whose name
matches that of any *but* the given ones, consuming it upon success.
Each argument of the `.new()` constructor is taken to be a token name, which
must be a symbol. The result of successful parsing is whatever token was
matched.

This is equivalent to the syntactic form `{: [! @token1 @token2 @etc] :}`.

There is also a variant constructor for character sets, which provides an
equivalent to the syntactic form `{: [! "string1" "string2" "etc."] :}`.


<br><br>
### Class Method Definitions

#### `class.new(names*) -> isa PegSetComplement | == any`

Creates an instance of this class.

As a special case, if no arguments are passed, this returns `core.Peg :: any`.

#### `class.newChars(strings*) -> isa PegSetComplement | == any`

Creates a character set instance of this class, which matches any token
*except* the characters of the given `strings`. Each argument must be a
string. The result of successful parsing is a character-as-token of the parsed
character.

As a special case, if no arguments are passed, this returns `core.Peg :: any`.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
