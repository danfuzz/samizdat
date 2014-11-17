Samizdat Language Guide
=======================

Parser Trees
------------

The node types and contents used in the parsed form of the parser syntax
have a fairly direct correspondence to the surface syntax and the
types and functions used to implement parsing.

### Normal (non-parsing) expression nodes

#### `@parser{pex}`

Representation of an anonymous parsing function. `pex` must be a parsing
expression node, that is, any of the node types defined here other than
this one.

Nodes of this type are the "glue" between execution trees and parser trees.

This corresponds to the syntax `{: pex :}`.


### Terminal parsing expression nodes

#### `@any{}`

Representation of the "match anything" rule.

This corresponds to the syntax `.`.

**Note:** The usual way to represent EOF is `@lookaheadFailure{pex: @any{}}`.

#### `@code{yieldDef?: name, statements: [statement*], yield: expression}`

Representation of a code expression.

* `yieldDef: name` (optional) &mdash; A name (typically a string) to bind
  as the nonlocal-exit function.

* `statements: [statement*]` &mdash; A list of statement nodes.
  Statement nodes are as defined by the Samizdat Layer 0 specification.

* `yield: expression` &mdash; An expression node representing the
  (local) result value for the code. Expression nodes are as defined
  by the Samizdat Layer 0 specification.

This corresponds to the syntax `{ <yieldDef> -> statement1; statement2;
etc; yieldExpression }`.

#### `@empty{}`

Representation of the "empty" ("always succeed" / no-op) rule.

This corresponds to the syntax `()`.

**Note:** The usual way to represent an unconditional failure is
`@lookaheadFailure{pex: @empty{}}`.

#### `@string{value: string}`

Representation of a multi-character sequence match. `string` must be a
string.

This corresponds to the syntax `"string"`.

#### `@thunk{value: expression}`

Representation of a parser thunk.

* `expression` &mdash; Regular (non-parsing) expression node,
  which when evaluated is expected to produce a parser.

This corresponds to the syntax `%expression`.

#### `@token{value: name}`

Representation of a token-match terminal. This is also used for
single-character matches in tokenizers. `value` must be a symbol.
This indicates that a token with the given name is to be matched.

This corresponds to the syntax `@token` or `"ch"` (where `ch` denotes
a single character).

#### `@tokenSet{values: [name*]}`

Representation of a token set rule. This is also used for matching
character sets in tokenizers. `values` must be a list of names (symbols),
which is taken to be an unordered set of token names to match.

This corresponds to the syntax `[@token1 @token2 @etc]` or `["charsToMatch"]`.

#### `@tokenSetComplement{values: [name*]}`

Representation of a token set complement rule. This is also used for matching
character set complements in tokenizers. `values` must be a list of names
(symbols), which is taken to be an unordered set of token names to not-match.

This corresponds to the syntax `[! @token1 @token2 @etc]` or
`[! "charsToNotMatch"]`.

#### `@varRef{name: name}`

Representation of a call out to a named parsing function. `name` is
an arbitrary value, but typically a string.

This corresponds to the syntax `name` (that is, a normal variable
reference).


### Non-terminal parsing expression nodes

#### `@choice{pexes: [pex*]}`

Representation of an ordered choice of items to match. Each element
of the list must be a parsing expression node.

This corresponds to the syntax `pex1 | pex2 | etc`.

#### `@lookaheadSuccess{pex}`

Representation of a lookahead-success expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `&pex`.

#### `@lookaheadFailure{pex}`

Representation of a lookahead-failure expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `!pex`.

#### `@opt{pex}`

Representation of an optional (zero-or-one) expression. `pex` must be a
parsing expression node.

This corresponds to the syntax `pex?`.

#### `@plus{pex}`

Representation of a plus (one-or-more) expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `pex+`.

#### `@sequence{pexes: [pexOrVarDef*]}`

Representation of a sequence of items to match, in order. Each element
of the list must be a parsing expression node or a `varDef` node.

This corresponds to the syntax `pex1 pex2 etc`.

#### `@star{pex}`

Representation of a star (zero-or-more) expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `pex*`.


### Other nodes

#### `@varDef{name: symbol, value: pex}`

Representation of a name-bound expression. These are *only* ever valid
as direct elements of the array of parsing nodes attached to a `sequence`
node. Anywhere else is an error.

* `name: symbol` &mdash; An arbitrary name.

* `value: pex` &mdash; A parsing expression node.

This corresponds to the syntax `name = pex`.
