Samizdat Layer 0: Core Library
==============================

core.Generator :: FilterGenerator
---------------------------------

This class is a filtering generator. It wraps any number of arbitrary other
generators, which it calls upon to generate values whenever it needs to
produce a value. These generated values are processed by a filtering function
to produce this class's values. This works as follows:

Each time `.nextValue()` is called on an instance of this class, it calls
`.nextValue()` on its wrapped generators. If any of the argument generators
has been voided, then this instance also becomes voided.

Otherwise, the values yielded from the inner generators are passed to the
`filterFunction` as its arguments (in generator order). If that function
returns a value, then that value in turn becomes the yielded result of
the outer generator. If the filter function yields void, then the
value-in-progress is discarded, and the inner generator is retried, with
the same void-or-value behavior.


<br><br>
### Class Method Definitions

#### `class.new(filterFunction, generators*) -> isa FilterGenerator`

Creates an instance of this class.

**Syntax Note:** Used in the translation of comprehension forms.

<br><br>
### Method Definitions: `Generator` protocol.

Works as documented per the specification for the protocol.
