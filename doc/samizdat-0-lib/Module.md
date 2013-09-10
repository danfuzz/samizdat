Samizdat Layer 0: Core Library
==============================

Modules
-------

The *Layer 0* support for modules consists of library functions that
allow manipulation of module values, but does not include anything
that affects global system state.

An uninitialized module value is a derived value of the form:

```
@[module: [
    name:    "name",
    version: "version",
    imports: ["name1", "name2", ...],
    init:    { modules :: ... }
]]
```

That is, `name` and `version` are strings, `imports` is a list of strings,
and `init` is a function that should take a `modules` parameter.
The `modules` parameter should be expected to itself be a map from names to
*initialized* modules.

An initialized module value is a derived value of the form:

```
@[module: [
    name:    "name",
    version: "version",
    imports: ["name1", "name2", ...],
    exports: [name1: value1, name2: value2, ...]
]]
```

That is, it is just like an uninitialized module, except that the
`init` mapping is replaced by an `exports` map.


<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `moduleInit(context) <> map`

Given an execution context (that is, a map from names to values meant to
be used as global variable bindings), extract the names that appear to
be module names (`"module:..."`), and perform initialization on the so-bound
modules. Returns a map from module names to initialized modules.
