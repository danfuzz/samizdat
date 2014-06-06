Samizdat Layer 0: Core Library
==============================

core.Lang0Node
--------------

These are a set of accessor and constructor functions for the various execution
tree node types.

A few functions in this module take an argument named `resolveFn`. Such an
argument is expected to be a function which behaves similarly to
`core.ModuleSystem::resolve()`, as described in the sections on the module
system. This function, though, is allowed to *not* actually load any code,
as when used here it is only the resolved metainformation that gets used.


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

#### `get_actuals(node) <> node | [node*]`

Gets the actual arguments of an `apply` node (resulting in an expression node)
or `call` node (resulting in a list of expression nodes).

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

#### `get_definedNames(node) <> [name*]`

Gets a list of the names of all variables defined by the given `node`.
If `node` defines no names, this returns `[]` (the empty list).

It is a fatal error to call this on an *unresolved* wildcard
`importModuleSelection` node.

#### `get_formals(node) <> [formal*]`

Gets the formal arguments of a `closure` node.

#### `get_format(node) <> string`

Gets the format of an `importResource` node.

#### `get_function(node) <> node`

Gets the function of an `apply`, or `call` node.

#### `get_id(node) <> int`

Gets the identifier of a reference node.

This function is defined here as a convenience for "reference" node types
used as intermediates during compilation. No layer 0 types use this.

#### `get_info(node) <> map`

Gets the metainformation map for the given node (presumably a closure).
This is only defined on closure nodes which have been processed by
`withModuleDefs()` or equivalent.

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
nodes of type `literal`, `maybe`, `noYield`, `parser`, `varBind`,
`varDef`, and `varDefMutable`.

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

#### `get_yieldNode(node) <> node`

Gets the yield of a `closure` node.

**Note:** This is named `yieldNode` and not just `yield` to avoid conflict
with the token of the latter name.

#### `get_yieldDef(node) <> . | void`

Gets the yield definition name of a `closure` node, if any.

#### `makeApply(function, optActuals?) <> node`

Makes an `apply` node, with the given `function` (an expression node)
being applied to the given `actuals` (an expression node). If `optActuals`
is not passed, it defaults to `@void`.

#### `makeBasicClosure(map) <> node`

Makes a `closure` node, using the bindings of `map` as a basis, and adding
in sensible defaults for `formals` and `statements` if missing:

* `formals: []` &mdash; An empty formals list.
* `statements: []` &mdash; An empty statements list.

No default is provided for `yield`, as it is not always possible to
figure out a default for it at the points where `closure` nodes need to
be produced de novo. See `makeFullClosure()` for more detail.

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

If there are no arguments at all, the end result is a straightforward
`apply` node with `@void` for the arguments.

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

It is a fatal error to call this on an `importModuleSelection` with
a wildcard selection.

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

#### `makeFullClosure(map) <> node`

Makes a `closure` node, using the bindings of `map` as a basis, adding
in defaults like `makeBasicClosure()` (see which), as well as for `yield`.

In particular, if `yield` is not specified, the result includes a binding
of `yield` to `@void`.

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

#### `makeInfoMap(node) <> {exports: {...}, imports: {...}, resources: {...}}`

Constructs the metainformation from a `closure` node that represents a
top-level module. This returns a map that binds `exports`, `imports`, and
`resources`.

`node` must be resolved, e.g. the result of having called
`withResolvedImports()`. It is a fatal error to call this on a node with
any unresolved wildcard imports.

**Note:** If `node` already has an `info` binding, then this function
just returns that.

#### `makeInterpolate(expr) <> node`

Makes an interpolation of the given expression node. The result is a
`call` to the function `interpolate`, but with an additional `interpolate`
binding in the data payload. See `makeCallOrApply` for more details.

#### `makeLiteral(value) <> node`

Makes a `literal` node.

#### `makeMaybe(value) <> node`

Makes a raw `maybe` node. These are only valid to use in limited contexts.
See the expression node specification for details.

**Note:** This is different than `makeMaybeValue` in that
the latter produces an expression node which always evaluates to a list.

#### `makeMaybeValue(node) <> node`

Makes a maybe-value expression for the given `node`. This effectively
returns a node representing `node?` (for the original `node`), or
equivalently and more expanded, `maybeValue { <> node }`.

#### `makeNoYield(value) <> node`

Makes a `noYield` node.

#### `makeNonlocalExit(function, optValue?) <> node`

Makes a node representing a nonlocal exit, for calling the given `function`
with optional expression argument `optValue*`. If `optValue` is passed, then
it is allowed to evaluate to void. The resulting node is a `noYield`, since
nonlocal exits are never supposed to return to their direct callers.

#### `makeThunk(node) <> node`

Makes a thunk (no-argument function) that evaluates the given node, allowing
it to evaluate to void. That is, this returns `{ <> node }`.

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

#### `resolveImport(node, resolveFn) <> node`

Returns a node just like the given one (which must be an `import*` node),
except that it is resolved, using `resolveFn` to resolve any references.
Resolution means validating it and replacing wildcard selections with explicit
names.

This only actually causes `importModuleSelection` nodes to be altered.
Everything else just passes through as-is, if valid.

It is a fatal error (terminating the runtime) if `node` is found to be
invalid.

#### `withDynamicImports(node) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` nodes in the `statements` converted to their dynamic
forms.

#### `withFormals(node, [formal*]) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with `formals` (re)bound as given.

#### `withModuleDefs(node) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with `statements` and `yield` bindings processed to make the node
appropriate for use as a top-level module definition, and with an `info`
binding for the metainformation. This includes the following transformations:

* All `export` nodes are replaced with their `value` payloads.

* All `exportSelection` nodes are removed entirely.

* A `yield` is added, of a `@module` value with a map payload that binds
  `exports` and `info`.

  * If there are any `export` or `exportSelection` nodes, the `exports`
    binding is built up to contain all of the defined exported bindings.

  * If there are no `export` or `exportSelection` nodes, the `exports`
    binding is arranged to be `{}` (the empty map).

  * The `info` binding is set up to be the defined metainformation of the
    module. This value is the same as the resulting node's direct `info`
    binding.

It is invalid (terminating the runtime) to call this function
on a `closure` with a `yield` that is anything but `@void`.

#### `withResolvedImports(node, resolveFn) <> node`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` or `export(import*)`nodes in the `statements` list
validated and transformed, by calling `resolveImport(node, resolveFn)`.

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
