Samizdat Layer 0
================

Tree Semantics
--------------

These are all the node types and other data types that can occur in a
parse tree, such that it can be compiled (or "compile-equivalent"ed) by
`sam0Eval` (and the like, with caveats as noted). These are presented in
a form meant to be representative of how one would construct them in the
source syntax of *Samizdat Layer 0*.

<br><br>
### Expression Nodes

Each of these node types can appear anywhere an "expression"
is called for.

#### `call` &mdash; `@[call: [function: expression, actuals: [expression*]]`

* `function: expression` (required) &mdash; An expression node that must
  evaluate to a function.

* `actuals: [expression*]` (required) &mdash; A list of arbitrary
  expression nodes (each of which must evaluate to a non-void value)
  or `interpolate` nodes (each of which must evaluate to a list).

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

#### `closure` &mdash; `@[closure: [(formals: [formal+])?, (yieldDef: name)?,` `statements: [statement*], (yield: expression)?]`

* `formals: [formal+]` (optional) &mdash; An array of `formal`
  elements (as defined below). This defines the formal arguments to
  the function.

* `yieldDef: name` (optional) &mdash; A name (typically a string) to
  bind as the nonlocal-exit function.

* `statements: [statement*]` (required) &mdash; A list of statement
  nodes. A statement node must be either an expression node, or a
  `fnDef` or `varDef` node (as defined below). This defines the bulk of
  the code to execute.

* `yield: expression` (optional) &mdash; An expression node representing
  the (local) result value for a call.

This represents a closure (anonymous function) definition.

When run, a closure function (representation of the closure as an in-model
value) is created, which binds as variables the names of all
the formals to all the incoming actual arguments (as defined below),
binds the `yieldDef` name if specified to a nonlocal-exit function,
and binds all other variable names to whatever they already refer to in
the lexical evaluation context. This closure function is the result of
evaluation.

If a nonlocal-exit function is defined, then that function accepts zero
or one argument. When called, it causes an immediate return from the active
function that it was bound to, yielding as a result whatever was passed to
it (including void). It is an error (terminating the runtime) to use a
nonlocal-exit function after the active function it was bound to has exit.

When the closure function is actually called (e.g. by virtue of being the
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
an implementation of downward-passed / upward-called continuations.

#### `expression` &mdash; `@[expression: expression]`

* `expression` &mdash; Arbitrary expression node.

A node with type `expression` per se is merely a pass-through to its
enclosed expression. In most positions, it is effectively a no-op.
However, it is useful to disambiguate argument or list element
interpolation from regular expression interpolation.

For example, in terms of surface syntax, `foo(bar*)` is a call to `foo`
with an interpolation of all of `bar`'s (zero or more) elements as
arguments, whereas `foo((bar*))` is a single-argument call to `foo`,
with the implied (and asserted) a single element of list `bar` as the
argument.

TODO: Not yet implemented.

#### `interpolate` &mdash; `@[interpolate: expression]`

* `expression` &mdash; Expression node, which must yield a list when
  evaluated.

This represents the interpolation of a list, either as an expression per
se or with special rules as part of an argument list.

As a regular expression, the node's `expression` is evaluated and must
result in a list. If the list is empty, then the result of evaluation
of this node is void. If the list has a single element, then the result
of evaluation is that single element value. All other evaluations are
invalid (terminating the runtime).

As part of function call argument evaluation, each `interpolate` node is
evaluated, and the elements of the resulting list become individual actual
arguments to the call, rather than the list itself being a single argument.

It is an error (terminating the runtime) either if `expression` evaluates to
something other than a list, or it evaluates to a list of size greater than
one when being used as anything but a function call argument.

#### `literal` &mdash; `@[literal: value]`

* `value` (required) &mdash; Arbitrary data value.

This represents arbitrary literal data.

The data `value` is the result of evaluation,

When a `literal` node is run, the result of evaluation is `value`.
Evaluation never fails.

#### `varRef` &mdash; `@[varRef: name]`

* `name` (required) &mdash; Name of a variable (typically a string).

This represents a by-name variable reference.

When run, this causes the `name` to be looked up in the current
execution context. If a binding is found for it, then the bound value
is the result of evaluation. If a binding is not found for it, then
evaluation fails (terminating the runtime).

<br><br>
### Other Nodes and Values

These are nodes and values that appear within the data payloads
of various expression nodes.

#### `fnDef` &mdash; `@[fnDef: [name: name, (formals: [formal+])?, (yieldDef: name)?,` `statements: [statement*], (yield: expression)?]`

* `name: name` (required) &mdash; The name of the function.

* `formals: [formal+]` (optional) &mdash; Same meaning as for
  `closure` nodes (see which).

* `yieldDef: name` (optional) &mdash; Same meaning as for
  `closure` nodes (see which).

* `statements: [statement*]` (required) &mdash; Same meaning as for
  `closure` nodes (see which).

* `yield: expression` (optional) &mdash; Same meaning as for
  `closure` nodes (see which).

This represents a statement-level function definition. Nodes of this
type are valid within the `statements` list of a `closure` or `fnDef` node.

This is similar to assigning the variable `name` to the result of
evaluating `fn ...` as an expression, with one twist: In a statement list,
multiple `fnDef`s in a row can mutually self-reference.

**Note:** This node type is not generated by either the *Samizdat Layer 0* or
*Samizdat Layer 1* grammar. However, it *is* recognized by `sam0Eval()`.

#### `formal` &mdash; `[(name: name)?, (repeat: repeat)?]`

* `name: name` (optional) &mdash; an arbitrary value (but typically a string),
  which indicates the name of the variable to be bound for this
  formal. If omitted, then this indicates an unused argument which is
  not bound to a variable in the context of the closure body.

* `repeat: repeat` (optional) &mdash; indicates (if present) that the number
  of actual arguments bound by this formal is not necessarily exactly one.
  If present it must be one of:

  * `"?"` &mdash; indicates that this formal binds a single argument if
    available, including none. As such, this only really makes sense if
    only ever followed by other `?` formals and possibly a final `*` formal,
    though the syntax will tolerate it being in any position. The bound
    argument variable becomes a list, either of size one if an argument
    was bound or of size zero if not.

  * `"*"` &mdash; indicates that this formal binds as many actual
    arguments as are available, including none. As such, this only really
    makes sense as the `repeat` of the last formal, though the surface syntax
    will tolerate it being in any position. The bound argument variable
    becomes a list of all the passed actual arguments that were bound.

  * `"+"` &mdash; indicates that this formal binds as many actual
    arguments as are available, and it must bind at least one. Other than
    the minumum of one, this is identical to the `"*"` modifier.

If no `"repeat"` is specified, then the given formal binds exactly one
actual argument. The argument variable as bound is the same as the
actual argument as passed (no extra wrapping).

#### `varDef` &mdash; `@[varDef: [name: name, value: expression]]`

* `name: name` &mdash; Variable name to define (typically a string).

* `value: expression` &mdash; Expression node representing the value
  that the variable should take on when defined.

This represents a variable definition statement as part of a closure body.
Nodes of this type are valid within the `statements` list of a `closure`
or `fnDef` node.

When run, the `value` expression is evaluated. If it evaluates to void,
then evaluation fails (terminating the runtime). Otherwise, the evaluated
value is bound in the current (topmost) execution context to the
indicated `name`.
