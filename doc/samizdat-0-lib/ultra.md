Samizdat Layer 0: Core Library
==============================

Ultraprimitives
---------------

This set of functions is required in order to execute
parse trees as produced by `sam0Tree` (see which).

<br><br>
### Primitive Definitions

#### `makeList(rest*) <> list`

Returns a list with the given elements (in argument order).
These equivalences hold for *Samizdat Layer 0* source code:

```
v = [v1];      is equivalent to  v = makeList(v1);
v = [v1, v2];  is equivalent to  v = makeList(v1, v2);
[etc.]
```

**Note:** The equivalence requires at least one argument, even though
the function is happy to operate given zero arguments.

**Note:** Technically, this function could be defined in-language as the
following, but for practical reasons &mdash; e.g. and in particular,
expected ordering of human operations during the course of
bootstrapping an implementation, as well as efficiency of
implementation (without sacrificing clarity) &mdash; it makes sense to
keep this defined as an "ultraprimitive":

```
fn makeList(rest*) { <> rest };
```

#### `makeMapping(keys*, value) <> map`

This makes a map which maps any number of keys (including none)
to the same value. If no keys are specified, then this function returns
the empty map. For example:

```
v = [(k1): v];      is equivalent to  v = makeMapping(k1, v);
v = [[k1, k2]*: v;  is equivalent to  v = makeMapping(k1, k2, v);
[etc.]
```

Note that the argument list is "stretchy" in front, which isn't really
representable in Samizdat syntax as presented.

#### `makeToken(type, value?) <> token`

Returns a token with the given type tag (an arbitrary value)
and optional data payload value (also an arbitrary value). These
equivalences hold for *Samizdat Layer 0* source code:

```
v = @[(type)];         is equivalent to  v = makeToken(type);
v = @[(type): value];  is equivalent to  v = makeToken(type, value);
```

#### `makeUniqlet() <> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist). This equivalence holds for *Samizdat Layer 0* source
code:

```
v = @@;  is equivalent to  v = makeUniqlet();
```


<br><br>
### In-Language Definitions

(none)
