Samizdat Layer 0: Core Library
==============================

core::Lang0Node
---------------

These are a set of accessor functions for various elements of the execution
tree node types.

<br><br>
### Function Definitions

In addition to the above, the module `core::Lang0` also defines a number
of accessors for the elements of execution tree nodes.

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
nodes of type `expression`, `interpolate`, `literal`, `varBind`, `varDef`,
and `varDefMutable`.

#### `get_yield(node) <> node | void`

Gets the yield of a `closure` node, if any.

#### `get_yieldDef(node) <> . | void`

Gets the yield definition name of a `closure` node, if any.
