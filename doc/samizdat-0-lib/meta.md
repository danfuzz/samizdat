Samizdat Layer 0: Core Library
==============================

Meta-Library
------------

<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

#### `LIBRARY`

A map of bindings of the entire *Samizdat Layer 0*
primitive library, except for the binding of `LIBRARY` itself (which can't
be done in that the data model doesn't allow self-reference).

**Note:** This is a constant map value, not a function.

**Note:** This binding as the *primitive* library is only available when
the in-language core library is first being loaded. When non-library code
is loaded, its `LIBRARY` binding is the full core library, including both
primitives and in-language definitions.

#### `sam0Eval(context, expressionNode) <> . | void`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document, using the *Layer 0*
evaluation rules. It is valid for the expression to yield void, in which
case this function returns void. Evaluation is performed in an execution
context that includes all of the variable bindings indicated by `context`,
which must be a map.

Very notably, the result of calling `sam0Tree` is valid as the
`expressionNode` argument here.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.
See `makeLibrary` for further suggestions and information.

#### `sam0Tokenize(string) <> [token*]`

Parses the given string as a series of tokens per the *Samizdat Layer 0*
token grammar. Returns a list of parsed tokens.

#### `sam0Tree(program) <> functionNode`

Parses the given program as a top-level program written in
*Samizdat Layer 0*. `program` must be either a string or a list of tokens.
If it is a string, then this function builds in a call to `sam0Tokenize`
to first convert it to a list of tokens.

Returns a `function` node, as defined by the *Samizdat Layer 0* parse tree
semantics, that represents the parsed program.


<br><br>
### In-Language Definitions

#### `LIBRARY`

A map of bindings of the entire *Samizdat Layer 0*
library, except for the binding of `LIBRARY` itself (which can't
be done in that the data model doesn't allow self-reference).

**Note:** This is a constant map value, not a function.

#### `makeLibrary(context) <> context`

Takes a library context (map of variable bindings) and returns one that
is just like the one given, except that the key `"LIBRARY"` is bound to
the given map. This makes a `LIBRARY` binding into a form suitable for
passing as the library / global context argument to evaluation
functions (such as `sam0Eval`), in that callees can rightfully expect
there to be a binding for `LIBRARY`.

#### `samCommandLine(context, args*) <> . | void`

Command-line evaluator. This implements standardized top-level command-line
parsing and evaluation. `context` is expected to be a library context, such
as one of the standard `LIBRARY` bindings. `args` are arbitrary other
arguments, which are parsed as optional command-line options, a program
file name, and additional arguments.

This parses the indicated file, as implied by its recognized suffix
(or lack thereof), evaluates the result of parsing, and then calls that
evaluated result as a function, passing it first the "componentized"
path to itself (see `io0PathFromFlat`), and then any additional
arguments that were passed to this function. This function returns whatever
was returned by the inner function call (including void).

Currently recognized command-line options:

* `--layer-0` &mdash; uses the layer 0 parser and evaluator.

#### `samEval(context, expressionNode) <> . | void`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document. It is valid for
the expression to yield void, in which case this function returns
void. Evaluation is performed in an execution context that includes
all of the variable bindings indicated by `context`, which must be a
map.

Very notably, the result of calling `samTree` is valid as the
`expressionNode` argument here.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.
See `makeLibrary` for further suggestions and information.

**Note:** This function gets bound and re-bound during system initialization,
as the higher language layers are hooked in. It is initially equivalent to
`sam0Eval`.

#### `samTokenize(string) <> [token*]`

Parses the given string as a series of tokens, per the *Samizdat*
token grammar. Returns a list of parsed tokens.

**Note:** This function gets bound and re-bound during system initialization,
as the higher language layers are hooked in. It is initially equivalent to
`sam0Tokenize`.

#### `samTree(program) <> functionNode`

Parses the given program as a top-level program written in
*Samizdat*. `program` must be either a string or a list of tokens.
If it is a string, then this function builds in a call to `samTokenize`
to first convert it to a list of tokens.

Returns a `function` node, as defined by the *Samizdat* parse tree
semantics, that represents the parsed program.

**Note:** This function gets bound and re-bound during system initialization,
as the higher language layers are hooked in. It is initially equivalent to
`sam0Tree`.
