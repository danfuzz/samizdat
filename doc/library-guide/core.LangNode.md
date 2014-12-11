Samizdat Layer 0: Core Library
==============================

core.LangNode
--------------

These are a set of accessor and constructor functions for the various
tree node types specified by the system. This module also contains other
miscellaneous utilities to help parse the language.

A few functions in this module take an argument named `resolveFn`. Such an
argument is expected to be a function which behaves similarly to
`core.ModuleSystem::resolve()`, as described in the sections on the module
system. This function, though, is allowed to *not* actually load any code,
as when used here it is only the resolved metainformation that gets used.

**Note:** In naming types here, `ExpressionNode` refers to arbitrary
expression node types.


<br><br>
### Constnants

#### `METHODS`

This is a table from class and instance method names to `@methodId` nodes that
refer to those methods, for all the method references needed when parsing the
language.

Each class method key is a symbol of the form `@Class_name`. For example,
`@Cmp_eq` maps to a reference to the class method `Cmp.eq()`.

Each instance method key is just the name as a symbol. For example,
`@and` maps to a reference to the instance method `.and()`.

For specific details on which names are mapped, refer to the source.

#### `KEYWORDS`

This is a table from symbol names of keywords in the language to the result
of tokenizing those keywords. The result of tokenizing a keyword is always
an empty-payload token (record).

#### `LITS`

This is a table from mnemonic names to literal nodes representing the
implied values, for all such literals needed when parsing the language.
For example, `LITS::false` is a literal node referring to the
boolean value `false`.

For specific details on which names are mapped, refer to the source.

#### `REFS`

This is a table from variable and module-scoped names to execution nodes that
refer to those variables and names, for all the references needed when
parsing the language.

Regular variable references are bound from their symbol name. For example,
`@blort` if bound would map to a regular variable reference of the name
`blort`.

Module-scoped names are bound from a symbol of the form `@Module_name`. For
example, `@Potions_blort` if bound would map to a reference to
`$Potions::blort`.

For specific details on which names are mapped, refer to the source.

#### `SYMS`

This is a table from symbols to literal nodes that represent those
symbols, for all the literal symbols needed directly when parsing the
language.

For specific details on which names are mapped, refer to the source.


<br><br>
### Functions

#### `canYieldVoid(node) -> logic`

Indicates whether `node` might possibly yield void when evalutated.
Returns `node` to indicate logic-true.

#### `formalsMaxArgs([formal*]) -> isa Int`

Gets the maximum number of arguments that a given list of `formal`
arguments could possibly accept. If there is no limit, this returns `-1`.

#### `formalsMinArgs([formal*]) -> isa Int`

Gets the minimum number of arguments that a given list of `formal`
arguments requires.

#### `extractLiteral(node) -> . | void`

If `node` is a `literal`, this returns its literal value. If not, this
returns void.

#### `get_baseName(taggedName) -> isa String`

Gets the "base" name from a tagged name value. Operates on `@external`
module name and `@internal` relative path values. For `@external` names,
returns the final component name. For `@internal` paths, returns the
final path component, minus the extension (if any).

For example:

```
get_baseName(@external{name: "Blort"})                    =>  "Blort"
get_baseName(@external{name: "core.Fizmo"})               =>  "Fizmo"
get_baseName(@internal{name: "frotz"})                    =>  "frotz"
get_baseName(@internal{name: "frotz.txt"})                =>  "frotz"
get_baseName(@internal{name: "frobozz/magic/frotz.txt"})  =>  "frotz"
```

#### `get_definedNames(node) -> isa [Symbol*]`

Gets a list of the names of all variables defined by the given `node`.
If `node` defines no names, this returns `[]` (the empty list).

It is a fatal error to call this on an *unresolved* wildcard
`importModuleSelection` node.

#### `intFromDigits(base, digits) -> isa Int`

Converts a list of digit character tokens into an int, given the base.
`base` is allowed to be any value in the range `(2..16)`. A digit character
of `"_"` is allowed but otherwise ignored.

#### `isExpression(node) -> isa ExpressionNode | void`

Indicates whether `node` is a full expression node type (as opposed to,
notably, a restricted expression node type or a statement node type).
Returns `node` to indicate logic-true.

#### `makeApply(target, name, optValues?) -> isa ExpressionNode`

Makes an `apply` node, where the method `name` (an expression node) is
called on the given `target` (an expression node) with the given `optValues*`
as arguments. If `optValues` is not passed, it defaults to `@void`.

#### `makeAssignmentIfPossible(target, value) -> isa ExpressionNode | .`

Makes an assignment node of some form, if possible. Given a `lvalue`-bearing
`target`, this calls `lvalue(value)` to produce a result. Otherwise, this
returns void.

#### `makeBasicClosure(table) -> isa ExpressionNode`

Makes a `closure` node, using the bindings of `table` as a basis, and adding
in sensible defaults for `formals` and `statements` if missing:

* `formals: []` &mdash; An empty formals list.
* `statements: []` &mdash; An empty statements list.

No default is provided for `yield`, as it is not always possible to
figure out a default for it at the points where `closure` nodes need to
be produced de novo. See `makeFullClosure()` for more detail.

#### `makeCall(target, name, values*) -> isa ExpressionNode`

Makes a `call` node, where the method `name` (an expression node) is
called on the given `target` (an expression node) with the given `values`
(each an expression node) as arguments, in order.

#### `makeCallGeneral(target, name, values*) -> isa ExpressionNode`

Returns a method invocation node, where the method `name` (an expression node)
is called on the given `target` (an expression node) with the given `values`
(each an expression node) as arguments, in order.

If any of the `values` is an `interpolate` node, this converts the call into
a form where the interpolated nodes have their usual surface-language effect.
The end result in this case is an `apply` node.

If `values` is empty (no extra arguments passed), the end result is a
straightforward `apply` node with `@void` for the arguments.

Otherwise, if there are no `interpolate` nodes in `values`, the end result is
a straightforward `call` node, identical to having called `makeCall` with the
same arguments.

#### `makeCallLiterals(target, name, values*) -> isa ExpressionNode`

Like `makeCall`, except that each of the `values` is made to be a literal
value.

#### `makeClassDef(name, attributes, methods) -> isa ExpressionNode`

Makes a class definition node. This is a `top` variable definition of a class
with the given name. `attributes` must be a list of single-binding symbol
tables, and `methods` must be a list of an arbitrary mix of `classMethod`
and `instanceMethod` nodes.

#### `makeDynamicImport(node) -> isa [ExpressionNode+]`

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

#### `makeExport(node) -> isa ExpressionNode`

Makes an `export` node, indicating that the given `node`'s definitions
are to be exported. `node` must be valid to export, e.g. (but not limited
to) a `varDef` node.

#### `makeExportSelection(names+) -> isa ExpressionNode`

Makes an `exportSelection` node to export the variables with the given
`names`. Each of the `names` must be a symbol.

#### `makeFullClosure(base) -> isa ExpressionNode`

Makes a `closure` node, using the bindings of `base` as a basis, adding
in defaults like `makeBasicClosure()` (see which), and also performing
expansion and defaulting for the `yield` binding. `base` must either be
a `@closure` node (allowed to be incomplete) or a symbol table.

If `base` binds `yield`, then that binding is reflected in the result.
If the binding is to a `nonlocalExit` node, then that node is expanded
into an appropriate function call. As a special case, if it binds a
`nonlocalExit` which would call the `yieldDef` defined in `base`, then
the function call is elided.

If `base` does *not* bind `yield`, then in the result, `yield` is bound
to `@void` unless all of the following are true of `base`:

* The table does *not* include a binding for `yieldDef`. That is, it does not
  have a named or implicit nonlocal exit.
* It has a binding for `statements`, with length of at least 1.
* The final element of `statements` is a non-statement expression node.

If all of those are true, then in the result, the final node of
`statements` is removed and becomes the basis for the `yield` in the
result. If the node cannot possibly yield void, then it is directly used as
the `yield`. If it might yield void, then it is wrapped in a `@maybe`, and
the `@maybe` is used as the binding for `yield` in the result.

#### `makeFunCall(function, values*) -> isa ExpressionNode`

Makes a `call` node, where `function` (an expression node) is called
with each of the `values` (each an expression node) as arguments, in
order.

The result is a `call` node with `function` as the target and literal
`@call` as the name.

As a special case, if `function` is actually a `@methodId` node, then that
node is "deconstructed" into either a class or instance method call.

#### `makeFunCallGeneral(function, values*) -> isa ExpressionNode`

Like `makeCallGeneral`, except this takes a `function` instead of a
`target` and `name`.

The result is a `call` or `apply` node with `function` as the target and
literal `@call` as the name.

#### `makeFunCallThunks(function, values*) -> isa ExpressionNode`

Like `makeFunCall`, except that each of the `values` is wrapped in
a thunk. This is useful in converting conditional expressions and the like.

#### `makeGet(collArg, keyArg) -> isa ExpressionNode`

Makes a collection access (`get`) expression. This is a `call` node
of two arguments (a collection node and a key node).

#### `makeImport(baseData) -> isa ExpressionNode`

Makes an `@import*` node, based on `baseData`, which must be a table which
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

#### `makeInfoTable(node) -> isa SymbolTable`

Constructs the metainformation from a `closure` node that represents a
top-level module. This returns a symbol table that binds `exports`, `imports`,
and `resources`.

`node` must be resolved, e.g. the result of having called
`withResolvedImports()`. It is a fatal error to call this on a node with
any unresolved wildcard imports.

**Note:** If `node` already has an `info` binding, then this function
just returns that.

#### `makeInterpolate(expr) -> isa ExpressionNode`

Makes an interpolation of the given expression node. The result is a
`fetch` node that refers to the given `expr` as both the main `value` and
as an `interpolate` binding, and which binds `lvalue` to a store conversion
function. See `makeFunCallGeneral` for more details about `interpolate` bindings.

#### `makeLiteral(value) -> isa ExpressionNode`

Makes a `literal` node.

#### `makeMapExpression(mappings*) -> isa ExpressionNode`

Makes an expression node that represents the construction of a map
consisting of the given `mappings`. Arguments that are `mapping`
values are restructured into appropriate calls to `Map.new()` or
`Map.singleValue()`. Other arguments are taken to be interpolated arguments.
In trivial cases, the result is a simple `call` node for a call to
`Map.new()` or `Map.singleValue()`. In other cases, the result is a call to
`cat` with less trivial internal structure.

#### `makeMaybe(value) -> isa ExpressionNode`

Makes a raw `maybe` node. These are only valid to use in limited contexts.
See the expression node specification for details.

**Note:** This is different than `makeMaybeValue` in that
the latter produces an expression node which always evaluates to a list.

#### `makeMaybeValue(node) -> isa ExpressionNode`

Makes a maybe-value expression for the given `node`. This effectively
returns a node representing `node?` (for the original `node`).

In the usual case, the result is equivalent to a node for
`If.maybeValue { -> node }`. However, if `node` bound `box`, then this simply
returns the so-bound value. The latter is the case for general variable
references, where postfix `?` denotes a reference to the varaible's box.

#### `makeNoYield(value) -> isa ExpressionNode`

Makes a `noYield` node.

#### `makeNonlocalExit(function, optValue?) -> isa ExpressionNode`

Makes a node representing a nonlocal exit, for calling the given `function`
with optional expression argument `optValue*`. `optValue*` is allowed
to be a `maybe` or `void` node. If `optValue` is not passed, it is
treated as if it were specified as `@void{}`.

This produces a `nonlocalExit` node per se, which must eventually be
processed via `makeFullClosure()` or similar.

#### `makeRecordExpression(name, data) -> isa ExpressionNode`

Makes an expression node that represents a construction of a record with
the given `name` and `data`, both themselves expression nodes.

Most of the time, this results in a node representing a call to `Record.new`.
However, if both `name` and `data` are `literal` nodes, this function
returns a `literal` node representing the actual constructed record.

#### `makeSymbolTableExpression(mappings*) -> isa ExpressionNode`

Makes an expression node that represents the construction of a symbol table
consisting of the given `mappings`. Arguments that are `mapping`
values are restructured into appropriate calls to `SymbolTable.new()` or
`SymbolTable.singleValue()`. Other arguments are taken to be interpolated
arguments. In trivial cases, the result is a simple `call` node for a call to
`SymbolTable.new()` or `SymbolTable.singleValue()`. In other cases, the result
is a call to `cat` with less trivial internal structure.

#### `makeThunk(node) -> isa ExpressionNode`

Makes a thunk (no-argument function) that evaluates the given node, allowing
it to evaluate to void. That is, this returns `{ -> node }`. If given
any regular expression node, the result has a `maybe` node for the `yield`.
For the special `yield` node types (`maybe`, `void`, or `nonlocalExit`), the
result contains the given `node` as the `yield`, directly.

#### `makeVarDef(name, box, optValue?) -> isa ExpressionNode`

Makes a `varDef` statement node. `name` and `box` must both be symbols.
`optValue` if passed must be an expression node, including the option of using
a `maybe` or `void` node. If `optValue` is not passed, it is treated as if it
were specified as `@void{}`.

See the documentation on `varDef` nodes for information about the allowed
values for `box` and how it relates to `optValue`.

#### `makeVarFetch(name) -> isa ExpressionNode`

Makes a `fetch` node with a `varRef` payload, and no additional bindings.
`name` must be a symbol.

#### `makeVarFetchGeneral(name) -> isa ExpressionNode`

Makes a `fetch` node with a `varRef` payload. `name` must be a symbol.

The resulting `fetch` node is a general variable reference, which makes it
usable as-is (to fetch a variable's value), as well as usable as an lvalue
(to store into a variable) *and* to use with the maybe-value operator
(postfix `?`) to refer to the box which holds the variable. The latter
operations are achieved by having `lvalue` and `box` bound on the resulting
node, respectively.

#### `makeVarRef(name) -> isa ExpressionNode`

Makes a `varRef` node. `name` must be a symbol.

#### `makeVarStore(name, value) -> isa ExpressionNode`

Makes a `store` node with a `varRef` node for the target and the indicated
`value` binding. `name` must be a symbol.

#### `resolveImport(node, resolveFn) -> isa ExpressionNode`

Returns a node just like the given one (which must be an `import*` node),
except that it is resolved, using `resolveFn` to resolve any references.
Resolution means validating it and replacing wildcard selections with explicit
names.

This only actually causes `importModuleSelection` nodes to be altered.
Everything else just passes through as-is, if valid.

It is a fatal error (terminating the runtime) if `node` is found to be
invalid.

#### `withDynamicImports(node) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` nodes in the `statements` converted to their dynamic
forms.

#### `withFormals(node, [formal*]) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with `formals` (re)bound as given.

#### `withModuleDefs(node) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with `statements` and `yield` bindings processed to make the node
appropriate for use as a top-level module definition, and with an `info`
binding for the metainformation. This includes the following transformations:

* All `export` nodes are replaced with their `value` payloads.

* All `exportSelection` nodes are removed entirely.

* A `yield` is added, of a `@module` value with a payload that binds
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

#### `withName(node, name) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with the `name` binding as given. If the original `node` already had a
`name` binding, this replaces it in the result.

`name` must be a symbol.

#### `withResolvedImports(node, resolveFn) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with any `import*` or `export(import*)`nodes in the `statements` list
validated and transformed, by calling `resolveImport(node, resolveFn)`.

#### `withTop(node) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `varDef` node), except
with the addition of a `top: true` binding.

#### `withYieldDef(node, name) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with the addition of a yield definition binding for the given `name`.
`name` must be a symbol.

If the given `node` already has a yield definition, then this does not
replace it. Instead, this adds an initial variable definition statement
to the `statements` in the result, which binds the given name to the original
`yieldDef` name.

#### `withYieldDefIfAbsent(node, name) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with the addition of a yield definition binding for the given `name`, but
only if `node` does not already have a yield definition. If `node` *does*
have a yield definition, then this just returns `node`.
`name` must be a symbol.

This function is useful for propagating an outer yield definition into an
inner closure, especially with regards to providing the expected behavior
around implicit yielding of the final statement of a closure.

#### `withoutIntermediates(node) -> isa ExpressionNode`

Makes a node just like the given one, except without any "intermediate"
data payload bindings. These are bindings which are incidentally used
during typical tree node construction but which are not used for execution.
This includes `box`, `lvalue` and `interpolate`.

This function is useful in a couple of situations. Notably, it is used
when parsing expressions in a context where their otherwise-special
transformations should *not* apply. For example, this is used when parsing
parenthesized expressions to ensure that a parenthesized postfix-`*`
expression (e.g. `foo(bar, (baz*))`) is treated as a "fetch" and not as a
call argument interpolation.

#### `withoutTops(node) -> isa ExpressionNode`

Makes a node just like the given one (presumably a `closure` node), except
with no `top` decalarations in the `statements` list.

More specifically, for each variable defined to be `top`, a forward-declaring
`varDef` is added at the top of the `statements` list. The original `varDef`
is replaced with an equivalent `store(varRef(...), ...)` node. If any
so-transformed variables were `export`ed, then an `exportSelection` node is
added to the end of the `statements` list referencing all such variables.

It is *not* valid for a `varDef` with `box` type `@lazy` to be marked as a
`top`.
