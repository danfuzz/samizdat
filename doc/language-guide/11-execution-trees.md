Samizdat Language Guide
=======================

Execution Trees
---------------

Samizdat source code is itself parsed into a well-specified parse tree form
before being executed (or being prepared for execution). This format, along
with API defined by the primitively-defined part of the core library, are the
analog to a bytecode specification in systems that are bytecode-based.

This section details all of the data structures of this form.

This section does *not* cover the forms used when building parsers. Parser
execution trees are covered in a later section. To be clear, those forms are
converted into the forms described in this section as an intermediate step of
parser construction.


<br><br>
### Expression Nodes

Each of these node types can appear anywhere an "expression" is called for.

#### `apply` &mdash; `@apply{target: expression, name: expression, values: expression}`

* `target: expression` &mdash; An expression node.

* `name: expression` &mdash; An expression node that must evaluate to a symbol.

* `values: expression` &mdash; An expression or `maybe` node that must
  evaluate to a list, or a `void` node.

This represents a method call.

When run, `target`, `name`, and `values` are evaluated, in that order.
If `target` evaluates to void, or if `name` evaluates to something other than
a symbol, the call fails (terminating the runtime). If `values` evaluates to
anything but void or a list, the call fails (terminating the runtime).
`values` is allowed to evaluate to void *only* if it is a `maybe` or `void`
node; any other void evaluation is a fatal error.

If there are too few actual arguments for the function (e.g., the
function requires at least three arguments, but only two are passed),
then the call fails (terminating the runtime).

With all the above prerequisites passed, the method `name` is looked up
on `target`, and called with the evaluated arguments. This can fail for
any of the usual reasons that method calling can fail.
The result of evaluation is the same as whatever was returned by the
method call (including void).

**Note:** The main difference between this and `call` is that the latter
takes its `values` as a list in the node itself.

**Note:** Function calls can be represented by nodes of this type, with
`target` bound to the function and `name` bound to the literal symbol `@call`.

#### `call` &mdash; `@call{target: expression, name: expression, values: [expression*], interpolate?: expression}`

* `target: expression` &mdash; An expression node.

* `name: expression` &mdash; An expression node that must evaluate to a symbol.

* `values: [expression*]` &mdash; A list of arbitrary expression nodes, each
  of which must evaluate to a non-void value.

* `interpolate: expression` (optional) &mdash; Expression to use when treating
  this as a function call argument interpolation. (See below.)

This represents a method call.

When run, `target`, `name`, and the elements of `values` are evaluated, in
that order. If `target` evaluates to void, or if `name` evaluates to something
other than a symbol, the call fails (terminating the runtime). If any of the
`values` evaluates to void, the call fails (terminating the runtime).

After that, this proceeds in the same manner as `apply`, using the
list of evaluated `values` as the arguments to the call.

The `interpolate` binding is *not* used during execution, rather it is only
ever used when programatically constructing trees. For example, it is used
by the function `$LangNode::makeCallGeneral` to know that a "call
to the function `fetch`" should actually be treated as an in-line
argument interpolation. Relatedly, `call` nodes with `interpolate` are
produced by the function `$LangNode::makeInterpolate`.

**Note:** Function calls can be represented by nodes of this type, with
`target` bound to the function and `name` bound to the literal symbol `@call`.

#### `closure` &mdash; `@closure{info?: symbolTable, name?: symbol, yieldDef?: symbol, formals: [formal*], statements: [statement*], yield: expression}`

* `info: symbolTable` (optional) &mdash; The metainformation of the closure.
  Only present in nodes which represent top-level module definitions that
  have been fully resolved.

* `name: symbol` (optional) &mdash; The function name of the closure, as
  a symbol. Only used for producing debugging info (e.g. stack traces).

* `yieldDef: symbol` (optional) &mdash; A name to bind as the nonlocal-exit
  function.

* `formals: [formal*]` &mdash; A list of zero or more `formal` elements (as
  defined below). This defines the formal arguments to the closure.

* `statements: [statement*]` &mdash; A list of statement
  nodes. A statement node must be either an expression node, or one of the
  various variable definition nodes (as defined below). This defines the bulk
  of the code to execute.

* `yield: expression` &mdash; An expression node representing the (local)
  result value for a call. Allowed to be a `maybe` or `void` node. In
  intermediate forms, also allowed to be a `nonlocalExit` node.

This represents a closure (anonymous function) definition.

When evaluated, a closure function (representation of the closure as an
in-model value) is created, but no code in the closure is executed.

When a closure function is called, all of the following takes place (in this
order, or at least indistinguishable from this order):

* A fresh execution environment is constructed.

* If defined, the `yieldDef` name is bound as a variable to a nonlocal exit
  function.

  A nonlocal-exit function accepts zero or one argument. When called, it
  causes an immediate return from the active function that it was bound to,
  yielding as a result whatever was passed to it (including void). It is an
  error (terminating the runtime) to use a nonlocal-exit function after the
  active function it was bound to has exit.

* All of the `formals` (formal arguments) are bound as variables to the
  incoming actual arguments.

* Each of the `statements` is evaluated, in order. The results of evaluation
  are ignored.

* If defined, the `yield` expression is evaluated. The final returned result
  of the function call is the result of this `yield`. If `yield` is defined
  as a `maybe` or `void` node, then it is possible or mandatory (respectively)
  for the final result to be void.

* If `yield` was not defined, then the final returned result of the function
  call is void.

Any free variable references encountered during evaluation are taken to refer
to variables defined in the lexical environment of the closure. **Note:** The
lexical environment always includes a top-level "global" environment.

**Note:** As a possible clarification about nonlocal-exit functions: Defining
and using these amounts to something along the lines of `try` / `catch` in
systems that are defined using those terms. In C terms, the facility is along
the lines of `setjmp` / `longjmp`. In Lisp or Scheme terms, the facility is an
implementation of downward-passed / upward-called continuations.

#### `fetch` &mdash; `@fetch{target: expression, interpolate?: expression}`

* `target: expression` &mdash; Expression node. Must evaluate to a
  generator.

* `interpolate: expression` (optional) &mdash; Expression to use when treating
  this as a function call argument interpolation. (See below.)

This represents a call to the method `fetch`.

When run, `target` is evaluated. If it evaluates to void, then it is a fatal
error (terminating the runtime). Otherwise, `fetch` is called on `target`,
and the result of evaluation is the same as the result of evaluation of
that call, including possibly void.

The `interpolate` binding is *not* used during execution, rather it is only
ever used when programatically constructing trees. For example, it is used
by the function `$LangNode::makeCallGeneral` to know that a node of this
type should actually be treated as an in-line argument interpolation.
Relatedly, `fetch` nodes with `interpolate` bindings are produced by the
function `$LangNode::makeInterpolate`.

#### `literal` &mdash; `@literal{value: value}`

* `value: value` &mdash; Arbitrary data value.

This represents arbitrary literal data.

The data `value` is the result of evaluation,

When a `literal` node is run, the result of evaluation is `value`.
Evaluation never fails.

#### `noYield` &mdash; `@noYield{value: value}`

* `value: expression` &mdash; Expression node representing the
  result value.

This wraps an arbitrary other expression node, indicating that it must
never yield. That is, it must not produce a result, including a value
*or* void.

Nodes of this type are used to indicate positively that the flow of
control will never return directly. In particular, these are used to
wrap the calls that represent nonlocal exits.

When run, `value` is evaluated. Should control ever return directly
(to the `noYield` evaluator), it will result in a fatal error (terminating
the runtime).

#### `store` &mdash; `@store{target: expression, value: expression}`

* `target: expression` &mdash; Target to store to; must evaluate to something
  the implements the `Box` protocol.

* `value: expression` &mdash; Expression node representing the value that the
  target should take on. Allowed to be a `maybe` or `void` node.

This represents a box store (assignment).

When run, the `target` and `value` expressions are evaluated, in that order.
If `target` evaluates to void, or `value` is neither a `maybe` or `void` node
and evaluates to void, then evaluation fails (terminating the runtime).
Otherwise, the method `store` is called, with the `target` and
`value` results as arguments (in that order), or just with `target` if
`value` (legitimately) evaluated to void.

The result of evaluating this form is the same as the result returned from
the `store` call. This is typically the same as the result of evaluating
`value`. To be clear, nodes of this type *can* evaluate to void.

#### `varRef` &mdash; `@varRef{name: name}`

* `name: name` &mdash; Name of a variable (must be a symbol).

This is a reference to the *box* to which a variable is bound.

When run, this causes the `name` to be resolved in the current
execution environment. If a variable reference is found for it, then the
result of evaluation is the box to which the name is bound. If `name`
cannot be resolved, then evaluation fails (terminating the runtime).


<br><br>
### Limited Context Expression Nodes

These are expression nodes that have some additional restrictions on where
they can appear.

#### `maybe` &mdash; `@maybe{value: expression}`

* `value: expression` &mdash; Expression node representing the
  result value.

This wraps an arbitrary other expression node, indicating that it should
be allowed to evaluate to void.

Nodes of this type are only allowed to appear in the following contexts:

* As the `value` binding of an `apply` node.
* As the `value` binding of a `nonlocalExit` node.
* As the `value` binding of a `store` node.
* As the `value` binding of a `varDef` node.
* As the `yield` binding of a `closure` node, but only during intermediate
  processing.
* As the node passed as an argument to `$Code::eval`.

If a `maybe` node is *not* used in those contexts, then it is a fatal error
for the node in question to evaluate to void.

#### `nonlocalExit` &mdash; `@nonlocalExit{function: expression, value: expression}`

* `function: expression` &mdash; An expression node that must
  evaluate to a function.

* `value: expression` &mdash; An expression node representing
  the argument value for a call. Allowed to be a `maybe` or `void`
  node.

This is a representation of a nonlocal exit call.

Nodes of this type are not ever executed. Instead, they are allowed to
appear in intermediate trees as the `yield` binding of a `closure` node.
The function `LangNode::makeFullClosure()` converts these into other
forms. See the docs of that function for more details.

#### `void` &mdash; `@void{}`

This is an expression node which always evaluates to void. Nodes of this
type are only allowed to appear in the contexts where `maybe` nodes are valid.


<br><br>
### Statement Nodes

These are nodes that are akin to expression nodes, but are limited to
be used only as direct as elements of the `statements` list of a `closure`
node.

#### `varDef` &mdash; `@varDef{name: name, box: symbol, value: expression, top?: true}`

* `name: name` &mdash; Variable name to define (must be a symbol).

* `box: symbol` &mdash; Indicates what kind of box to produce. Must be one
  of `@cell`, `@lazy`, `@promise`, or `@result`.

* `value: expression` &mdash; Expression node representing the
  value that the variable should take on when defined. Allowed to be a `maybe`
  or `void` node, but see below for restrictions based on the `box` binding.

* `top: true` (optional) &mdash; If present, indicates that this definition
  should be promoted to the top of the closure in which it appears.

This represents a variable definition statement as part of a closure body.

When run successfully, nodes of this type cause `name` to be bound in the
current (innermost) execution environment. The `box` binding determines the
nature of the variable:

  * `@cell` &mdash; The variable is a mutable cell, which can be initially
    void or set, and which can be stored to multiple times.
  * `@lazy` &mdash; The variable is bound to a lazily-evaluated result. In
    this case `value` is expected to evaluate to a function (never void). That
    function is called only the first time the variable is fetched;
    subsequently that function's result is cached.
  * `@promise` &mdash; The variable is bound to a promise, which starts out
    holding void and which can be stored to exactly once. This can be used,
    for example, for forward declarations. In this case `value` is expected to
    always evaluate to void.
  * `@result` &mdash; The variable is bound upon construction and is
    thereafter immutable. It *can* be bound to void.

In cases where `value` is supplied, `value` is always evaluated at the time
of evaluation of this node.

The `top` binding, if present, has no effect at runtime. Instead, this is
expected to be used during definition simplification. See
`LangNode::withoutTops` for more details.


<br><br>
### Module / Program Nodes

These are nodes that are akin to expression or statement nodes, but are
limited to be used only as direct as elements of the `statements` list of a
`closure` node, and only for `closure` nodes which represet the outermost
layer of a module definition (including of a full program definition).

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
`LangNode::withModuleDefs` for more details.

#### `exportSelection` &mdash; `@exportSelection{select: [name+]}`

* `select: [name+]` &mdash; Selection of variable names to export, where
  each element must be a symbol.

This represents the export of a set of named bindings out of a module. Each
of the `names` must have been defined earlier in the list of statements in
which this node appears.

These nodes are not directly executable. Instead, these are intended to be
used as part of a pre-execution or pre-compliation transformation, used to
produce a modified `closure` (with an altered `statements` list, and so on)
that incorporates the implied declaration(s). See `LangN::simplify` and
`LangNode::withModuleDefs` for more details.

#### `importModule` &mdash; `@importModule{name: name, source: source}`

* `name: name` &mdash; Name of the variable to bind to. Must be a symbol.

* `source: source` &mdash; Name of the module. Must be either an `@external`
  or `@internal` value (described below).

This represents the import of a module, binding it to a named variable in
the program's top-level environment.

#### `importModuleSelection` &mdash; `@importModuleSelection{prefix?: name, select?: [name+], source: source}`

* `prefix: name` (optional) &mdash; Prefix for variable names to bind. Must
  be a symbol. Omitted to indicate no prefixing should be done.

* `select: [name+]` (optional) &mdash; List of module-exported bindings
  to import. Elements must be symbols. When absent, indicates that *all* of
  the module's exports are to be imported.

* `source: source` &mdash; Name of the module. Must be either an `@external`
  or `@internal` value (described below).

This represents the import of some or all of a module's bindings as individual
variables in the program's top-level environment.

#### `importResource` &mdash; `@importResource{name: name, source: source, format: format}`

* `name: name` &mdash; Name of the variable to bind to. Must be a symbol.

* `source: source` &mdash; Name of the module. Must be either an `@external`
  or `@internal` value (described below).

* `format: format` &mdash; Format name which describes how to interpret the
  resource contents (must be a symbol).

This represents the import of a resource file, binding it to a named variable
in the program's top-level environment. The `format` indicates how the raw
data of the resource is to be interpreted.


<br><br>
### Other Values

These are values that appear within the data payloads of various nodes.

#### `classMethod` &mdash; `@classMethod{name: symbol, ...}`

* `name: symbol` &mdash; The name of the method.

* Other bindings identical to `closure`.

This is a representation of a class method, as part of a list of methods
for a class definition. Nodes of this type are not ever executed. Rather,
they are used as elements of the method list passed to `makeClassDef`.

#### `external` &mdash; `@external{name: string}`

* `name` &mdash; String that represents an external module name. External
  names must take the form of a dot-delimited list of identifiers.

Used as the `source` binding for an `import*` node, this represents an
*external* module reference, which is always in the form of a fully-qualified
module name.

Examples:

```
@external{name: "Blort"}
@external{name: "core.Blort"}
@external{name: "core.potions.Blort"}
```

#### `formal` &mdash; `@formal{name?: symbol, repeat?: repeat}`

* `name: symbol` (optional) &mdash; An arbitrary symbol, which indicates the
  name of the variable to be bound for this formal. If omitted, then this
  indicates an unused argument which is not bound to a variable in the
  execution environment of the closure body.

* `repeat: repeat` (optional) &mdash; Indicates (if present) that the number
  of actual arguments bound by this formal is not necessarily exactly one.
  If present it must be one of:

  * `@"?"` &mdash; indicates that this formal binds a single argument if
    available, including none. As such, this only really makes sense if
    only ever followed by other `?` formals and possibly a final `*` formal,
    though the syntax will tolerate it being in any position. The bound
    argument variable becomes a list, either of size one if an argument
    was bound or of size zero if not.

  * `@"*"` &mdash; indicates that this formal binds as many actual
    arguments as are available, including none. As such, this only really
    makes sense as the `repeat` of the last formal, though the surface syntax
    will tolerate it being in any position. The bound argument variable
    becomes a list of all the passed actual arguments that were bound.

  * `@"+"` &mdash; indicates that this formal binds as many actual
    arguments as are available, and it must bind at least one. Other than
    the minumum of one, this is identical to the `"*"` modifier.

This is a representation of a formal argument in a `@closure` node.

If no `repeat` is specified, then the given formal binds exactly one
actual argument. The argument variable as bound is the same as the
actual argument as passed (no extra wrapping).

#### `instanceMethod` &mdash; `@instanceMethod{name: symbol, ...}`

* `name: symbol` &mdash; The name of the method.

* Other bindings identical to `closure`.

This is a representation of an instance method, as part of a list of methods
for a class definition. Nodes of this type are not ever executed. Rather,
they are used as elements of the method list passed to `makeClassDef`.

#### `internal` &mdash; `@internal{name: string}`

* `name` &mdash; String that represents an internal module name. Internal
  names must take the form of a slash-delimited list of identifiers,
  optionally suffixed with a dot and a final identifier.

Used as the `source` binding for an `import*` node, this represents an
*internal* module reference, which is always in the form of a relative
path.

Examples:

```
@internal{name: "blort"}
@internal{name: "blort.txt"}
@internal{name: "potion/blort"}
@internal{name: "potion/blort.txt"}
@internal{name: "frobozz/potion/blort.txt"}
```

#### `mapping` &mdash; `@mapping{keys: [key*], value}`

* `keys` &mdash; List of keys to be bound.
* `value` &mdash; Value to bind to.

Used as an argument to `makeMapExpression` or `makeSymbolTableExpression`
(never directly executed). This represents one mapping inside a map or
map-like constructor expression. Elements of `keys` are treated with respect
to interpolation just like arguments in a `call` node.

#### `methodId` &mdash; `@methodId{class?: expression, name: expression}`

* `class` (optional) &mdash; Node that must evaluate to a class.
* `name` &mdash; Node that must evaluate to a symbol.

Used as an argument to `makeFunCall` (never directly executed). This
represents a class or instance method, so that functions and methods can
be treated more uniformly in cases where it's useful.
