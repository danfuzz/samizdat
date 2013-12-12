Samizdat Layer 0: Core Library
==============================

core::Lang*
-----------

The modules `core::Lang*` (where `*` is a digit) implement the variants
of Samizdat language parsing and evaluation. There are modules for
layers 0, 1, and 2.

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Function Definitions

#### `eval(context, expressionNode) <> . | void`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document. It is valid for the
expression to yield void, in which case this function returns void.
Evaluation is performed in an execution context that includes all of
the variable bindings indicated by `context`, which must be a map.

Very notably, the result of calling `parseProgram` is valid as the
`expressionNode` argument here.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.

#### `evalBinary(context, filePath) <> . | void`

Evaluates the named compiled file. `filePath` is expected to name
a file in the (platform-dependent) binary library format. The file
is loaded, and its `eval` function is called, passing it the given
`context`. The return value of this function is the result of the `eval`
call.

The usual case is for a binary to evaluate to a function definition,
most typically one that takes no arguments. This is parallel to what
results from evaluating a program tree using `eval` (above).

It is an error (terminating the runtime) if the file does not exist,
is not a library file, or is missing necessary bindings.

**Note:** The `Lang0` and `Lang1` versions of this function accept a
platform-dependent flat path string as the `filePath` argument. Higher
language layers accept standard structured paths (lists of path components).

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
