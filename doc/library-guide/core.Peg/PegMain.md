Samizdat Layer 0: Core Library
==============================

core.Peg :: PegMain
-------------------

An instance of this class is a rule which provides a fresh (empty)
parsed item scope for another rule. The direct evaluation result of a parser
expression (`{: ... :}`) is always an instance of this class.


<br><br>
### Class Method Definitions

#### `.new(rule) -> :PegMain`

Creates an instance of this class which wraps the given `rule`.

#### `.newChoice(rules*) -> :PegMain`

Creates an instance of this class which wraps a `PegChoice` constructed
from the given `rules`.

#### `.newSequence(rules*) -> :PegMain`

Creates an instance of this class which wraps a `PegSequence` constructed
from the given `rules`.

<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
