Samizdat Layer 0: Core Library
==============================

core.Generator :: ListWrapGenerator
-----------------------------------

This class is a list-wrapping generator wrapper. This wraps a single other
generator, wrapping all values yielded by that generator as single-element
lists. This class is effectively a special case of both `ParaGenerator`
and `FilterGenerator`, implemented more efficiently than either of those.


<br><br>
### Class Method Definitions

#### `class.new(generator) -> isa ListWrapGenerator`

Creates an instance of this class.

<br><br>
### Method Definitions: `Generator` protocol.

Works as documented per the specification for the protocol.
