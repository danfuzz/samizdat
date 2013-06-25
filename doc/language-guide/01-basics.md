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
expression. The distinction is that statements can only appear at
the "top level" of a program or function definition, while expressions
can directly contain other expressions, bottoming out at simple "atoms"
such as variable names and literal constant values (such as numbers
and strings).

The various forms of statement and expression are described in more
detail in later sections of this guide.


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

### Void, `.`, and `!.`

*Samizdat* has the concept of "void" meaning the lack of a value.
This is in contrast to many other languages which define one or
more distinguished values as meaning "nothingness" while still
actually being "something".

For example, it is possible for functions in *Samizdat* to return without
yielding a value. Such functions are referred to as "void functions", and
one can say that such a function "returns void" or "returns a void result".

There are three major restrictions on how voids may be used:

* A void cannot be stored in a variable.

* A void cannot take part in a function application. An attempt to
  apply void as a function is a fatal error, and an attempt to pass
  a void argument causes short-circuit (but non-fatal) termination
  of the application. See the section on "Function Application" for
  more details.

* A void cannot be part of a data structure (e.g., a list element,
  or a map key or value).

The *Samizdat* library provides several facilities to help deal with
cases where code needs to act sensibly without knowing up-front whether
or not a given expression will yield a value. The facilities notably
include `if` expressions and the question mark (`?`) and star (`*`)
postfix operators.

**Note:** When describing functions in this (and related) documents,
a void result is written `!.`, which can be read as "not anything"
(or with more technical accuracy, "the failure to match any value").
Relatedly, a result that is a value but without any further specifics is
written as `.`. If a function can possibly return a value *or* return void,
that is written as `. | !.`. While not proper syntax in the language per
se, these are meant to be suggestive of the syntax used in function
argument declaration and in defining parsers.

### Logic operations

Logic operations in *Samizdat* &mdash; things like `if`s, logical
conjunctions and disjunctions, loop tests, and so on &mdash;
are based on the idea that a value &mdash; any value &mdash; counts
as logical-true, whereas void &mdash; that is, the lack of a
value &mdash; counts as logical-false.

*Samizdat* also has boolean values `true` and `false`, which are used
for boolean values "at rest", such as in variables or data structures.
There are operators in the language and library support functions that
aid in the conversion between logical and boolean truth values. See later
sections for details.

By convention, if a logic expression or function has no better non-void
value to return, it will use the boolean value `true`.

While this is a bit of a departure from the object-safe languages in the
C family (such as Java and JavaScript), it is actually close to how C
itself operates, in that void in *Samizdat* is very close in meaning to
`NULL` in C. *Samizdat* departs from C in that void and `false` are
not equivalent. Somewhat confusingly &mdash; but necessary to avoid certain
ambiguities &mdash; the boolean value `false` counts as true, as
far as logical expressions are concerned.
