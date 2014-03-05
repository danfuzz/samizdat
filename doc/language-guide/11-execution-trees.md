Samizdat Language Guide
=======================

Execution Trees
---------------

Samizdat source code is itself parsed into a well-specified parse tree form
before being executed (or being prepared for execution). This format,
along with API defined by the primitively-defined part of the core library,
are the analog to a bytecode specification in systems that are
bytecode-based.

This section details all of the data structures of this form.

This section does *not* cover the forms used when building parsers. Parser
parse trees are covered in a later section. To be clear, those forms are
converted into the forms described in this section as an intermediate
step of parser construction.

<br><br>
### Expression Nodes

Each of these node types can appear anywhere an "expression"
is called for.

#### `apply` &mdash; `@apply{function: expression, actuals: expression}`

* `function: expression` (required) &mdash; An expression node that must
  evaluate to a function.

* `actuals: expression` (required) &mdash; An expression node that must
  evaluate to a list.

This represents a function call.

When run, first `function` and then `actuals` are evaluated. If `function`
evaluates to something other than a function, the call fails (terminating
the runtime). If `actuals` evaluates to anything but a list, the call fails
(terminating the runtime).

If there are too few actual arguments for the function (e.g., the
function requires at least three arguments, but only two are passed),
then the call fails (terminating the runtime).

With all the above prerequisites passed, the function is applied to
the evaluated actuals as its arguments, and the result of evaluation
is the same as whatever was returned by the function call (including
void).

**Note:** The difference between this and `call` is that the latter
takes its `actuals` as a list in the node itself.

#### `call` &mdash; `@call{function: expression, actuals: [expression*], (interpolate: expression)?}`

* `function: expression` (required) &mdash; An expression node that must
  evaluate to a function.

* `actuals: [expression*]` (required) &mdash; A list of arbitrary
  expression nodes, each of which must evaluate to a non-void value.

* `interpolate: expression` (optional) &mdash; Expression to use when treating
  this as a function call argument interpolation. (See below.)

This represents a function call.

When run, first `function` and then the elements of `actuals` (in
order) are evaluated. If `function` evaluates to something other than
a function, the call fails (terminating the runtime). If any of the
`actuals` evaluates to void, the call fails (terminating the runtime).

After that, this proceeds in the same manner as `apply`, using the
list of evaluated `actuals` as the arguments to the call.

The `interpolate` binding is *not* used during execution, rather it is only
ever used when programatically constructing trees. For example, it is used
by the function `Lang0Node::makeCallOrApply` to know that a "call
to the function `interpolate`" should actually be treated like an in-line
argument interpolation. Relatedly, `call` nodes with `interpolate` are
produced by the function `Lang0Node::makeInterpolate`.

#### `closure` &mdash; `@closure{formals: [formal+], (name: name)?, (yieldDef: name)?,` `statements: [statement*], (yield: expression)?}`

* `formals: [formal+]` (required) &mdash; An array of zero or more `formal`
  elements (as defined below). This defines the formal arguments to
  the function.

* `name: name` (optional) &mdash; The function name of the closure. Only
  used for producing debugging info (e.g. stack traces).

* `yieldDef: name` (optional) &mdash; A name (typically a string) to
  bind as the nonlocal-exit function.

* `statements: [statement*]` (required) &mdash; A list of statement
  nodes. A statement node must be either an expression node, or one of the
  various variable definition nodes (as defined below). This defines the bulk
  of the code to execute.

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
along the lines of `setjmp` / `longjmp`. In Lisp or Scheme terms, the
facility is an implementation of downward-passed / upward-called
continuations.

#### `expression` &mdash; `@expression{value: expression}`

* `value: expression` &mdash; Arbitrary expression node.

A node with type `expression` per se is merely a pass-through to its
enclosed expression. In terms of surface syntax, `expression` nodes
generally correspond to parenthesized expressions.

**Note:** Strictly speaking, there is no need for these nodes to exist.
However, they can be handy when generating execution trees.

#### `interpolate` &mdash; `@interpolate{value: expression}`

* `value: expression` &mdash; Expression node, which must yield a list when
  evaluated.

This represents the interpolation of a generator, which must either produce
no values or exactly one value when `collect`ed.

When run, the node's `expression` is evaluated and must result in a generator.
`collect` is called on the generator, producing a list. If the list is empty,
then the result of evaluation of this node is void. If the list has a single
element, then the result of evaluation is that single element value. All other
evaluations are invalid (terminating the runtime).

#### `jump` &mdash; `@jump{function: expression, (value: expression)?}`

* `function: expression` (required) &mdash; An expression node that must
  evaluate to a function.

* `value: expression` (optional) &mdash; An expression node, which is allowed
  to evaluate to void.

This represents a call to a function which is not expected to return.
When run, this behaves very nearly like a `call` node with the `value` as
the call `actuals`. The differences are:

* The `value` expression is allowed to evaluate to void, which in turn
  causes the evaluated `function` to be called with no arguments.
* It is a fatal error (terminating the runtime) if the called function returns.

This is used in the translation of `break`s, `return`s, `<named>` exits, and
other nonlocal exit calls.

#### `literal` &mdash; `@literal{value: value}`

* `value: value` (required) &mdash; Arbitrary data value.

This represents arbitrary literal data.

The data `value` is the result of evaluation,

When a `literal` node is run, the result of evaluation is `value`.
Evaluation never fails.

#### `varBind` &mdash; `@varBind{name: name, value: expression}`

* `name: name` &mdash; Variable name to bind (typically a string).

* `value: expression` &mdash; Expression node representing the
  value that the variable should take on when defined.

This represents a variable binding (assignment) statement as part of a
closure body.

When run, the `value` expression is evaluated. If it evaluates to void,
then evaluation fails (terminating the runtime). Otherwise, the `name`
is looked up and resolved to a variable reference. It is an error
(terminating the runtime) if no such variable is found, or if such a
variable is found and it is not available for binding (or rebinding).
Otherwise, the evaluated value is bound to the variable.

The result of evaluating this form is the same as the result of evaluating
`value`.

#### `varRef` &mdash; `@varRef{name: name}`

* `name: name` (required) &mdash; Name of a variable (typically a string).

This represents a by-name variable reference.

When run, this causes the `name` to be looked up in the current
execution context. If a binding is found for it, then the bound value
is the result of evaluation. If a binding is not found for it, then
evaluation fails (terminating the runtime).

<br><br>
### Other Nodes and Values

These are nodes and values that appear within the data payloads
of various expression nodes.

#### `formal` &mdash; `{(name: name)?, (repeat: repeat)?}`

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

#### `varDef` &mdash; `@varDef{name: name, value: expression}`

* `name: name` &mdash; Variable name to define (typically a string).

* `value: expression` (optional) &mdash; Expression node representing the
  value that the variable should take on when defined.

This represents an immutable variable definition statement as part of a
closure body. Nodes of this type are valid within the `statements` list of
a `closure` node.

When run successfully, nodes of this type cause `name` to be bound in the
current (topmost) execution context, to an immutable variable. That is,
once the variable is bound to a value, it can never be re-bound. The
behavior varies depending on if `value` is supplied in this node:

* Without a supplied `value`, this serves as a forward declaration. The
  variable is defined, but it is unbound (i.e. bound to void). It is then
  valid to bind the variable to a value *exactly once* by use of a
  `varBind` node. Before such binding, it is invalid (terminating the runtime)
  to refer to the variable.

* With `value` supplied, said `value` is evaluated. If it evaluates to void,
  then evaluation fails (terminating the runtime). Otherwise, the evaluated
  value becomes the permanently-bound value of the variable.

#### `varDefMutable` &mdash; `@varDef{name: name, (value: expression)?}`

* `name: name` &mdash; Variable name to define (typically a string).

* `value: expression` (optional) &mdash; Expression node representing the
  value that the variable should take on when defined.

This represents a mutable variable definition statement as part of a closure
body. Nodes of this type are valid within the `statements` list of a `closure`
node.

When run successfully, nodes of this type cause `name` to be bound in the
current (topmost) execution context, to a mutable variable. That is, the
variable may be bound and re-bound multiple times, by using `varBind` nodes.
The behavior varies depending on if `value` is supplied in this node:

* Without a supplied `value`, this serves as a forward declaration. The
  variable is defined, but it is unbound (i.e. bound to void). After
  definition and before binding (via `varBind`), it is invalid (terminating
  the runtime) to refer to the variable.

* With `value` supplied, said `value` is evaluated. If it evaluates to void,
  then evaluation fails (terminating the runtime). Otherwise, the evaluated
  value becomes the initially-bound value of the variable.
