Samizdat Layer 0: Core Library
==============================

core.Peg :: PegMain
-------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegMain`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeMainChoice(rules*) -> rule`

Makes and returns a parser rule which tries the given rules in order until
one succeeds. This is identical to `makeChoice` (see which), except that
it provides a fresh (empty) parsed item scope.

This is equivalent to the syntactic form `{: rule1 | rule2 | etc :}`.

#### `makeMainSequence(rules*) -> rule`

Makes and returns a parser rule which runs a sequence of given other rules
(in order). This is identical to `makeSequence` (see which), except that
it provides a fresh (empty) parsed item scope.

This is equivalent to the syntactic form `{: rule1 rule2 etc :}`.
