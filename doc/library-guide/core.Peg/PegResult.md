Samizdat Layer 0: Core Library
==============================

core.Peg :: PegResult
---------------------

An instance of this class is a parser rule which always succeeds, yielding the
given result `value`, and never consuming any input.

This is equivalent to the syntactic form `{: { value } :}` assuming
that `value` is a constant expression.


<br><br>
### Class Method Definitions

#### `class.new(value) -> isa PegResult`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
