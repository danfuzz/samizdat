Samizdat Layer 0: Core Library
==============================

Functions And Code
------------------

<br><br>
### Primitive Definitions

#### `apply function (value* list)? <> . | ~.`

Calls the given function with the given arguments.

It is valid to pass no arguments other than `function`, in which case the
function is called with no arguments.

If any other arguments are supplied, then each of the initial `value`
arguments is taken to be an arbitrary value, and the final `list`
argument must be a list, whose contents are "flattened" into the
final list of arguments to pass to the function. For example, this is
a five-argument call: `apply fizmoFunc @foo @bar @baz [@frob @twiddle]`

This function returns whatever the called function returned (including
void).

#### `object implementation state <> function`

Constructs and returns an "object". In *Samizdat Layer 0* an object is
merely the combination of a mutable `state` value (an arbitarary
value) with an `implementation` function (an arbitrary function).
The return value from the call to `object` is another function,
referred to as the "object interface function".

When the object interface function is called, it in turn calls the
`implementation` function, passing it the same arguments as it was
called with, along with two additional arguments at the head of the
argument list:

* The first argument is a `yield` function. This function must be
  called exactly once during the course of a call to `implementation`.
  This is used to indicate the return value from the object interface
  function. If `yield` is called with no argument, then the object
  interface function returns void. If `yield` is called with one argument,
  then the object interface function returns that value.

  When the `yield` function is called, it does *not* cause the
  implementation function to return; it merely causes the eventual
  result value (or void) to be cached until the function does finally
  return.

* The second argument is a `state` value. This is the latest state
  for the object, as defined immediately below.

The implementation function can return either a value or void. If
it returns a value, then the value becomes the new `state` for
the object, replacing either the `state` originally passed to
`object`, or whatever other state had been returned in the mean time.

To avoid confusion as well as hew closely to the actor model,
it is invalid (terminating the runtime) for an object implementation
function to call its own interface function, either directly or
indirectly. To implement a recursive operation, it is necessary to do
so without going through the interface.

#### `sam0Eval context expressionNode <> . | ~.`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document. It is valid for
the expression to yield void, in which case this function returns
void. Evaluation is performed in an execution context that includes
all of the variable bindings indicated by `context`, which must be a
map.

Very notably, the result of calling `sam0Tree` is valid as the
`expressionNode` argument here.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.

#### `sam0Tree string <> functionNode`

Parses the given string as a program written in *Samizdat Layer 0*.
Returns a `function` node (as defined by the parse tree semantics)
that represents the function.


<br><br>
### In-Language Definitions

#### `yCombinator function <> function`

The Y combinator, in a form suitable for use in Samizdat Layer 0 when
defining self-recursive functions.

**Summary**

If you want to make a self-recursive function in *Samizdat Layer 0*,
you can write it like this:

```
myRecursiveFunction = yCombinator { selfCall ::
    # Inner function.
    <> { myArg1 myArg2 ... ::
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
and the "applicative-order Y combinator", but this function is not
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

#### `yStarCombinator functions* <> [functions*]`

The Y* combinator, in a form suitable for use in Samizdat Layer 0 when
defining sets of mutually-recursive functions.

This is like `yCombinator`, except that it can take any number of
functions as arguments, resulting in a list of mutual-recursion-enabled
result functions.

If you want to make a set of N mututally-recursive functions in
*Samizdat Layer 0*, you can write it like this:

```
myRecursiveFunctions = xCombinator
    { selfCall1 selfCall2 ... ::
        # Inner function.
        <> { myArg1 myArg2 ... ::
            ... my code ...
            selfCall1 args ... # Call this function self-recursively.
            selfCall2 args ... # Call the other function.
            ... my code ...
        }
    }
    { selfCall1 selfCall2 ... ::
        # Inner function.
        <> { myArg1 myArg2 ... ::
            ... my code ...
            selfCall1 args ... # Call the other function.
            selfCall2 args ... # Call this function self-recursively.
            ... my code ...
        }
    };
```

This results in an array of functions corresponding to the original argument
functions. Each of those functions can recurse by calling any of the other
functions, via the arguments passed into the wrapper functions (arguments
prefixed `selfCall` in the example here).

See `yCombinator` for more detailed discussion and explanation.
