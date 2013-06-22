Samizdat Layer 0 Restrictions
=============================

*Samizdat 0* is the system in which the full Samizdat implementation is
written. It implements several "layers" of language, with *Samizdat Layer 0*
being the most basic language, and each subsequent layer being (practically
speaking) a strict superset of the layer that it is build directly upon.

This document describes how the various layers of *Samizdat 0* differ
from the full language.


Ints
----

Ints only have a 32-bit signed range, with out-of-range arithmetic
results causing failure, not wraparound.

In the surface syntax, base 10 is the only recognized form for int
constants. Layer 2 introduces syntax for hexadecimal and binary int
constants.

In the surface syntax, underscores are not recognized as digit spacers
in int literals.

Strings
-------

Neither hexadecimal nor entity escapes are recognized in string literals.
Parsing of these is implemented in Layer 2.

Variable Definition
-------------------

Only immutable variable definitions (`def name = ...`) are recognized.
TODO: Mutable variable definitions are implemented in Layer 2.

Parsing
-------

Parsing syntax (parsing blocks and parsing operators) is not recognized at
all. This is implemented in Layer 1.

Operators
---------

The only operators regognized in Layer 0 are:

* `-expr` &mdash; Unary negation, strictly limited to operating on ints.
* `expr*` &mdash; Interpolation.
* `expr..expr` &mdash; Ranges, but not chained ranges (`x..y..z`), which
  aren't all that useful anyway.
* `expr(expr, ...) { block } ...` &mdash; Function calls.
* `<> expr` &mdash; Local yield.
* `<out> exr` &mdash; Named nonlocal return.
* `return expr` &mdash; Function return.

Parsing expression operator syntax is implemented in Layer 1.
Full expression operator syntax is implemented in Layer 2.

Control Constructs
------------------

No control expressions are recognized (`if`, `do`, `while`, etc.). These
are implemented in Layer 2.

**Note:** Function statements and expressions *are* recognized in Layer 0.

