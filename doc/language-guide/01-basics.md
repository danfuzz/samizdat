Samizdat Language Guide
=======================

The Basics
----------

Samizdat is a functional-forward (but not purely functional) programming
language, meant to be familiar to those steeped in the traditions of the
[C programming language](https://en.wikipedia.org/wiki/C_%28programming_language%29)
family, including C per se, [C++](https://en.wikipedia.org/wiki/C%2B%2B),
[Java](https://en.wikipedia.org/wiki/Java_%28programming_language%29),
and [JavaScript](https://en.wikipedia.org/wiki/JavaScript).

The goal of Samizdat is to be a compelling language choice when building
applications that need to take into account the realities of computing
across address spaces and over sometimes-high-latency communications links.

Samizdat is mostly an "expression language": Most syntactic constructs in
the language &mdash; including those which look like "statements" from the
C tradition &mdash; are in fact "expressions" that can be combined usefully
with any other expressions in the language.

### Comments

Samizdat has both single-line and multi-line comments.

A single-line comment starts with a number sign (`#`) and continues to the
end of the line it appears on.

```
#
# I am commentary.
thisIsNotCommentary   # ...but this *is* commentary.
```

A multi-line comment starts with the sequence slash-star (`/*`) and continues
to a matching star-slash (`*/`). Multi-line comments nest.

```
/* I am commentary. */ thisIsNotCommentary

/* This
is commentary. /* This too. */ Commentary
continues. */ thisIsNotCommentary
```

### Statements and Expressions

Most non-comment lines in a Samizdat program are part of one statement
or other, while *within* a statement most of the text is part of an
expression.

Statements are separated by semicolons (`;`). In addition (TODO), there
is implicit statement separation based on the indentation level of
adjacent lines.


### Variables

The most convenient way to name a variable in code is as a regular
"identifier". An regular identifier consists of an initial ASCII alphabetic
character (either lower- or upper-case) or initial underscore (`_`),
followed by zero or more other ASCII alphabetic characters, underscores, or
ASCII decimal digits.

As an escape hatch to name variables more arbitrarily, a variable name
can also be represented as an initial backslash (`\`) followed by a
double-quoted string literal. See the [Data](02-data.md) section for
details about double-quoted strings.

Variables are defined in two ways, either within a program body as local
variables, or as formal arguments to functions.

In general, a variable can be either mutable or immutable. Immutable
is typically more preferable.

#### Local variables

Within a program body, local variables are defined using the syntax:

```
def name = value
var name = value
```

where `name` is a variable name (per above), and `value` is an arbitrary
expression. `def` introduces an immutable variable binding, and
(TODO) `var` introduces a mutable variable binding.

(TODO) A mutable variable can be rebound using the syntax:

```
name = value
```

that is, an assignment without prefacing it with either `def` or `var`.

Variable definitions and assignments such as this are some of the few
statement forms in Samizdat. That is, these are not combining expressions.

#### Formal arguments

See the [Functions](03-functions.md) section for information about
formal arguments.
