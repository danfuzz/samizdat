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
makeList = { rest* :: <> rest };
```

#### `makeMap(rest*) <> map`

Returns a map with the given key-value mappings, built up in argument
order. Each argument must be either a map or a list of at least two elements.
In the case of a map, the contents of the map are to be included in the
result. In the case of a list, the first element is the value to bind into
the result map, and the rest of the elements are keys to which the first
element is to be bound.

It is valid to repeat keys in the arguments to this function, in
which case the *final* value mapping for any given key in the argument
list (in argument order) is the one that ends up in the result. These
equivalences hold for *Samizdat Layer 0* source code:

```
v = [(k1): v1];          is equivalent to  v = makeMap([v1, k1]);
v = [(k1): v1, k2: v2];  is equivalent to  v = makeMap([v1, k1], [v2, k2]);
[etc.]

v = [k1..k2: v];         is equivalent to  v = makeMap([v, k1..k2]);
v = [:, m1*, m2*];       is equivalent to  v = makeMap(m1, m2);
```

#### `makeMapReversed(rest*) <> map`

This is the same as `makeMap`, except in how duplicate keys are resolved.
In this function, the *first* value mapping for any given key is the one
that ends up in the result.

#### `makeRange(start, end) <> list`

Returns a list consisting of the values ranging from `start` to `end`
inclusive. Both arguments must be of type int.

#### `makeToken(type, value?) <> token`

Returns a token with the given type tag (an arbitrary value)
and optional data payload value (also an arbitrary value). These
equivalences hold for *Samizdat Layer 0* source code:

```
v = @[(key)];         is equivalent to  v = makeToken(key);
v = @[(key): value];  is equivalent to  v = makeToken(key, value);
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
