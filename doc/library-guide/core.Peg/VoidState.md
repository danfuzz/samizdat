Samizdat Layer 0: Core Library
==============================

core.Peg :: VoidState
---------------------

This class represents a voided `ParserState` value.


<br><br>
### Method Definitions: `ParserState` protocol.

Works as documented per the specification for the protocol, with the
following refinements:

#### `.applyRule(rule, box, items) -> newState`

Always calls `rule.parse(box, this, items)`.

#### `.nextValue(box) -> void`

Always returns void and takes no other action.
