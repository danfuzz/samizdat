Samizdat Layer 0: Core Library
==============================

core.Code
---------

This module defines the most basic code execution facilities of
Samizdat.

<br><br>
### Method Definitions

(none)


<br><br>
### Function Definitions

#### `eval(env, expressionNode) -> . | void`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document, converted for
execution by a call to `simplify()` or similar.

Evaluation is performed in an execution environment that includes all of the
variable bindings indicated by `env`, which must be a symbol table.
It is recommended (but not required) that the given `env` include
bindings for all of the library functions specified by the library guide.

If `expressionNode` is a `@maybe`, then it is valid for the expression to
evaluate to void, in which case this function returns void. If any other node
type is passed, then it is invalid for it to evaluate to void; doing so is
a fatal error (terminating the runtime).

Very notably, the result of calling `simplify(parseProgram(code), resolveFn)`
is valid as the `expressionNode` argument here.

#### `evalBinary(env, filePath) -> . | void`

Evaluates the named compiled file. `filePath` is expected to name
a file in the (platform-dependent) binary library format. The file
is loaded, and its `eval` function is called, passing it the given
`env`. The return value of this function is the result of the `eval`
call.

The usual case is for a binary to evaluate to a function definition,
most typically one that takes no arguments. This is parallel to what
results from evaluating a program tree using `eval` (above).

It is an error (terminating the runtime) if the file does not exist,
is not a library file, or is missing necessary bindings.
