Samizdat Layer 0: Core Library
==============================

core.Peg :: BasicState
----------------------

This class is the thinnest possible wrapper around an input generator,
providing the `ParserState` protocol.


<br><br>
### Class Method Definitions

#### `class.new(input) -> isa BasicState`

Creates an instance of this class, with an empty trailing context and the
given `input` as its input generator.


<br><br>
### Method Definitions: `ParserState` protocol.

Works as documented per the specification for the protocol, with the
following refinements:

#### `.applyRule(rule) -> isa ParserState`

Always calls `rule.parse(this)`.
