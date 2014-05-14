Samizdat Layer 0: Core Library
==============================

core.Lang*
----------

The modules `core.Lang*` (where `*` is a digit) implement the variants
of Samizdat language parsing and evaluation. There are modules for
layers 0, 1, and 2.

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Function Definitions

#### `convertToLang0(expressionNode) <> expressionNode`

Converts any higher-layer node types within the given expression into
Layer 0 forms.

In `Lang0`, this is a no-op. In `Lang1` and `Lang2` this converts parser
expression nodes into appropriate calls to `Peg` functions.

#### `languageOf(string) <> string | void`

Finds and returns the language module directive in the given string,
which is assumed to be a Samizdat program. If there is no directive
present, this returns void.

#### `parseExpression(expression) <> expressionNode`

Parses the given `expression` as an expression in the language.
`expression` must be either a string or a list of tokens. If it is a string,
then this function builds in a call to `tokenize` to first convert it
to a list of tokens.

Returns an expression node, as defined by the corresponding parse tree
semantics, that represents the parsed expression.

#### `parseProgram(program) <> functionNode`

Parses the given `program` as a top-level program written in the language.
`program` must be either a string or a list of tokens. If it is a string,
then this function builds in a call to `tokenize` to first convert it to a
list of tokens.

Returns a `function` node, as defined by the corresponding parse tree
semantics, that represents the parsed program.

#### `tokenize(string) <> [token*]`

Parses the given `string` as a series of whitespace-delimited tokens, per the
corresponding token grammar specification. Returns a list of parsed tokens.
