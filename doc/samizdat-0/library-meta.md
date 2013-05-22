Samizdat Layer 0: Core Library
==============================

Meta-Library
------------

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

#### `sam0Eval context expressionNode <> . | !.`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document. It is valid for
the expression to yield void, in which case this function returns
void. Evaluation is performed in an execution context that includes
all of the variable bindings indicated by `context`, which must be a
map.

Very notably, the result of calling `sam0Tree` is valid as the
`expressionNode` argument here.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.
See `makeLibrary` for further suggestions and information.

#### `sam0Tokenize string <> [token*]`

Parses the given string as a series of tokens per the *Samizdat Layer 0*
token grammar. Returns a list of parsed tokens.

#### `sam0Tree program <> functionNode`

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

#### `makeLibrary context <> context`

Takes a library context (map of variable bindings) and returns one that
is just like the one given, except that the key `"LIBRARY"` is bound to
the given map. This makes a `LIBRARY` binding into a form suitable for
passing as the library / global context argument to evaluation
functions (such as `sam0Eval`), in that callees can rightfully expect
there to be a binding for `LIBRARY`.

#### `metaFunctions context fileName <> map`

Given a context (map of variable bindings, presumed to be a variant of
the core library) and a file name (a string), returns a map of the
functions to use to process that file as source code. The result is a
map that binds the three keys `eval` `tokenize` and `tree`. These are
bound to a matched set of functions for evaluating trees, tokenizing
text, and parsing text or token lists into trees (respectively).

This function works by noting `fileName`'s suffix, and matching it
against a mapping from suffixes to function names. If the suffix is
unrecognized, it assumes that the file is intended to be used with
the top language layer. It is an error (terminating the runtime)
if the file's suffix corresponds to a language layer not represented
in `context`.

#### `sam1Eval context expressionNode <> . | !.`

Returns the evaluation result of executing the given *Samizdat Layer 1*
expression node. This is just like `sam0Eval`, except for the specification
used to interpret the node.

Very notably, the result of calling `sam1Tree` is valid as the
`expressionNode` argument here.

#### `sam1Tokenize string <> [token*]`

*Samizdat Layer 1* tokenizer. This parses a string according to the
*Samizdat Layer 1* tokenization rules, returning a list of tokens,
per the *Samizdat Layer 1* specification.

This function is part of the bridge between layers 0 and 1.

#### `sam1Tree program <> functionNode`

*Samizdat Layer 1* tree parser. This parses a program according to the
*Samizdat Layer 1* parsing rules, returning a top-level function node,
per the *Samizdat Layer 1* specification. `program` must be either
a string or a list of tokens. If it is a string, then this function
builds in a call to `sam1Tokenize` to first convert it to a list of
tokens.

This function is part of the bridge between layers 0 and 1.

#### `samCommandLine context args* <> . | !.`

Command-line evaluator. This implements standardized top-level command-line
parsing and evaluation. `context` is expected to be a library context, such
as one of the standard `LIBRARY` bindings. `args` are arbitrary other
arguments, which are parsed as optional command-line options, a program
file name, and additional arguments.

This parses the indicated file, as implied by its recognized suffix
(or lack thereof), evaluates the result of parsing, and then calls that
evaluated result as a function, passing it first the "componentized"
path to itself (see `io0PathFromString`), and then any additional
arguments that were passed to this function. This function returns whatever
was returned by the inner function call (including void).

**Note:** There are currently no defined command-line options.
