Samizdat Layer 0: Core Library
==============================

core.Generator / Generator Protocol
-----------------------------------

The `Generator` module exports a `Generator` protocol and a set of
utility functions for making and using various generators.

Note that all collections are also generators (which generate their
elements in sequence).

<br><br>
### Classes

* [FilterGenerator](FilterGenerator.md)
* [ListWrapGenerator](ListWrapGenerator.md)
* [NullGenerator](NullGenerator.md)
* [OptGenerator](OptGenerator.md)
* [ParaGenerator](ParaGenerator.md)
* [RepeatGenerator](RepeatGenerator.md)
* [SerialGenerator](SerialGenerator.md)
* [ValueGenerator](ValueGenerator.md)


<br><br>
### Method Definitions: `Generator` protocol.

#### `.collect(optFilterFunction?) -> list`

Collects all the elements yielded by the generator into a list. Returns
the list. If a filter function is given, calls it with each element (as
a sole argument), and the collected results are the results of calling
the function instead of the originally-generated values. If the filter
function returns void for a given element, then that element is not
represented in the collected output.

Calling `collect` on an unbounded generator (one with an infinite number
of elements to generate) is a fatal error (terminating the runtime).

The default implementation of this method iterates over calls to
`nextValue()` in the expected manner, collecting up all the yielded
results.

**Note:** The function `filterAll` is a multi-generator generalization
of this function.

#### `.fetch() -> . | void`

Returns the sole generated value of the generator, or void if given
a voided generator. It is a fatal error (terminating the runtime) if
`this` is capable of generating more than one value.

**Syntax Note:** Used in the translation of `expression*` forms when they
are *not* collection constructor or function call arguments.

#### `.nextValue(box) -> generator | void`

Generates the next item in `this`, if any. If there is a generated
element, calls `box.store(elem)` and returns a generator which can
generate the remainder of the elements. If there is no generated element,
does nothing (in particular, does not make a `store` call on `box`), and
returns void.


<br><br>
### Functions

#### `filterAll(filterFunction, generators*) -> list`

Creates a filter generator over the indicated generators, and collects
the results of running it into a list.

This is a convenient and idiomatic shorthand for saying something like:

```
[(FilterGenerator.new(generator, ...) { ... code ... })*]
```

**Syntax Note:** Used in the translation of comprehension forms.

#### `filterPump(filterFunction, generators*) -> void`

Iterates over the given generators, calling the given `filterFunction`
on generated items, iterating until at least one of the generators
is voided. This function returns the last non-void value yielded by
`filterFunction`. If `filterFunction` never yields a value, then this
function returns void.

This is equivalent to calling `generatorPump` on a filter generator
constructed with the same arguments as a call to this function,
that is, something like:

```
generatorPump(FilterGenerator.new(generator, ...) { ... code ... })
```

**Syntax Note:** Used in the translation of `for` forms.

#### `generatorPump(generator) -> void`

Generator iterator, ignoring results. This takes a generator, calling
it repeatedly until it becomes voided.

This function returns the last value yielded by the generator. If the
generator never yielded a value, this function returns void.

#### `stdCollect(generator, optFilterFunction?) -> list`

"Standard" implementation of `collect`, in terms of `nextValue`. This
function is provided as a convenient function to bind `collect` to, for
classes that don't have anything fancier to do.

#### `stdFetch(generator) -> . | void`

"Standard" implementation of `fetch`, in terms of `nextValue`. This
function is provided as a convenient function to bind `fetch` to, for
classes that don't have anything fancier to do.

#### `unboundedCollect(generator, optFilterFunction?) ->  n/a  ## Terminates the runtime.`

Handy implementation of `collect` which simply dies with a message indicating
that the given generator is unbounded (that is, has infinite elements).
This function is provided as a convenient thing to bind `collect` to, for
appropriate classes.

#### `unboundedFetch(generator) ->  n/a  ## Terminates the runtime.`

Handy implementation of `fetch` which simply dies with a message indicating
that the given generator is unbounded (that is, has infinite elements).
This function is provided as a convenient thing to bind `fetch` to, for
appropriate classes.


<br><br>
### Constants

#### `nullGenerator`

A generator which is perennially voided. It is an instance of `NullGenerator`.
