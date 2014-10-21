Samizdat Language Guide
=======================

Module Metainformation
----------------------

This section describes the metainformation format used by `saminfo` files
and returned by `LangNode::makeInfoTable`. This is used to describe the
high-level properties of a module.

### Top level

The top level is a symbol table which binds any of `exports`, `imports`,
and/or `resources`. Each of these bindings is optional.

```
{
    exports: ...,
    imports: ...,
    resources: ...
}
```

### `exports`

The `exports` section describes the bindings exported from the module.
It is a map from string names to types.

```
{
    exports: {
        someFunction: Value,
        SOME_CONSTANT: Value,
        ...
    },
    ...
}
```

### `imports`

The `imports` section describes the modules imported into the module.
It is a map from path references to import maps.

An import map is a map from string names to expected types. In addition to
string names as keys, the special key `@module` indicates that the
module as a whole is to be imported.

```
{
    imports: {
        @external{name: "core.Lang0"}: {@module: Value},
        @internal{name: "helpers"}: {helpFunc: Value, otherFunc: Value},
        ...
    },
    ...
}
```

### `resources`

The `resources` section describes the resources imported into the module.
It is a map from path references to lists of format names. (It is a list
because it is valid to import a resource multiple times using different
formats.) Resource path references must always be `@internal`.

Format names are strings, with the same meanings as for the resource `import`
syntax.

```
{
    resources: {
        @internal{name: "etc/template.txt"}: ["utf8"],
        ...
    },
    ...
}
```

### Path references

A path reference is an `@external{...}` or `@internal{...}` value, as
described in the "Execution Trees" section.

### Types

The only type currently defined is `Value`, which doesn't actually restrict
anything. (It indicates that the type of the export is `Value`, and
*everything* is by definition a `Value`.)
