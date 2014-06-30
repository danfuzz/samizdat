Samizdat Layer 0: Core Library
==============================

core.Generator / proto.Generator
--------------------------------

The `Generator` module exports a `Generator` protocol and a set of
utility functions for making and using various generators.

Note that all collections are also generators (which generate their
elements in sequence).

Because they are so commonly used, the following definitions are
exported to the standard global variable environment:

* `collect`
* `fetch`
* `interpolate`
* `maybeValue`
* `nextValue`


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(generator, optFilterFunction?) -> list`

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

#### `fetch(generator) -> . | void`

Returns the sole generated value of the generator, or void if given
a voided generator. It is a fatal error (terminating the runtime) if
`generator` is capable of generating more than one value.

#### `nextValue(generator, box) -> generator | void`

Generates the next item in `generator`, if any. If there is a generated
element, calls `store(box, elem)` and returns a generator which can
generate the remainder of the elements. If there is no generated element,
does nothing (in particular, does not make a `store` call on `box`), and
returns void.


<br><br>
### Primitive Definitions

#### `interpolate(generator) -> . | void`

Interpolation helper. This takes a generator, `collect`s it, and then does
the following based on the size of the collected result:

* If the result is empty (the empty list), then this function returns void.
* If the result has exactly one element, then this function returns
  that element.
* In all other cases, this terminates the runtime with an error.

This function could be implemented as something like:

```
fn interpolate(generator) {
    def list = [generator*];
    if (list[1]) {
        ## Die with error.
    } else {
        return list[0]
    }
}
```

**Syntax Note:** Used in the translation of `expression*` forms when they
are *not* collection constructor or function call arguments.


#### `maybeValue(function) -> list`

Function call helper, to deal with value-or-void situations. This calls
`function` with no arguments, wrapping its return value in a list and in
turn returning that list. That is, if `function` returns `value`, then this
function returns `[value]` (a single-element list), and if `function` returns
void, then this function returns `[]` (the empty list).

This function could be implemented as:

```
fn maybeValue(function) {
    return (def v = function()) & [v] | []
}
```

or more primitively as:

```
fn maybeValue(function) {
    return ifValue(function, { v -> [v] }, { -> [] })
}
```

**Syntax Note:** Used in the translation of string interpolation and
`expression?` forms.

#### `stdCollect(generator, optFilterFunction?) -> list`

"Standard" implementation of `collect`, in terms of `nextValue`. This
function is provided as a convenient function to bind `collect` to, for
types that don't have anything fancier to do.

#### `stdFetch(generator) -> . | void`

"Standard" implementation of `fetch`, in terms of `nextValue`. This
function is provided as a convenient function to bind `fetch` to, for
types that don't have anything fancier to do.

#### `unboundedCollect(generator, optFilterFunction?) ->  n/a  ## Terminates the runtime.`

Handy implementation of `collect` which simply dies with a message indicating
that the given generator is unbounded (that is, has infinite elements).
This function is provided as a convenient thing to bind `collect` to, for
appropriate types.

#### `unboundedFetch(generator) ->  n/a  ## Terminates the runtime.`

Handy implementation of `fetch` which simply dies with a message indicating
that the given generator is unbounded (that is, has infinite elements).
This function is provided as a convenient thing to bind `fetch` to, for
appropriate types.


<br><br>
### In-Language Definitions

#### Constant: `nullGenerator`

A generator which is perennially voided. It is defined as `@NullGenerator`,
along with generator method bindings for that type.

#### `collectAsMap(generator) -> map`

Takes a generator which must yield map values, and collects all of its
generated results, in generated order, by building up an overall map,
as if by calling `cat({}, map1, map2, ...)` on all the results.

If there are mappings in the yielded results with equal keys, then the
*last* such mapping is the one that "wins" in the final result.

#### `filterAll(filterFunction, generators*) -> list`

Creates a filter generator over the indicated generators, and collects
the results of running it into a list.

This is a convenient and idiomatic shorthand for saying something like:

```
[(makeFilterGenerator(generator, ...) { ... code ... })*]
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
generatorPump(makeFilterGenerator(generator, ...) { ... code ... })
```

**Syntax Note:** Used in the translation of `for` forms.

#### `generatorPump(generator) -> void`

Generator iterator, ignoring results. This takes a generator, calling
it repeatedly until it becomes voided.

This function returns the last value yielded by the generator. If the
generator never yielded a value, this function returns void.

#### `makeFilterGenerator(filterFunction, generators*) -> generator`

Filtering generator constructor. This takes any number of arbitrary
generators, and returns a generator which filters the generated results
with the given filter function. This works as follows:

Each time `nextValue()` is called on the outer (result) generator, it calls
`nextValue()` on the argument generators. If any of the argument generators
has been voided, then the outer generator also becomes voided.

Otherwise, the values yielded from the inner generators are passed to the
`filterFunction` as its arguments (in generator order). If that function
returns a value, then that value in turn becomes the yielded result of
the outer generator. If the filter function yields void, then the
value-in-progress is discarded, and the inner generator is retried, with
the same void-or-value behavior.

**Note:** This function makes a value of type `"FilterGenerator"`
with `filterFunction` and either a `ParaGenerator` or `ListWrapGenerator`
as the payload. That type has appropriate `Generator` method bindings.

**Syntax Note:** Used in the translation of comprehension forms.

#### `makeListWrapGenerator(generator) -> generator`

List wrapping generator combination constructor. This takes a single
generator, and returns a generator that yields single-element lists.
This is a special case of `makeParaGenerator` and exists so that
single-element para-generators can be implemented without undue overhead.

Each yielded list consists of values yielded from the individual generators,
in passed order. The generator becomes voided when *any* of the individual
generators is voided.

**Note:** This function makes a value of type `"ListWrapGenerator"`
with `generator` as the payload. That type has appropriate `Generator`
method bindings.

#### `makeOptGenerator(generator) -> generator`

"Optional" generator constructor. This takes an arbitrary generator,
returning a new generator that always yields lists and never
becomes voided. As long as the underlying generator yields a value, the
returned generator yields a single-element list of that value. Once the
underlying generator is voided, the returned generator yields the empty
list, and will continue doing so ad infinitum.

**Note:** This function makes a value of the type `"OptGenerator"` with
the given `generator` as the payload. That type has
appropriate `Generator` method bindings.

#### `makeParaGenerator(generators*) -> generator`

Parallel generator combination constructor. This takes an arbitrary number of
generators, and returns a generator that yields lists.

Each yielded list consists of values yielded from the individual generators,
in passed order. The generator becomes voided when *any* of the individual
generators is voided.

Special cases:

* If passed no arguments, this returns `nullGenerator`.

* If passed one argument, this returns a result from `makeListWrapGenerator`.

**Note:** Special cases aside, this function makes a value of type
`"ParaGenerator"` with `[generators*]` as the payload. That type has
appropriate `Generator` method bindings.

**Syntax Note:** Used in the translation of `for` forms.

#### `makeRepeatGenerator(size, optValue?) -> generator`

Repeated-value generator. This takes a size, which must be a non-negative
int, and an optional value, producing a generator that yields the given
value (or `null` if no value was supplied) the indicated number of times.

Special cases:

* If passed `0` for `size`, this returns `nullGenerator`.

**Note:** Special cases aside, this function makes a value of type
`"RepeatGenerator"` with `{size, value}` as the payload. That type has
appropriate `Generator` method bindings.

**Syntax Note:** Used in the translation of `for` and comprehension forms.

#### `makeSerialGenerator(generators*) -> generator`

Sequential generator combination constructor. This takes an arbitrary number
of generators, and returns a generator that yields from each of
the generators in argument order.

As each generator becomes voided, the next one (in argument order) is called
upon to generate further elements. The generator becomes voided after the
final argument is voided.

Special cases:

* If passed no arguments, this returns `nullGenerator`.

* If passed one argument, this returns that argument directly.

**Note:** Special cases aside, this function makes a value of type
`"SerialGenerator"` with `[generators*]` as the payload. That type has
appropriate `Generator` method bindings.

#### `makeValueGenerator(value) -> generator`

Creates an unnbounded generator (one with infinite elements) which always
yields the given `value` upon `nextValue()` call.

**Note:** This function makes a value of type `"ValueGenerator"` with `value`
as the payload. That type has appropriate `Generator` method bindings.
