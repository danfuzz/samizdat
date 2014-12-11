Samizdat Layer 0: Core Library
==============================

core.Peg :: PegChoice
---------------------

An instance of this class is a parser rules which perform an ordered choice
amongst a given set of other rules. Upon success, it passes back the yield
and replacement state of whichever alternate rule succeeded.

This is equivalent to the syntactic form `{: rule1 | rule2 | etc :}`.


<br><br>
### Class Method Definitions

#### `class.new(rules*) -> isa PegChoice | == fail`

Creates an instance of this class.

Special cases:

* If no arguments are passed, this returns `core.Peg :: fail`.
* If exactly one argument is passed, this returns that rule directly.

<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
