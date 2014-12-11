Samizdat Layer 0: Core Library
==============================

core.Peg :: PegCode
-------------------

An instance of this class is a parser rule which calls a given function.
When called, it is passed as arguments all the in-scope matched results from
the current sequence context. Whatever it returns becomes the yielded value of
the rule. If it returns void, then the rule is considered to have failed.
Code rules never consume any input.

This is equivalent to the syntactic form
`{: ... { arg1, arg2, etc -> code } :}`.


<br><br>
### Class Method Definitions

#### `class.new(function) -> isa PegCode`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
