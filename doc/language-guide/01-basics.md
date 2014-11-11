Samizdat Language Guide
=======================

The Basics
----------

Samizdat is a functional-forward (but not purely functional) programming
language, meant to be familiar to those steeped in the traditions of the
[C programming
language](https://en.wikipedia.org/wiki/C_%28programming_language%29)
family, including C per se, [C++](https://en.wikipedia.org/wiki/C%2B%2B),
[Java](https://en.wikipedia.org/wiki/Java_%28programming_language%29),
and [JavaScript](https://en.wikipedia.org/wiki/JavaScript).

One major goal of Samizdat is to be a useful synchronous language, for use
in text processing and general parsing and data processing tasks such as is
accomplished using tools such as [Awk](http://en.wikipedia.org/wiki/AWK) or in
general purpose languages, sometimes with the support of a parsing framework
such as [ANTLR](http://en.wikipedia.org/wiki/ANTLR).

Another major goal of Samizdat &mdash; arguably its ultimate goal &mdash;
is to be a compelling language choice when building applications that need
to take into account the realities of computing across address spaces and
over sometimes-high-latency communications links.
(TODO: No actual concurrency or networking support has yet been implemented
or even fully specified.)

Samizdat is mostly an "expression language:" Most syntactic constructs in
the language &mdash; including those which look like "statements" from the
C tradition &mdash; are in fact "expressions" that can be combined usefully
with any other expressions in the language.


### Comments

Samizdat has both single-line and multi-line comments.

A single-line comment starts with a pair of hash marks `##` (also
known as "pound signs" or "number signs") and continues to the
end of the line it appears on.

In order to support Unix-style "shebang" script redirection,
a hash mark followed by an exclamation point `#!` is also treated as a
single-line comment start sequence.

```
#! I am commentary.
##
## I am also commentary.
thisIsNotCommentary   ## ...but this *is* commentary.
```

A multi-line comment starts with the sequence hash-colon (`#:`) and continues
to a matching colon-hash (`:#`). Multi-line comments nest.

```
#: I am commentary. :# thisIsNotCommentary

#: This
is commentary. #: This too. :# Commentary
continues. :# thisIsNotCommentary
```


### Directives

Samizdat has a simple syntax for single-line meta-program directives.
Directives, if present, are ignored during regular parsing; they take
effect *before* the main acts of tokenization and parsing are performed.

A directive starts with `#=` (hash then equal sign) at any column of a line
(not just in the first column), and it continues to the end of the line it
appears on. After the start mark and optional whitespace, an identifier
(simple name) indicates the name of the directive. The rest of the line
(trimmed of spaces on either side) is the "value" of the directive.

Currently, only one directive is implemented:

* `language fully.qualified.Name` &mdash; indicates the module name for
  the parser to use to interpret (or compile) the source file in which it
  appears. If present, this must be the first non-whitespace / non-comment
  item in the file.

  **Note:** This directive is mostly of use in the core library, which uses
  a few different versions of the parser as part of its bootstrap process.


### Statements and Expressions

Most non-comment lines in a Samizdat program are part of one statement
or other, while *within* a statement most of the text is part of an
expression. The distinction is that statements can only appear at
the "top level" of a program or function definition, while expressions
can directly contain other expressions, bottoming out at simple "terms"
such as variable names and literal constant values (such as numbers
and strings).

The various forms of statement and expression are described in more
detail in later sections of this guide.


### Variables

The most convenient way to name a variable in code is as a regular
"identifier." A regular identifier consists of an initial ASCII alphabetic
character (either lower- or upper-case), underscore (`_`), or dollar sign
(`$`); followed by zero or more other ASCII alphabetic characters,
underscores, dollar signs, or ASCII decimal digits.

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
def name { value }
```

where `name` is a variable name (per above), and `value` is an arbitrary
expression. `def name =` introduces an immutable variable binding, `var`
introduces a mutable variable binding, and `def name {` introduces a *lazy*
immutable variable binding.

It is valid to declare a variable (either mutable or immutable) but leave
it unbound, by omitting the `= value` part.

Mutable variables (whether bound or not) and declared-but-unbound and
immutable variables can be bound or rebound using the syntax:

```
name := value
```

that is, a `:=` assignment without prefacing it with either `def` or `var`.

Variable definitions (but *not* `:=` assignments) as described in
this section are some of the few statement forms in Samizdat. That is, these
are not combining expressions.

Lazy variables have their `value` body evaluated at most once, which occurs
at the first point when the variable's value is fetched. The body can have
any number of statements, and has the usual semantics of a nullary closure
(in particular, no explicit `yield` needed unless it contains a yield
declaration).

As part of a module definition, an immutable variable can be exported from
the module, by prefixing its definition with `export`:

```
export def name = value
```

It is also possible to export a previously-defined variable by just naming
it directly:

```
export name
```

**Note:** This form can interact surprisingly with *mutable* variable
definitions, because an `export` only exports a simple value and not a
mutable binding.

#### Formal arguments

See the [Functions](03-functions.md) section for information about
formal arguments.

### Void and Any (`.`)

Samizdat has the concept of "void" meaning the lack of a value.
This is in contrast to many other languages which define one or
more distinguished values as meaning "nothingness" while still
actually being "something."

For example, it is possible for functions in Samizdat to return without
yielding a value. Such functions are referred to as "void functions," and
one can say that such a function "returns void" or "returns a void result."

There are three major restrictions on how voids can be used:

* A void cannot be stored in a variable.

* A void cannot take part in a function application. An attempt to
  apply void as a function is a fatal error, as is an attempt to pass
  a void argument to a function. See the section on "Function Application" for
  more details.

* A void cannot be part of a data structure (e.g., a list element,
  or a map key or value).

The Samizdat library provides several facilities to help deal with
cases where code needs to act sensibly without knowing up-front whether
or not a given expression will yield a value. The facilities notably
include `if` expressions and the question mark (`?`) and star (`*`)
postfix operators.

**Note:** When describing functions in this (and related) documents,
a void result is written `void`, though that is not part of the language
syntax per se.

Relatedly, a result that is a value but without any further specifics is
written as `.`. If a function can possibly return a value *or* return void,
that is written as `type | void` or `. | void`. Again, while not proper
syntax in the language per se, these are meant to be suggestive of the
syntax used in function argument declaration and in defining parsers.

### Logic operations

Logic operations in Samizdat &mdash; things like `if`s, logical
conjunctions and disjunctions, loop tests, and so on &mdash;
are based on the idea that a value &mdash; any value &mdash; counts
as logical-true, whereas void &mdash; that is, the lack of a
value &mdash; counts as logical-false.

Samizdat also has boolean values `true` and `false`, which are used
for boolean values "at rest," such as in variables or data structures.
There are operators in the language and library support functions that
aid in the conversion between logical and boolean truth values. See later
sections for details.

By convention, if a logic expression or function has no better non-void
value to return, it will use the boolean value `true`.

While this is a bit of a departure from the object-safe languages in the
C family (such as Java and JavaScript), it is actually close to how C
itself operates, in that void in Samizdat is very close in meaning to
`NULL` in C. Samizdat departs from C in that void and `false` are
not equivalent, and in that `void` is not ever allowed to be stored in
a variable or passed as an argument. Somewhat confusingly &mdash; but
necessary to avoid certain ambiguities &mdash; the boolean value `false`
counts as true, as far as logical expressions are concerned.
