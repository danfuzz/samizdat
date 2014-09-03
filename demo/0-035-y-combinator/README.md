Y Combinator and Friends
========================

#### `yCombinator(function) -> function`

The Y combinator, in a form suitable for use in Samizdat Layer 0 when
defining self-recursive functions.

**Summary**

If you want to make a self-recursive function in Samizdat Layer 0,
you can write it like this:

```
myRecursiveFunction = yCombinator { selfCall ->
    ## Inner function.
    { myArg1 myArg2 ... ->
        ... my code ...
        selfCall args ...
        ... my code ...
    }
};
```

In the example above, a call to `selfCall` will end up recursively
calling into the (so-labeled) inner function.

**Detailed Explanation**

This function takes another function as its argument, called the
"wrapper" function. That function must take a single argument, which itself
is a function, called the "recurser" function. The "wrapper" function must
return yet another function, called the "inner" function. The return value
of this function, called the "result" function, is a function which, when
called, ends up calling the wrapper function and then calling the inner
function that the wrapper function returned.

The inner function is an arbitrary function, taking arbitrary arguments,
returning anything including void, and performing any arbitrary
actions in its body. In particular, it can be written to call the
"recurser" argument passed to its wrapper. If it does so, that will in
turn result in a recursive call to itself.

This function is used to write recursive functions without relying
on use-before-def variable binding.

See Wikipedia [Fixed-point
combinator](http://en.wikipedia.org/wiki/Fixed-point_combinator) for
more details about some of the theoretical basis for this stuff.
This function is in the "Y combinator family" but is not exactly any
of the ones described on that page. It is most similar to the Z
combinator, which is also known as the "call-by-value Y combinator"
and the "applicative-order Y combinator," but this function is not
*exactly* the Z combinator. In particular, this version is written
such that the wrapper function always gets called directly with the
result of a U combinator call. It is unclear whether this results
in any meaningful difference in behavior. See also [this question on
StackOverflow](http://stackoverflow.com/questions/16258308).

In traditional notation (and with
the caveat that `a` represents an arbitrary number of arguments here),
this function would be written as:

```
U = λx.x x
Y_sam = λf . U (λs . (λa . (f (U s)) a))
```

#### `yStarCombinator(functions*) -> [functions*]`

The Y* combinator, in a form suitable for use in Samizdat Layer 0 when
defining sets of mutually-recursive functions.

This is like `yCombinator`, except that it can take any number of
functions as arguments, resulting in a list of mutual-recursion-enabled
result functions.

If you want to make a set of N mututally-recursive functions in
Samizdat Layer 0, you can write it like this:

```
myRecursiveFunctions = yStarCombinator
    { selfCall1, selfCall2, ... ->
        ## Inner function.
        { myArg1, myArg2, ... ->
            ... my code ...
            selfCall1(args, ...)  ## Call this function self-recursively.
            selfCall2(args, ...)  ## Call the other function.
            ... my code ...
        }
    }
    { selfCall1, selfCall2, ... ->
        ## Inner function.
        { myArg1, myArg2, ... ->
            ... my code ...
            selfCall1(args, ...)  ## Call the other function.
            selfCall2(args, ...)  ## Call this function self-recursively.
            ... my code ...
        }
    };
```

This results in an array of functions corresponding to the original argument
functions. Each of those functions can recurse by calling any of the other
functions, via the arguments passed into the wrapper functions (arguments
prefixed `selfCall` in the example here).

See `yCombinator` for more detailed discussion and explanation.
