Samizdat Layer 0: Core Library
==============================

core.LangNode
--------------

These are a set of accessor and constructor functions for the various
tree node types specified by the system.

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

#### `canYieldVoid(node) -> node | void`

Indicates whether `node` might possibly yield void when evalutated.
Returns `node` to indicate logic-true.

#### `formalsMaxArgs([formal*]) -> int`

Gets the maximum number of arguments that a given list of `formal`
arguments could possibly accept. If there is no limit, this returns `-1`.

#### `formalsMinArgs([formal*]) -> int`

Gets the minimum number of arguments that a given list of `formal`
arguments requires.

#### `get_baseName(taggedName) -> string`

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

#### `get_definedNames(node) -> [name*]`

Gets a list of the names of all variables defined by the given `node`.
If `node` defines no names, this returns `[]` (the empty list).

It is a fatal error to call this on an *unresolved* wildcard
`importModuleSelection` node.

#### `get_formals(node) -> [formal*]`

Gets the formal arguments of a `closure` node.

#### `get_format(node) -> string`

Gets the format of an `importResource` node.

#### `get_function(node) -> node`

Gets the function of an `apply`, or `call` node.

#### `get_id(node) -> int`

Gets the identifier of a reference node.

This function is defined here as a convenience for "reference" node types
used as intermediates during compilation. No layer 0 types use this.

#### `get_info(node) -> map | void`

Gets the metainformation map for the given node (presumably a closure),
if any. This is only defined on closure nodes which have been processed by
`withModuleDefs()` or equivalent.

#### `get_interpolate(node) -> node | void`

Gets the interpolated node, if any, of a node. This is non-void when a node
was created by virtue of a call to `makeInterpolate` (or equivalent)
and is in turn used by `makeCallOrApply` to detect when to translate
a call into an interpolated form.

**Note**: `interpolate` bindings aren't used during execution.

#### `get_lvalue(node) -> function | void`

Gets the `lvalue` binding for a node, if any. This is non-void for
`varFetch` nodes created with `makeVarFetchLvalue` or for any node in general
to which an `lvalue` has been attached.

The expected use of an `lvalue` is that it is a function (a real one, not
a tree of one) of one node argument, which can be called to produce a tree
representing an assignment of the lvalue-bearing node to the given node.

**Note**: `lvalue` bindings aren't used during execution.

#### `get_maxArgs(node) -> int`

Convenient shorthand for `formalsMaxArgs(get_formals(node))`.

#### `get_minArgs(node) -> int`

Convenient shorthand for `formalsMinArgs(get_formals(node))`.

#### `get_name(node) -> . | void`

Gets the name defined or used by the given node. This is applicable to
nodes of type `closure`, `importModule`, `importResource`, `varDef`,
`varDefMutable`, `varFetch`, `varRef`, and `varStore`.

#### `get_nodeValue(node) -> . | void`

Gets the value (literal or node) used by the given node, if any. This is
applicable to nodes of type `literal`, `maybe`, `noYield`, `string`
(pex type), `thunk` (pex type), `token` (pex type), `varDef`, `varDefMutable`,
and `varStore`.

#### `get_pex(node) -> pex`

Gets the inner parser node of the given node. This is applicable to
nodes of type `lookaheadFailure`, `lookaheadSuccess`, `opt`, `plus`, `star`,
and `parser`.

#### `get_pexes(node) -> [pex*]`

Gets the inner parser node list of the given node. This is applicable to
nodes of type `choice` and `sequence`.

#### `get_prefix(node) -> string`

Gets the name of an `importModuleSelection` node.

#### `get_select(node) -> [name*] | void`

Gets the binding selection of an `importModuleSelection` or `exportSelection`
node.

#### `get_source(node) -> source`

Gets the source of an import. This is applicable to nodes of type
`importModule`, `importModuleSelection`, and `importResource`.

#### `get_statements(node) -> [node*]`

Gets the statement list of a `closure` node.

#### `get_target(node) -> node`

Gets the target of a `fetch` or `store` node.

#### `get_values(node) -> node | [node*]`

Gets the values of a node. This is a list of nodes for `call`, `tokenSet`
(pex type), and `tokenSetComplement` (pex type) nodes. It is a simple node
for `apply` nodes.

#### `get_yieldNode(node) -> node | void`

Gets the yield of a `closure` node, if any. Full closures are required to
have a yield node, but basic closures are not.

**Note:** This is named `yieldNode` and not just `yield` to avoid conflict
with the token of the latter name.

#### `get_yieldDef(node) -> . | void`

Gets the yield definition name of a `closure` node, if any.

#### `intFromDigits(base, digits) -> int`

Converts a list of digit character tokens into an int, given the base.
`base` is allowed to be any value in the range `(2..16)`. A digit character
of `"_"` is allowed but otherwise ignored.

#### `isExpression(node) -> node | void`

Indicates whether `node` is a full expression node type (as opposed to,
notably, a restricted expression node type or a statement node type).
Returns `node` to indicate logic-true.

#### `makeApply(function, optValues?) -> node`

Makes an `apply` node, with the given `function` (an expression node)
being applied to the given `values` (an expression node). If `optValues`
is not passed, it defaults to `@void`.

#### `makeAssignmentIfPossible(target, value) -> node | .`

Makes an assignment node of some form, if possible. Given a `lvalue`-bearing
`target`, this calls `lvalue(value)` to produce a result. Otherwise, this
returns void.

#### `makeBasicClosure(map) -> node`

Makes a `closure` node, using the bindings of `map` as a basis, and adding
in sensible defaults for `formals` and `statements` if missing:

* `formals: []` &mdash; An empty formals list.
* `statements: []` &mdash; An empty statements list.

No default is provided for `yield`, as it is not always possible to
figure out a default for it at the points where `closure` nodes need to
be produced de novo. See `makeFullClosure()` for more detail.

#### `makeCall(function, values*) -> node`

Makes a `call` node, where `function` (an expression node) is called
with each of the `values` (each an expression node) as arguments, in
order.

#### `makeCallLiterals(function, values*) -> node`

Like `makeCall`, except that each of the `values` is made to be a literal
value.

#### `makeCallOrApply(function, values*) -> node`

Returns a function call node, where `function` (an expression node) is called
with each of the `values` (each an expression node) as arguments, in
order.

If any of the `values` is an `interpolate` node, this converts the
call into a form where the interpolated nodes have their usual
surface-language effect. The end result is an `apply` node.

If there are no arguments at all, the end result is a straightforward
`apply` node with `@void` for the arguments.

If there are no `interpolate` nodes in `values`, the end result is a
straightforward `call` node, the same as calling `makeCall` on the same
arguments.

#### `makeCallThunks(function, values*) -> node`

Like `makeCall`, except that each of the `values` is wrapped in
a thunk. This is useful in converting conditional expressions and the like.

#### `makeDynamicImport(node) -> [node+]`

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

#### `makeExport(node) -> node`

Makes an `export` node, indicating that the given `node`'s definitions
are to be exported. `node` must be valid to export, e.g. (but not limited
to) a `varDef` node.

#### `makeExportSelection(names+) -> node`

Makes an `exportSelection` node to export the variables with the given
`names`.

#### `makeFullClosure(nodeOrMap) -> node`

Makes a `closure` node, using the bindings of `nodeOrMap` as a basis, adding
in defaults like `makeBasicClosure()` (see which), and also performing
expansion and defaulting for the `yield` binding.

If `map` binds `yield`, then that binding is reflected in the result. If
the binding is to a `nonlocalExit` node, then that node is expanded
into an appropriate function call. As a special case, if it binds a
`nonlocalExit` which would call the `yieldDef` defined in `nodeOrMap`, then
the function call is elided.

If `nodeOrMap` does *not* bind `yield`, then in the result, `yield` is bound
to `@void` unless all of the following are true of `nodeOrMap`:

* The map does *not* include a binding for `yieldDef`. That is, it does not
  have a named or implicit nonlocal exit.
* It has a binding for `statements`, with length of at least 1.
* The final element of `statements` is a non-statement expression node.

If all of those are true, then in the result, the final node of
`statements` is removed and becomes the basis for the `yield` in the
result. If the node cannot possibly yield void, then it is directly used as
the `yield`. If it might yield void, then it is wrapped in a `@maybe`, and
the `@maybe` is used as the binding for `yield` in the result.

#### `makeGet(collArg, keyArg) -> node`

Makes a collection access (`get`) expression. This is a `call` node
of two arguments (a collection node and a key node).

#### `makeImport(baseData) -> node`

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

#### `makeInfoMap(node) -> {exports: {...}, imports: {...}, resources: {...}}`

Constructs the metainformation from a `closure` node that represents a
top-level module. This returns a map that binds `exports`, `imports`, and
`resources`.

`node` must be resolved, e.g. the result of having called
`withResolvedImports()`. It is a fatal error to call this on a node with
any unresolved wildcard imports.

**Note:** If `node` already has an `info` binding, then this function
just returns that.

#### `makeInterpolate(expr) -> node`

Makes an interpolation of the given expression node. The result is a
`fetch` node that refers to the given `expr` as both the main `value` and
as an `interpolate` binding. See `makeCallOrApply` for more details.

#### `makeLiteral(value) -> node`

Makes a `literal` node.

#### `makeMaybe(value) -> node`

Makes a raw `maybe` node. These are only valid to use in limited contexts.
See the expression node specification for details.

**Note:** This is different than `makeMaybeValue` in that
the latter produces an expression node which always evaluates to a list.

#### `makeMaybeValue(node) -> node`

Makes a maybe-value expression for the given `node`. This effectively
returns a node representing `node?` (for the original `node`), or
equivalently and more expanded, `maybeValue { -> node }`.

#### `makeNoYield(value) -> node`

Makes a `noYield` node.

#### `makeNonlocalExit(function, optValue?) -> node`

Makes a node representing a nonlocal exit, for calling the given `function`
with optional expression argument `optValue*`. `optValue*` is allowed
to be a `maybe` or `void` node. If `optValue` is not passed, it is
treated as if it were specified as `@void`.

This produces a `nonlocalExit` node per se, which must eventually be
processed via `makeFullClosure()` or similar.

#### `makeThunk(node) -> node`

Makes a thunk (no-argument function) that evaluates the given node, allowing
it to evaluate to void. That is, this returns `{ -> node }`. If given
any regular expression node, the result has a `maybe` node for the `yield`.
For the special `yield` node types (`maybe`, `void`, or `nonlocalExit`), the
result contains the given `node` as the `yield`, directly.

#### `makeVarDef(name, optValue?) -> node`

Makes a `varDef` statement node.

#### `makeVarDefMutable(name, optValue?) -> node`

Makes a `varDefMutable` statement node.

#### `makeVarFetch(name) -> node`

Makes a `fetch` node with a `varBox` payload, and no additional bindings.

#### `makeVarFetchLvalue(name) -> node`

Makes a `fetch` node with a `varBox` payload. The resulting `fetch` node
has an `lvalue` binding to a one-argument function which takes a node and
calls `makeVarStore()` to produce an assignment node.

#### `makeVarRef(name) -> node`

Makes a `varRef` node.

#### `makeVarStore(name, value) -> node`

Makes a `store` node with a `varBox` node for the target and the indicated
`value` binding.

#### `resolveImport(node, resolveFn) -> node`

Returns a node just like the given one (which must be an `import*` node),
except that it is resolved, using `resolveFn` to resolve any references.
Resolution means validating it and replacing wildcard selections with explicit
names.

This only actually causes `importModuleSelection` nodes to be altered.
Everything else just passes through as-is, if valid.

It is a fatal error (terminating the runtime) if `node` is found to be
invalid.

#### `withDynamicImports(node) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` nodes in the `statements` converted to their dynamic
forms.

#### `withFormals(node, [formal*]) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with `formals` (re)bound as given.

#### `withModuleDefs(node) -> node`

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

#### `withName(node, name) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with the `name` binding as given. If the original `node` already had a
`name` binding, this replaces it in the result.

#### `withResolvedImports(node, resolveFn) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` or `export(import*)`nodes in the `statements` list
validated and transformed, by calling `resolveImport(node, resolveFn)`.

#### `withTop(node) -> node`

Makes a node just like the given one (presumably a `varDef` node), except
with the addition of a `top: true` binding.

#### `withYieldDef(node, name) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with the addition of a yield definition binding for the given `name`.

If the given `node` already has a yield definition, then this does not
replace it. Instead, this adds an initial variable definition statement
to the `statements` in the result, which binds the given name to the original
`yieldDef` name.

#### `withYieldDefIfAbsent(node, name) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with the addition of a yield definition binding for the given `name`, but
only if `node` does not already have a yield definition. If `node` *does*
have a yield definition, then this just returns `node`.

This function is useful for propagating an outer yield definition into an
inner closure, especially with regards to providing the expected behavior
around implicit yielding of the final statement of a closure.

#### `withoutIntermediates(node) -> node`

Makes a node just like the given one, except without any "intermediate"
data payload bindings. These are bindings which are incidentally used
during typical tree node construction but which are not used for execution.
This includes `lvalue` and `interpolate`.

#### `withoutInterpolate(node) -> node`

Makes a node just like the given one, except without any binding
for `interpolate`. This is used by parser code to preventing argument
interpolation from applying to parenthesized expressions.

#### `withoutTops(node) -> node`

Makes a node just like the given one (presumably a `closure` node), except
with no `top` decalarations in the `statements` list.

More specifically, for each variable defined to be `top`, a forward-declaring
`varDef` is added at the top of the `statements` list. The original `varDef`
is replaced with an equivalent `varStore`. If any so-transformed variables
were `export`ed, then an `exportSelection` node is added to the end of the
`statements` list referencing all such variables.
