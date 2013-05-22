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
