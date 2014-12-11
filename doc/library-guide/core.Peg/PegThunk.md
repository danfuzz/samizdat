Samizdat Layer 0: Core Library
==============================

core.Peg :: PegThunk
--------------------

An instance of this class is a parser rule which runs the given function to
produce a parser value, which is then called to do the actual parsing.
When called, it is passed as arguments all the in-scope matched results from
the current sequence context. Whatever it returns is expected to be a parser,
and that parser is then called upon to perform parsing.

If the called function returns void, then the rule is considered to have
failed.

This is equivalent to the syntactic form `{: %term :}`.


<br><br>
### Class Method Definitions

#### `class.new(function) -> isa PegThunk`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
