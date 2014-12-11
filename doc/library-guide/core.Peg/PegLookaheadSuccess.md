Samizdat Layer 0: Core Library
==============================

core.Peg :: PegLookaheadSuccess
-------------------------------

An instance of this class is a parser rule which runs a given other rule,
suppressing its usual state update behavior. Instead, if the other rule
succeeds, this rule also succeeds, yielding the same value but *not*
consuming any input.

This is equivalent to the syntactic form `{: &rule :}`.


<br><br>
### Class Method Definitions

#### `class.new(rule) -> isa PegLookaheadSuccess`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
