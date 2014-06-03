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
by the function `$Lang0Node::makeCallOrApply` to know that a "call
to the function `interpolate`" should actually be treated like an in-line
argument interpolation. Relatedly, `call` nodes with `interpolate` are
produced by the function `$Lang0Node::makeInterpolate`.

#### `closure` &mdash; `formals: [formal+], (info: map)?, (name: name)?,` `statements: [statement*], (yield: expression)?, (yieldDef: name)?}`

* `formals: [formal+]` (required) &mdash; An array of zero or more `formal`
  elements (as defined below). This defines the formal arguments to
  the function.

* `info: map` (optional) &mdash; The metainformation of the closure. Only
  present in nodes which represent top-level module definitions that have
  been fully resolved.

* `name: name` (optional) &mdash; The function name of the closure. Only
  used for producing debugging info (e.g. stack traces).

* `statements: [statement*]` (required) &mdash; A list of statement
  nodes. A statement node must be either an expression node, or one of the
  various variable definition nodes (as defined below). This defines the bulk
  of the code to execute.

* `yield: expression` (optional) &mdash; An expression node representing
  the (local) result value for a call.

* `yieldDef: name` (optional) &mdash; A name (typically a string) to
  bind as the nonlocal-exit function.

This represents a closure (anonymous function) definition.

When run, a closure function (representation of the closure as an in-model
value) is created, which binds as variables the names of all
the formals to all the incoming actual arguments (as defined below),
binds the `yieldDef` name if specified to a nonlocal-exit function,
and binds all other variable names to whatever they already refer to in
the lexical evaluation environment. This closure function is the result of
evaluation.

If a nonlocal-exit function is defined, then that function accepts zero
or one argument. When called, it causes an immediate return from the active
function that it was bound to, yielding as a result whatever was passed to
it (including void). It is an error (terminating the runtime) to use a
nonlocal-exit function after the active function it was bound to has exit.

When the closure function is actually called (e.g. by virtue of being the
designated `function` in a `call` node), a fresh execution environment is
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

This is used in the translation of `break`s, `return`s, `/named` yields, and
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
execution environment. If a binding is found for it, then the bound value
is the result of evaluation. If a binding is not found for it, then
evaluation fails (terminating the runtime).

<br><br>
### Statement Nodes

These are nodes that are akin to expression nodes, but are limited to
be used only as direct as elements of the `statements` list of a `closure`
node.

#### `varDef` &mdash; `@varDef{name: name, value: expression, (top: true)?}`

* `name: name` &mdash; Variable name to define (typically a string).

* `value: expression` (optional) &mdash; Expression node representing the
  value that the variable should take on when defined.

* `top: true` (optional) &mdash; If present, indicates that this definition
  should be promoted to the top of the closure in which it appears.

This represents an immutable variable definition statement as part of a
closure body.

When run successfully, nodes of this type cause `name` to be bound in the
current (topmost) execution environment, to an immutable variable. That is,
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

The `top` binding, if present, has no effect at runtime. Instead, this is
expected to be used during definition simplification. See
`Lang0Node::withoutTops` for more details.

**Note:** Though there are no restrictions on the `name` in general, if
a node of this type represents a variable being exported from a module,
then it must be a string.

#### `varDefMutable` &mdash; `@varDef{name: name, (value: expression)?}`

* `name: name` &mdash; Variable name to define (typically a string).

* `value: expression` (optional) &mdash; Expression node representing the
  value that the variable should take on when defined.

This represents a mutable variable definition statement as part of a closure
body.

When run successfully, nodes of this type cause `name` to be bound in the
current (topmost) execution environment, to a mutable variable. That is, the
variable may be bound and re-bound multiple times, by using `varBind` nodes.
The behavior varies depending on if `value` is supplied in this node:

* Without a supplied `value`, this serves as a forward declaration. The
  variable is defined, but it is unbound (i.e. bound to void). After
  definition and before binding (via `varBind`), it is invalid (terminating
  the runtime) to refer to the variable.

* With `value` supplied, said `value` is evaluated. If it evaluates to void,
  then evaluation fails (terminating the runtime). Otherwise, the evaluated
  value becomes the initially-bound value of the variable.


<br><br>
### Program Nodes

These are nodes that are akin to expression or statement nodes, but are
limited to be used only as direct as elements of the `statements` list of a
`closure` node, and only for `closure` nodes which represet the outermost
layer of a program.

#### `export` &mdash; `@export{value: node}`

* `value: node` &mdash; Node representing definition to export.

This represents the export of one or more named bindings out of a module.
`node` must be of a statement type that is valid to be exported. This includes
types `@importModule`, `@importModuleSelection`, `@importResource`, and
`@varDef`.

These nodes are not directly executable. Instead, these are intended to be
used as part of a pre-execution or pre-compliation transformation, used to
produce a modified `closure` (with an altered `statements` list, and so on)
that incorporates the implied declaration(s). See `LangN::simplify` and
`Lang0Node::withModuleDefs` for more details.

#### `exportSelection` &mdash; `@exportSelection{select: [name+]}`

* `select: [name+]` &mdash; Selection of variable names to export, where
  each element must be a string.

This represents the export of a set of named bindings out of a module. Each
of the `names` must have been defined earlier in the list of statements in
which this node appears.

These nodes are not directly executable. Instead, these are intended to be
used as part of a pre-execution or pre-compliation transformation, used to
produce a modified `closure` (with an altered `statements` list, and so on)
that incorporates the implied declaration(s). See `LangN::simplify` and
`Lang0Node::withModuleDefs` for more details.

#### `importModule` &mdash; `@importModule{name: name, source: source}`

* `name: name` &mdash; Name of the variable to bind to. Must be a string.

* `source: source` &mdash; Name of the module. Must be either an `@external`
  or `@internal` value (described below).

This represents the import of a module, binding it to a named variable in
the program's top-level environment.

#### `importModuleSelection` &mdash; `@importModuleSelection{prefix: name, (select: [name+])?, source: source}`

* `prefix: name` &mdash; Prefix for variable names to bind. Must be a string.
  Allowed to be the empty string (`""`) to indicate no prefixing should be
  done.

* `select: [name+]` (optional) &mdash; List of module-exported bindings
  to import. Elements must be strings. When absent, indicates that *all* of
  the module's exports are to be imported.

* `source: source` &mdash; Name of the module. Must be either an `@external`
  or `@internal` value (described below).

This represents the import of some or all of a module's bindings as individual
variables in the program's top-level environment.

#### `importResource` &mdash; `@importResource{name: name, source: source, format: format}`

* `name: name` &mdash; Name of the variable to bind to. Must be a string.

* `source: source` &mdash; Name of the module. Must be either an `@external`
  or `@internal` value (described below).

* `format: format` &mdash; Format name which describes how to interpret the
  resource contents (typically a string).

This represents the import of a resource file, binding it to a named variable
in the program's top-level environment. The `format` indicates how the raw
data of the resource is to be interpreted.


<br><br>
### Other Values

These are values that appear within the data payloads of various nodes.

#### `external` &mdash; `@external(name)`

* `name` &mdash; String that represents an external module name. External
  names must take the form of a dot-delimited list of identifiers.

Used as the `source` binding for an `import*` node, this represents an
*external* module reference, which is always in the form of a fully-qualified
module name.

Examples:

```
@external("Blort")
@external("core.Blort")
@external("core.potions.Blort")
```

#### `formal` &mdash; `{(name: name)?, (repeat: repeat)?}`

* `name: name` (optional) &mdash; An arbitrary value (but typically a string),
  which indicates the name of the variable to be bound for this
  formal. If omitted, then this indicates an unused argument which is
  not bound to a variable in the environment of the closure body.

* `repeat: repeat` (optional) &mdash; Indicates (if present) that the number
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

#### `internal` &mdash; `@internal(name)`

* `name` &mdash; String that represents an internal module name. Internal
  names must take the form of a slash-delimited list of identifiers,
  optionally suffixed with a dot and a final identifier.

Used as the `source` binding for an `import*` node, this represents an
*internal* module reference, which is always in the form of a relative
path.

Examples:

```
@internal("blort")
@internal("blort.txt")
@internal("potion/blort")
@internal("potion/blort.txt")
@internal("frobozz/potion/blort.txt")
```
