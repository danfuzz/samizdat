Samizdat Layer 0: Core Library
==============================

core.Lang0Node
--------------

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
example, `"Number_add"` maps to a reference to `$Number::add`.

For specific details on which names are mapped, refer to the source. (As of
this writing, the set of things mapped in `REFS` is still undergoing
development.)


<br><br>
### Function Definitions

#### `formalsMaxArgs([formal*]) <> int`

Gets the maximum number of arguments that a given list of `formal`
arguments could possibly accept. If there is no limit, this returns `-1`.

#### `formalsMinArgs([formal*]) <> int`

Gets the minimum number of arguments that a given list of `formal`
arguments requires.

#### `get_actuals(node) <> [node*]`

Gets the actual arguments of an `apply` or `call` node.

#### `get_baseName(taggedName) <> string`

Gets the "base" name from a tagged name value. Operates on `@external`
module name and `@internal` relative path values. For `@external` names,
returns the final component name. For `@internal` paths, returns the
final path component, minus the extension (if any).

For example:

```
get_baseName(@external("Blort"))                    =>  "Blort"
get_baseName(@external("core.Fizmo"))               =>  "Fizmo"
get_baseName(@internal("frotz"))                    =>  "frotz"
get_baseName(@internal("frotz.txt"))                =>  "frotz"
get_baseName(@internal("frobozz/magic/frotz.txt"))  =>  "frotz"
```

#### `get_formals(node) <> [formal*]`

Gets the formal arguments of a `closure` node.

#### `get_format(node) <> string`

Gets the format of an `importResource` node.

#### `get_function(node) <> node`

Gets the function of an `apply`, `call`, or `jump` node.

#### `get_id(node) <> int`

Gets the identifier of a reference node.

This function is defined here as a convenience for "reference" node types
used as intermediates during compilation. No layer 0 types use this.

#### `get_lvalue(node) <> function | .`

Gets the `lvalue` binding for a node, if any. This is non-void for
`varRef` nodes created with `makeVarRefLvalue` or for any node in general
to which an `lvalue` has been attached.

The expected use of an `lvalue` is that it is a function (a real one, not
a tree of one) of one node argument, which can be called to produce a tree
representing an assignment of the lvalue-bearing node to the given node.

**Note**: `lvalue` bindings aren't used during execution.

#### `get_interpolate(node) <> node | .`

Gets the interpolated node, if any, of a `call` node. This is non-void
when a `call` node was created by virtue of a call to `makeInterpolate`
and is in turn used by `makeCallOrApply` to detect when to translate
a call into an interpolated form.

**Note**: `interpolate` bindings aren't used during execution.

#### `get_maxArgs(node) <> int`

Convenient shorthand for `formalsMaxArgs(get_formals(node))`.

#### `get_minArgs(node) <> int`

Convenient shorthand for `formalsMinArgs(get_formals(node))`.

#### `get_name(node) <> . | void`

Gets the name defined or used by the given node. This is applicable to
nodes of type `closure`, `importModule`, `importResource`, `varBind`,
`varDef`, `varDefMutable`, and `varRef`.

#### `get_nodeValue(node) <> .`

Gets the value (literal or node) used by the given node. This is applicable to
nodes of type `jump`, `literal`, `parser`, `varBind`, `varDef`, and
`varDefMutable`.

#### `get_prefix(node) <> string`

Gets the name of an `importModuleSelection` node.

#### `get_select(node) <> [name*]`

Gets the binding selection of an `importModuleSelection` or `exportSelection`
node.

#### `get_source(node) <> source`

Gets the source of an import. This is applicable to nodes of type
`importModule`, `importModuleSelection`, and `importResource`.

#### `get_statements(node) <> [node*]`

Gets the statement list of a `closure` node.

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

#### `makeDynamicImport(node) <> [node+]`

Converts an `import*` node to a list of statement nodes which perform an
equivalent set of actions, dynamically.

This can be used as part of a filter on the list of top-level statements of a
module, when they are to be executed in an environment that performs
dynamic (not static) importing.

**Note:** This returns a list of replacement nodes and not just a single
replacement node, because some `import*` forms must expand to multiple
statements. Always returning a list makes it possible to treat all return
values more uniformly.

#### `makeExport(node) <> node`

Makes an `export` node, indicating that the given `node`'s definitions
are to be exported. `node` must be valid to export, e.g. (but not limited
to) a `varDef` node.

#### `makeExportSelection(names+) <> node`

Makes an `exportSelection` node to export the variables with the given
`names`.

#### `makeGet(collArg, keyArg) <> node`

Makes a collection access (`get`) expression. This is a `call` node
of two arguments (a collection node and a key node).

#### `makeImport(baseData) <> node`

Makes an `@import*` node, based on `baseData`, which must be a map which
includes a consistent set of bindings for one of the `@import` node types.

See the tree grammar specification for most of the details on bindings.
Beyond that:

* To specify an `@importModuleSelection` node with a wildcard (import
  everything) import, use the binding `select: @"*"` instead of omitting
  `select`.

* If the `name` binding for a whole-module or resource import is omitted,
  then the name is automatically derived from the `source` binding.

* If the `prefix` binding for a selection import is omitted,
  then the prefix is automatically bound to `""` (the empty string).

This function rejects invalid combinations of bindings, terminating the
runtime with a message that indicates a plausible high-level reason for
the rejection. This makes it safe to "optimistically" parse a generalized
version of the `import` syntax, and use this function for a final
validation.

#### `makeInterpolate(expr) <> node`

Makes an interpolation of the given expression node. The result is a
`call` to the function `interpolate`, but with an additional `interpolate`
binding in the data payload. See `makeCallOrApply` for more details.

#### `makeJump(function, optValue?) <> node`

Makes a `jump` node, for calling the given `function` with optional
expression argument `value`.

#### `makeLiteral(value) <> node`

Makes a `literal` node.

#### `makeOptValue(node) <> node`

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

#### `resolveInfo(node) <> {exports: [name+], imports: [name+]}`

Resolves the metainformation a `closure` node that represents a top-level
module. This returns a map that binds `exports` and `imports`.

#### `resolveSelection(node) <> {(name: sourceName)+}`

Resolves the selection of a `importModuleSelection` node. This returns a
map from names to bind (the keys) to the corresponding source names to
import from (the values).

#### `withDynamicImports(node) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` nodes in the `statements` converted to their dynamic
forms.

#### `withFormals(node, [formal*]) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with `formals` (re)bound as given.

#### `withSimpleDefs(node) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with simpler definitions in the `statements` list. This includes the
following transformations:

* All `varDef` nodes with `top` bindings are "split" into an early forward
  declaration (`varDef` with no `value`) and a `varBind` in the original
  position.

* All `export` nodes are replaced with their `value` payloads.

* All `exportSelection` nodes are removed entirely.

* If there are any `export` or `exportSelection` nodes, a `yield` node
  is built up to contain all of the defined bindings.

**Note:** It is invalid for a `closure` with a `yield` to have any `export`
or `exportSelection` nodes its `statements`. Attempting to transform such
a node results in a fatal error.

#### `withTop(node) <> node`

Makes a node just like the given one (presumably a `varDef` node), except
with the addition of a `top: true` binding.

#### `withoutIntermediates(node) <> node`

Makes a node just like the given one, except without any "intermediate"
data payload bindings. These are bindings which are incidentally used
during typical tree node construction but which are not used for execution.
This includes `lvalue` and `interpolate`.

#### `withoutInterpolate(node) <> node`

Makes a node just like the given one, except without any binding
for `interpolate`. This is used by parser code to preventing argument
interpolation from applying to parenthesized expressions.

#### `withoutTops(node) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with no `top` decalarations in the `statements` list.

More specifically, for each variable defined to be `top`, a forward-declaring
`varDef` is added at the top of the `statements` list. The original `varDef`
is replaced with an equivalent `varBind`. If any so-transformed variables
were `export`ed, then an `exportSelection` node is added to the end of the
`statements` list referencing all such variables.
