Samizdat Layer 0
================

Tree Semantics
--------------

These are all the node types and other data types that can occur in a
parse tree, such that it can be compiled (or "compile-equivalent"ed) by
`sam0Eval` (and the like). These are presented in a form meant to be
representative of how one would construct them in the source syntax of
*Samizdat Layer 0*.

<br><br>
### Expression Nodes

Each of these node types can appear anywhere an "expression"
is called for.

#### `call` &mdash; `[:@call @[@function=function @actuals=@[actual*]:]`

* `@function=function` (required) &mdash; An expression node that must
  evaluate to a function.

* `@actuals=@[actual*]` (required) &mdash; A listlet of arbitrary expression
  nodes, each of which must evaluate to a non-void value.

This represents a function call.

When run, first `function` and then the elements of `actuals` (in
order) are evaluated. If `function` evaluates to something other than
a function, the call fails (terminating the runtime). If any of the
`actuals` evaluates to void, the call fails (terminating the runtime).

If there are too few actual arguments for the function (e.g., the
function requires at least three arguments, but only two are passed),
then the call fails (terminating the runtime).

With all the above prerequisites passed, the function is applied to
the evaluated actuals as its arguments, and the result of evaluation
is the same as whatever was returned by the function call (including
void).

#### `function` &mdash; `[:@function @[(@formals=@[formal+])? (@yieldDef=name)?`
#### `@statements=@[statement*] (@yield=expression)?:]`

* `@formals=@[formal+]` (optional) &mdash; An array of `formal`
  elements (as defined below). This defines the formal arguments to
  the function.

* `@yieldDef=name` (optional) &mdash; A name (typically a stringlet) to
  bind as the nonlocal-exit function.

* `@statements=@[statement*]` (required) &mdash; A listlet of statement
  nodes. A statement node must be either an expression node or a
  `varDef` node (as defined below). This defines the bulk of the
  code to execute.

* `@yield=expression` (optional) &mdash; An expression node representing
  the (local) result value for a call.

This represents a function definition.

When run, a closure (representation of the function as an in-model
value) is created, which binds as variables the names of all
the formals to all the incoming actual arguments (as defined below),
binds the `yieldDef` name if specified to a nonlocal-exit function,
and binds all other variable names to whatever they already refer to in
the lexical evaluation context. This closure is the result of evaluation.

If a nonlocal-exit function is defined, then that function accepts zero
or one argument. When called, it causes an immediate return from the active
function that it was bound to, yielding as a result whatever was passed to
it (including void). It is an error (terminating the runtime) to use a
nonlocal-exit function after the active function it was bound to has exit.

When the closure is actually called (e.g. by virtue of being the
designated `function` in a `call` node), a fresh execution context is
created, in which the actual arguments are bound to the formals. If
there are too few actual arguments, the call fails (terminating the
runtime). After that, the `statements` are evaluated in
order. Finally, if there is a `yield`, then that is evaluated. The
result of the call is the same as the result of the `yield` evaluation
(including possibly void) if a `yield` was present, or void if
there was no `yield` to evaluate.

**Note:** As a possible clarification about nonlocal-exit functions: Defining
and using these amounts to something along the lines of `try` / `catch` in
systems that are defined using those terms. In C terms, the facility is
along the lines of `setjmp` / `longjmp`. In Lisp terms, the facility is
an implementation of downward-passing continuations.

#### `literal` &mdash; `[:@literal value:]`

* `value` (required) &mdash; Arbitrary data value.

This represents arbitrary literal data.

The data `value` is the result of evaluation,

When a `literal` node is run, the result of evaluation is `value`.
Evaluation never fails.

#### `varRef` &mdash; `[:@varRef name:]`

* `name` (required) &mdash; Name of a variable (typically a stringlet).

This represents a by-name variable reference.

When run, this causes the `name` to be looked up in the current
execution context. If a binding is found for it, then the bound value
is the result of evaluation. If a binding is not found for it, then
evaluation fails (terminating the runtime).

<br><br>
### Other Nodes and Values

These are nodes and values that appear within the data payloads
of various expression nodes.

#### `formal` &mdash; `@[@name=name (@repeat=repeat)?]`

* `@name` (required) &mdash; an arbitrary value (but typically a stringlet),
  which indicates the name of the variable to be bound for this
  formal.

* `@repeat` (optional) &mdash; indicates (if present) that the number of
  actual arguments bound by this formal is not necessarily exactly one.
  If present it must be one of:

  * `@"*"` &mdash; indicates that this formal binds as many actual
    arguments as are available, including none. As such, this only really
    makes sense as the `repeat` of the last formal, though the surface syntax
    will tolerate it being in any position. The bound argument variable
    becomes a listlet of all the passed actual arguments that were bound.

  * `@"?"` &mdash; indicates that this formal binds a single argument if
    available, including none. As such, this only really makes sense if
    only ever followed by other `?` formals and possibly a final `*` formal,
    though the syntax will tolerate it being in any position. The bound
    argument variable becomes a listlet, either of size one if an argument
    was bound or of size zero if not.

If no `@repeat` is specified, then the given formal binds exactly one
actual argument. The argument variable as bound is the same as the
actual argument as passed (no extra wrapping).

#### `varDef` &mdash; `[:@varDef @[@name=name @value=expression]:]`

* `@name=name` &mdash; Variable name to define (typically a stringlet).

* `@value=expression` &mdash; Expression node representing the value
  that the variable should take on when defined.

This represents a variable definition statement as part of a function body.

When run, the `value` expression is evaluated. If it evaluates to void,
then evaluation fails (terminating the runtime). Otherwise, the evaluated
value is bound in the current (topmost) execution context to the
indicated `name`.
