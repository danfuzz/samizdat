Samizdat Language Guide
=======================

Appendix: Lower Layer Implementation Restrictions
-------------------------------------------------

The langauge layers 0 through 2 are used to build and bootstrap the rest
of the system. Samizdat Layer 0 is the most basic language, and each
subsequent layer is (practically speaking) a strict superset of the layer
that it is built directly upon.

This document describes how these various lower layers of differ
from the full language.

### The Layers

#### Layer 0

Samizdat Layer 0 is a "parti" of the final language layer. That is, it
embodies *just* the main thrusts of the language with very little
embellishment.

The goal is that code written in this layer be recognizably Samizdat,
even while eschewing niceties such as most operators and control constructs.

#### Layer 1

The sole purpose of Samizdat Layer 1 is to introduce parsing syntax
into the language.

#### Layer 2

Samizdat Layer 2 adds a more complete set of syntactic constructs to
the main language, including functional operators (e.g. math operations),
control constructs, and a bit more variety in expressing literal data.

### The Restrictions

#### Comments

In Layers 0 and 1, only single-line comments are recognized. Layer 2
introduces multi-line comment syntax (`#: ... :#`).

#### Semicolons

In Layer 0, only single semicolons (not multiple) are allowed between
statements.

#### Ints

Ints only have a 64-bit signed range in Layers 0, 1, and 2. Out-of-range
arithmetic results cause failure, not wraparound.

In the surface syntax, base 10 is the only recognized form for int
constants in Layer 0. Layer 2 introduces syntax for hexadecimal and
binary int constants.

#### Strings

In Layer 0, the only backslash escapes that are recognized are the
trivial one-character ones. *Not* included are hexadecimal escapes,
entity escapes, `\/`, escaped newlines, or string interpolation.
Handling of all of these is implemented in Layer 2.

In addition, literal newlines are not ever valid inside string literals in
Layer 0. Handling of *unescaped* newlines is implemented in Layer 1.

#### Parsing

Parsing syntax (parsing blocks and parsing operators) is not recognized at
all in Layer 0. This is implemented in Layer 1.

#### Operators

The only operators recognized in Layer 0 are:

* `expr(expr, ...) { block } ...` &mdash; Function calls.
* `expr.name(expr, ...) { block } ...` &mdash; Method calls.
* `expr::name` &mdash; Collection-style indexing by name.
* `expr*` &mdash; Interpolation.
* `expr?` &mdash; Optional-value-to-list conversion.
* `name := expr` &mdash; Variable assignment.
* `expr* := expr` &mdash; Box assignment.

In addition, Layer 1 recognizes:

* `expr.name` &mdash; Getter method call.
* `expr.name := expr` &mdash; Setter method call.

**Note:** All yield statements (`yield`, `yield /name`, `break`, `continue`,
and `return`) are available at all layers, though `break` and `continue`
aren't particularly useful in layers 0 or 1.

**Note:** Negative int constants (e.g. `-1`) are recognized in all layers,
but unary negation as an operator is only introduced in Layer 2.

#### Control Constructs

No control expressions are recognized (`if`, `do`, `while`, etc.) in Layer 0.
These are implemented in Layer 2.

**Note:** `fn` statements and expressions *are* recognized in Layer 0.

