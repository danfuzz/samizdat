Samizdat Layer 0: Core Library
==============================

core.Generator :: RepeatGenerator
---------------------------------

This class is a repeated-value generator, which repeatedly generates a
given value a finite number of times before becoming voided.


<br><br>
### Class Method Definitions

#### `class.new(size, optValue?) -> isa SerialGenerator | == nullGenerator`

Creates an instance of this class. If `optValue` is not passed, it is
taken to be `null`.

Special cases:
* If passed `0` for `size`, this returns `nullGenerator`.

**Syntax Note:** Used in the translation of `for` and comprehension forms.

<br><br>
### Method Definitions: `Generator` protocol.

Works as documented per the specification for the protocol.
