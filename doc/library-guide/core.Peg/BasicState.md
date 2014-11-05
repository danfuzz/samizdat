Samizdat Layer 0: Core Library
==============================

core.Peg :: BasicState
----------------------

This class is the thinnest possible wrapper around an input generator,
providing the `ParserState` protocol.


<br><br>
### Class Method Definitions

#### `class.new(generator) -> :BasicState`

Creates an instance of this class. As a generator, it generates items from
the given `generator`, with `.nextValue()` always returning another instance
of this class, until voided.


<br><br>
### Method Definitions: `ParserState` protocol.

Works as documented per the specification for the protocol, with the
following refinements:

#### `.applyRule(rule, box, items) -> newState`

Always calls `rule.parse(box, this, items)`.

