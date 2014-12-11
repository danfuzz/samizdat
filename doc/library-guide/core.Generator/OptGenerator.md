Samizdat Layer 0: Core Library
==============================

core.Generator :: OptGenerator
------------------------------

This class is an "optional" generator. It wraps an arbitrary other generator,
yielding values from that generator, wrapped as single-element lists. Once
the wrapped generator is voided, this instance yields the empty list, ad
infinitum, never becoming voided.


<br><br>
### Class Method Definitions

#### `class.new(generator) -> isa OptGenerator`

Creates an instance of this class.

<br><br>
### Method Definitions: `Generator` protocol.

Works as documented per the specification for the protocol.
