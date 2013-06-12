Samizdat Layer 1
================

Tree Semantics
--------------

The node types and contents used in the parsed form of the parser syntax
have a fairly direct correspondence to the surface syntax. Most of the
nodes have token types whose name is the exactly the corresponding
syntactic operator or reminiscent of the corresponding syntactic forms.
As exceptions, the highest layer of nodes have token types that are
(English) words, both to help distinguish them as well as to avoid the
problem that a "sequence" has no syntactic operator; and the most basic
token and string terminals are named `token` and `string`.

### Normal (non-parsing) expression nodes

#### `@["parser": pex]`

Representation of an anonymous parsing function. `pex` must be a parsing
expression node, that is, any of the node types defined here other than
this one.

This corresponds to the syntax `{/ pex /}`.


### Terminal parsing expression nodes

#### `@["string": string]`

Representation of a multi-character sequence match. `string` must be a
string.

This corresponds to the syntax `"string"`.

#### `@["token": type]`

Representation of a token-match terminal. This is also used for
single-character matches in tokenizers. `type` must be a string. This
indicates that a token of the given type is to be matched.

This corresponds to the syntax `@token` or `"ch"` (where `ch` denotes
a single character).

#### `@["varRef": name]`

Representation of a call out to a named parsing function. `name` is
an arbitrary value, but typically a string.

This corresponds to the syntax `name` (that is, a normal variable
reference).

#### `@["."]`

Representation of the "match anything" rule.

This corresponds to the syntax `.`.

#### `@["()"]`

Representation of the "always succeed" (no-op) rule.

This corresponds to the syntax `()`.

#### `@["[]": tokenSet]`

Representation of a token set rule. This is also used for matching
character sets in tokenizers. `tokenSet` must be a list of strings,
which is taken to be an unordered set of token types to match.

This corresponds to the syntax `[token1 token2 etc]` or `["charsToMatch"]`.

#### `@["[!]": tokenSet]`

Representation of a token set complement rule. This is also used for matching
character set complements in tokenizers. `tokenSet` must be a list of
strings, which is taken to be an unordered set of token types to not-match.

This corresponds to the syntax `[! token1 token2 etc]` or
`[! "charsToNotMatch"]`.

#### `@["{}": [("yieldDef": name)?, statements: [statement*], ("yield": expression)?]]`

Representation of a code expression.

* `"yieldDef": name` (optional) &mdash; A name (typically a string) to bind
  as the nonlocal-exit function.

* `"statements": [statement*]` (required) — A list of statement nodes.
  Statement nodes are as defined by the *Samizdat Layer 0* specification.

* `"yield": expression` (optional) — An expression node representing the
  (local) result value for the code. Expression nodes are as defined
  by the *Samizdat Layer 0* specification.

This corresponds to the syntax `{ <yieldDef> :: statement1; statement2;
etc; <> yield }`.

#### `@["&&": expression]`

Representation of a predicate. `expression` must be a regular (non-parsing)
expression node, which is taken to be the predicate to test.

This corresponds to the syntax `&&(expression)`.

### Non-terminal parsing expression nodes

#### `@["choice": [pex*]]`

Representation of an ordered choice of items to match. Each element
of the list must be a parsing expression node.

This corresponds to the syntax `pex1 | pex2 | etc`.

#### `@["sequence": [pexOrVarDef*]]`

Representation of a sequence of items to match, in order. Each element
of the list must be a parsing expression node or a `varDef` node.

This corresponds to the syntax `pex1 pex2 etc`.

#### `@["&": pex]`

Representation of a lookahead-success expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `&pex`.

#### `@["!": pex]`

Representation of a lookahead-failure expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `!pex`.

#### `@["?": pex]`

Representation of an optional (zero-or-one) expression. `pex` must be a
parsing expression node.

This corresponds to the syntax `pex?`.

#### `@["*": pex]`

Representation of a star (zero-or-more) expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `pex*`.

#### `@["+": pex]`

Representation of a plus (one-or-more) expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `pex+`.

### Other nodes

#### `@["varDef": ["name": name, "value": pex]]`

Representation of a name-bound expression. These are *only* ever valid
as direct elements of the array of parsing nodes attached to a `sequence`
node. Anywhere else is an error.

* `"name": name` (required) &mdash; An arbitrary literal name
  (typically a string).

* `"value": pex` (required) &mdash; A parsing expression node.

This corresponds to the syntax `name = pex`.
