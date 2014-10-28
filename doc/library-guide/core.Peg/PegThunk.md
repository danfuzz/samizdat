Samizdat Layer 0: Core Library
==============================

core.Peg :: PegThunk
--------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegThunk`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeParserThunk(function) -> rule`

Makes and returns a parser rule which runs the given function to produce
a parser, which is then called to do the actual parsing. `function`
must be a function. When called, it is passed as arguments all the
in-scope matched results from the current sequence context. Whatever it
returns is expected to be a parser, and that parser is then called upon
to perform parsing.

If the called function returns void, then the rule is considered to have
failed.

This is equivalent to the syntactic form `{: %term :}`.
