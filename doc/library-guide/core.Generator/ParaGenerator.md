Samizdat Layer 0: Core Library
==============================

core.Generator :: ParaGenerator
-------------------------------

This class is a parallel generator wrapper. It wraps an arbitrary number of
other generators, and yields values from all of them in parallel, as lists.
Each yielded list consists of values yielded from the individual generators,
in passed order. The generator becomes voided when *any* of the individual
generators is voided.


<br><br>
### Class Method Definitions

#### `class.new(generators*) -> isa ParaGenerator | isa ListWrapGenerator | == nullGenerator`

Creates an instance of this class.

Special cases:
* If passed no arguments, this returns `nullGenerator`.
* If passed one argument, this returns an instance of `ListWrapGenerator`.

**Syntax Note:** Used in the translation of `for` forms.

<br><br>
### Method Definitions: `Generator` protocol.

Works as documented per the specification for the protocol.
