Samizdat Layer 0: Core Library
==============================

core.Lang*
----------

The modules `core.Lang*` (where `*` is a digit) implement the variants
of Samizdat language parsing and evaluation. There are modules for
layers 0, 1, and 2.

**Note:** In naming types here, `ExpressionNode` refers to arbitrary
expression node types.

<br><br>
### Functions

#### `languageOf(string) -> is String | void`

Finds and returns the language module directive in the given string,
which is assumed to be a Samizdat program. If there is no directive
present, this returns void.

#### `parseExpression(expression) -> is ExpressionNode`

Parses the given `expression` as an expression in the language.
`expression` must be either a string or a list of tokens. If it is a string,
then this function builds in a call to `tokenize` to first convert it
to a list of tokens.

Returns an expression node, as defined by the corresponding parse tree
semantics, that represents the parsed expression.

#### `parseProgram(program) -> is ExpressionNode`

Parses the given `program` as a top-level program written in the language.
`program` must be either a string or a list of tokens. If it is a string,
then this function builds in a call to `tokenize` to first convert it to a
list of tokens.

Returns a `closure` node, as defined by the corresponding parse tree
semantics, that represents the parsed program.

#### `simplify(expressionNode, resolveFn) -> is ExpressionNode`

Converts and simplifies the given node into a form usable by
`core.Code::eval`. This will call `resolveFn` to resolve the imports
found in any encountered `import*` nodes.

In `Lang0`, this is a no-op on everything but top-level program nodes.
On program nodes, it resolves imports and converts all `export*`
nodes into a `yield`ed result.

In `Lang1` and `Lang2`, in addition to the above, this converts parser
expression nodes into appropriate calls to `Peg` functions.

#### `tokenize(string) -> is List`

Parses the given `string` as a series of whitespace-delimited tokens, per the
corresponding token grammar specification. Returns a list of parsed tokens.
