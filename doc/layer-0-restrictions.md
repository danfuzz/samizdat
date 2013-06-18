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

In the surface syntax, a minus sign before a decimal digit is taken to
be part of the int literal. In Layer 2, such a minus sign is an operator.

In the surface syntax, underscores are not recognized as digit spacers
in int literals.

Strings
-------

Hexadecimal escapes are not recognized in string literals. Parsing of
these is implemented in Layer 2.
