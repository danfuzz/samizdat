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

#### `class.new(generator) -> :CacheState`

Creates an instance of this class. As a generator, it generates items from
the given `generator`, with the guarantee as stated in the intro (above).

When first constructed, this will call `generator.nextValue()` once. As
a special case, if `generator` turns out to be voided, this constructor
returns `voidState`.

<br><br>
### Method Definitions: `ParserState` protocol.

Works as documented per the specification for the protocol, with the
following refinements:

#### `.applyRule(rule, box, items) -> newState`

Always calls `rule.parse(box, this, items)`.

#### `.nextValue(box) -> :ParserState`

Stores into `box` the next generated element. Returns another instance
of this class if that instance would not be voided. Returns `voidState` at
the end of the generator chain.
