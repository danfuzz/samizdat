Samizdat Layer 0: Core Library
==============================

core.Peg :: PegLookaheadFailure
-------------------------------

An instance of this class is a parser rule which runs a given other rule,
suppressing its usual yield and state update behavior. Instead, if the other
rule succeeds, this rule fails. And if the other rule fails, this one
succeeds, yielding `null` and consuming no input.

This is equivalent to the syntactic form `{: !rule :}`.


<br><br>
### Class Method Definitions

#### `class.new(rule) -> isa PegLookaheadFailure | == eof | == fail`

Creates an instance of this class.

Special cases:

* If `rule` is `core.Peg :: any`, this returns `core.Peg :: eof`.
* If `rule` is `core.Peg :: empty`, this returns `core.Peg :: fail`.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
