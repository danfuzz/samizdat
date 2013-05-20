Samizdat Layer 1
================

Tree Semantics
--------------

The node types and contents used in the parsed form of the parser syntax
have a fairly direct correspondence to the surface syntax. Most of the
"operator" nodes have token types whose name is the same as the corresponding
syntactic operator. The highest layer of nodes have token types that are
(English) words, both to help distinguish them as well as to avoid the
problem that a "sequence" has no syntactic operator.

### Regular expression nodes

#### @["parser" pex]

Representation of an anonymous parsing function. `pex` must be a parsing
expression node, that is, any of the node types defined here other than
this one.

This corresponds to the syntax `{/ pex /}`.


### Terminal parsing expression nodes

#### @["."]

Representation of the "match anything" rule.

This corresponds to the syntax `.`.

#### @["()"]

Representation of the "always succeed" (no-op) rule.

This corresponds to the syntax `()`.

#### @["[]" charSet]

Representation of a character set rule. `charSet` must be a string, which
is taken to be an unordered set of characters.

This corresponds to the syntax `["charSet"]`.

#### @["[]" tokenSet]

Representation of a token set rule. `tokenSet` must be a list of tokens,
which is taken to be an unordered set of token types (token values are
ignored).

This corresponds to the syntax `[token1 token2 etc]`.

#### @["[!]" charSet]

Representation of a character set complement rule. `charSet` must be a string,
which is taken to be an unordered set of characters.

This corresponds to the syntax `[! "charSet"]`.

#### @["[!]" tokenSet]

Representation of a token set complement rule. `tokenSet` must be a list of
tokens, which is taken to be an unordered set of token types (token values
are ignored).

This corresponds to the syntax `[! token1 token2 etc]`.

#### @["{}" [("yieldDef"=name)? statements=[statement*] ("yield"=expression)?]]

Representation of a code expression.

* `"yieldDef"=name` (optional) &mdash; A name (typically a string) to bind
  as the nonlocal-exit function.

* `"statements"=[statement*]` (required) — A list of statement nodes.
  Statement nodes are as defined by the *Samizdat Layer 0* specification.

* `"yield"=expression` (optional) — An expression node representing the
  (local) result value for the code. Expression nodes are as defined
  by the *Samizdat Layer 0* specification.

This corresponds to the syntax `{ <yieldDef> :: statement1; statement2;
etc; <> yield }`.


### Non-terminal parsing expression nodes

#### @["choice" [pex*]]

Representation of an ordered choice of items to match. Each element
of the list must be a parsing expression node.

This corresponds to the syntax `pex1 | pex2 | etc`.

#### @["sequence" [pex*]]

Representation of a sequence of items to match, in order. Each element
of the list must be a parsing expression node.

This corresponds to the syntax `pex1 pex2 etc`.

#### @["varDef" ["name"=name "value"=pex]]

Representation of a name-bound expression.

* `"name"=name` (required) &mdash; An arbitrary literal name
  (typically a string).

* `"value"=pex` (required) &mdash; A parsing expression node.

This corresponds to the syntax `name = pex`.

#### @["&" pex]

Representation of a lookahead-success expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `&pex`.

#### @["!" pex]

Representation of a lookahead-failure expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `!pex`.

#### @["?" pex]

Representation of an optional (zero-or-one) expression. `pex` must be a
parsing expression node.

This corresponds to the syntax `pex?`.

#### @["*" pex]

Representation of a star (zero-or-more) expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `pex*`.

#### @["+" pex]

Representation of a plus (one-or-more) expression. `pex` must be a parsing
expression node.

This corresponds to the syntax `pex+`.

