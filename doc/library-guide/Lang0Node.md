Samizdat Layer 0: Core Library
==============================

core::Lang0Node
---------------

These are a set of accessor and constructor functions for the various execution
tree node types.

<br><br>
### Constnants

#### `REFS`

This is a map from variable and module-scoped names to execution nodes that
refer to those variables and names, for all the references needed when
parsing the language.

Regular variable references are bound from their string name. For example,
`"cat"` maps to a regular variable reference of the name `cat`.

Module-scoped names are bound from a string of the form `Module_name`. For
example, `"Number_add"` maps to a reference to `Number::add`.

For specific details on which names are mapped, refer to the source. (As of
this writing, the set of things mapped in `REFS` is still undergoing
development.)


<br><br>
### Function Definitions

#### `get_actuals(node) <> [node*]`

Gets the actual arguments of a `call` or `apply` node.

#### `get_formals(node) <> [formal*]`

Gets the formal arguments of a `closure` node.

#### `get_function(node) <> node`

Gets the function a `call` or `apply` node.

#### `get_id(node) <> .`

Gets the identifier of reference nodes.

This function is defined here as a convenience for node types used as
intermediates during compilation. No layer 0 types use this.

#### `get_maxArgs(node) <> int`

Gets the maximum number of arguments that a given `closure` node
could possibly accept. If there is no limit, this returns `-1`.

#### `get_minArgs(node) <> int`

Gets the minimum number of arguments that a given `closure` node
requires.

#### `get_name(node) <> . | void`

Gets the name defined or used by the given node. This is applicable to
nodes of type `closure`, `varBind`, `varDef`, `varDefMutable`, and `varRef`.

#### `get_statements(node) <> [node*]`

Gets the statement list of a `closure` node.

#### `get_value(node) <> .`

Gets the value (literal or node) used by the given node. This is applicable to
nodes of type `expression`, `interpolate`, `literal`, `parser`, `varBind`,
`varDef`, and `varDefMutable`.

#### `get_yield(node) <> node | void`

Gets the yield of a `closure` node, if any.

#### `get_yieldDef(node) <> . | void`

Gets the yield definition name of a `closure` node, if any.

#### `makeApply(function, actuals) <> node`

Makes an `apply` node, with the given `function` (an expression node)
being applied to the given `actuals` (an expression node).

#### `makeCall(function, actuals*) <> node`

Makes a `call` node, where `function` (an expression node) is called
with each of the `actuals` (each an expression node) as arguments, in
order.

#### `makeCallLiterals(function, actuals*) <> node`

Like `makeCall`, except that each of the `actuals` is made to be a literal
value.

#### `makeCallNonlocalExit(nleRef, optExpr?) <> node`

Returns a function call node to a nonlocal exit reference, with optional
expression value. If passed, the expression is allowed to evaluate to void,
in which case the nonlocal exit yields void at its exit point.

#### `makeCallOrApply(function, actuals*) <> node`

Returns a function call node, where `function` (an expression node) is called
with each of the `actuals` (each an expression node) as arguments, in
order.

If any of the `actuals` is an `interpolate` node, this converts the
call into a form where the interpolated nodes have their usual
surface-language effect. The end result is an `apply` node.

If there are no `interpolate` nodes in `actuals`, the end result is a
straightforward `call` node, the same as calling `makeCall` on the same
arguments.

#### `makeCallThunks(function, actuals*) <> node`

Like `makeCall`, except that each of the `actuals` is wrapped in
a thunk. This is useful in converting conditional expressions and the like.

#### `makeGetExpression(collArg, keyArg) <> node`

Makes a collection access (`get`) expression. This is a `call` node
of two arguments (a collection node and a key node).

#### `makeInterpolate(expr) <> node`

Makes an `interpolate` node.

#### `makeLiteral(value) <> node`

Makes a `literal` node.

#### `makeOptValueExpression(node) <> node`

Makes an optional-value expression for the given `node`. This effectively
returns `node?`, or equivalently and more expanded, `optValue { <> node }`.

#### `makeThunk(node) <> node`

Makes a thunk (no-argument function) that evaluates the given node. That is,
this returns `{ <> node }`.

#### `makeVarBind(name, value) <> node`

Makes a `varBind` node.

#### `makeVarDef(name, optValue?) <> node`

Makes a `varDef` statement node.

#### `makeVarDefMutable(name, optValue?) <> node`

Makes a `varDefMutable` statement node.

#### `makeVarRef(name) <> node`

Makes a `varRef` node. The result is a direct `varRef` node, with no
additional bindings.

#### `makeVarRefLvalue(name) <> node`

Makes a `varRef` node, with an `lvalue` binding. In the result, `lvalue`
is bound to a one-argument function which takes a node and produces a
`varBind` node representing an assignment of the variable.

#### `withoutLvalue(node) <> node`

Makes a node just like the given one, except without any data payload
binding for `lvalue`.
