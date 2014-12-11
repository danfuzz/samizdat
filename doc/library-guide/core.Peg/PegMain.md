Samizdat Layer 0: Core Library
==============================

core.Peg :: PegMain
-------------------

An instance of this class is a rule which provides a fresh (empty)
parsed item context for another rule. That is, the sub-rule, when called,
always gets `[]` for the `items` argument. The direct evaluation result of a
parser expression (`{: ... :}`) is always an instance of this class.


<br><br>
### Class Method Definitions

#### `class.new(rule) -> isa PegMain`

Creates an instance of this class which wraps the given `rule`.

#### `class.newChoice(rules*) -> isa PegMain`

Creates an instance of this class which wraps a `PegChoice` constructed
from the given `rules`.

#### `class.newSequence(rules*) -> isa PegMain`

Creates an instance of this class which wraps a `PegSequence` constructed
from the given `rules`.

<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
