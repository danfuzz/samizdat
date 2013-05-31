Samizdat Layer 0: Core Library
==============================

Ultraprimitives
---------------

This set of functions is required in order to execute
parse trees as produced by `sam0Tree` (see which).

<br><br>
### Primitive Definitions


#### `makeToken type value? <> token`

Returns a token with the given type tag (an arbitrary value)
and optional data payload value (also an arbitrary value). These
equivalences hold for *Samizdat Layer 0* source code:

```
v = @[key];         is equivalent to   v = makeToken(key);
v = @[key = value]; is equivalent to   v = makeToken(key, value);
```

#### `makeList rest* <> list`

Returns a list with the given elements (in argument order).
These equivalences hold for *Samizdat Layer 0* source code:

```
v = [v1];      is equivalent to   v = makeList(v1);
v = [v1, v2];  is equivalent to   v = makeList(v1, v2);
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
makeList = { rest* :: <> rest; };
```

#### `makeMap rest* <> map`

Returns a map with the given key-value mappings (in argument
order), with each key-value pair represented as two consecutive
arguments. The number of arguments passed to this function must be
even. It is valid to repeat keys in the arguments to this function, in
which case the *final* value mapping for any given key in the argument
list (in argument order) is the one that ends up in the result. These
equivalences hold for *Samizdat Layer 0* source code:

```
v = [k1=v1];         is equivalent to   v = makeMap(k1, v1);
v = [k1=v1, k2=v2];  is equivalent to   v = makeMap(k1, v1, k2, v2);
[etc.]
```

**Note:** The equivalence requires at least two arguments, even though
the function is happy to operate given zero arguments.

**Note:** Technically, this function could be defined in-language as the
following. (See `makeList` for discussion.):

```
makeMap = { rest* ::
    makeStep = { key value rest* ::
        restMap = apply(makeMap, rest);
        <> ifValue { <> mapGet(restMap, key); }
            { <> restMap; }
            { <> mapPut(restMap, key, value); };
    };

    <> ifTrue { <> eq(rest, []) }
        { <> [=]; }
        { <> apply(makeStep, rest); };
};
```

#### `makeUniqlet() <> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist). This equivalence holds for *Samizdat Layer 0* source
code:

```
v = @@;   is equivalent to   v = makeUniqlet();
```


<br><br>
### In-Language Definitions

(none)
