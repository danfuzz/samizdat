Samizdat Layer 0: Core Library
==============================

core.Peg :: CacheState
----------------------

This class is a caching wrapper around an input generator. It provides the
guarantee that no value in the generator chain will ever have its
`.nextValue()` called on it more than once.

TODO: Eventually this class may grow into a packrat parser.


<br><br>
### Class Method Definitions

#### `class.new(input) -> isa CacheState`

Creates an instance of this class with an empty trailing context and the
given `input` as its input generator.

<br><br>
### Method Definitions: `ParserState` protocol.

Works as documented per the specification for the protocol, with the
following refinements:

#### `.applyRule(rule) -> isa CacheState`

Always calls `rule.parse(this)`.

#### `.shiftInput() -> isa CacheState`

Returns an instance just like this one, except with on one item from the
`input` shifted onto the trailing context. This class guarantees that
the underlying `input` generator will only have its `.nextValue()` method
called exactly once, no matter how many times this method is called.
