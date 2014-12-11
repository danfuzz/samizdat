Samizdat Layer 0: Core Library
==============================

core.Generator :: SerialGenerator
---------------------------------

This class is a sequential generator wrapper. It wraps an arbitrary number
of other generators. It yields elements from the first wrapped generator,
until it is voided, and then moves on to the second, and so on. The instance
becomes voided after the final wrapped generator is voided.


<br><br>
### Class Method Definitions

#### `class.new(generators*) -> isa SerialGenerator | == nullGenerator`

Creates an instance of this class.

Special cases:
* If passed no arguments, this returns `nullGenerator`.
* If passed one argument, this returns that argument directly.

<br><br>
### Method Definitions: `Generator` protocol.

Works as documented per the specification for the protocol.
