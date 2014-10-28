Samizdat Layer 0: Core Library
==============================

core.Peg :: PegChoice
---------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegChoice`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeChoice(rules*) -> rule`

Makes and returns a parser rule which performs an ordered choice amongst
the given rules. Upon success, it passes back the yield and replacement
state of whichever alternate rule succeeded.

This is equivalent to the syntactic form `{: rule1 | rule2 | etc :}`.
