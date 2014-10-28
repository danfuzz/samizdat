Samizdat Layer 0: Core Library
==============================

core.Peg :: PegResult
---------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegResult`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeResult(value) -> rule`

Makes and returns a parser rule which always succeeds, yielding the
given result `value`, and never consuming any input.

This is equivalent to the syntactic form `{: { value } :}` assuming
that `value` is a constant expression.
