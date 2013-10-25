Samizdat Layer 0: Core Library
==============================

core::Generator / proto::Generator
----------------------------------

The `Generator` module exports a `Generator` protocol and a set of
utility functions for making and using various generators.

Note that all collections are also generators (which generate their
elements in sequence).


<br><br>
### Generic Function Definitions: `Generator` protocol.

#### `collect(generator) <> list`

Collects all the elements yielded by the generator into a list. Returns
the list.

Calling `collect` on an unbounded generator (one with an infinite number
of elements to generate) is a fatal error (terminating the runtime).

The default implementation of this method iterates over calls to
`nextValue()` in the expected manner, collecting up all the yielded
results.

#### `filter(generator, filterFunction) <> list`

Collects the result of calling `filterFunction` on each of the elements
yielded by the generator, into a list. Returns the list. If a given call
to `filterFunction` returns void, then there is no result element for the
corresponding generated value.

Calling `filter` on an unbounded generator (one with an infinite number
of elements to generate) is a fatal error (terminating the runtime).

The default implementation of this method iterates over calls to
`nextValue()` in the expected manner, collecting up all the yielded
results.

**Note:** The function `filterAll` is a multi-generator generalization
of this function.

#### `nextValue(generator, box) <> generator | void`

Generates the next item in `generator`, if any. If there is a generated
element, calls `store(box, elem)` and returns a generator which can
generate the remainder of the elements. If there is no generated element,
calls `store(box)` (storing void), and returns void.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### Constant: `nullGenerator`

A generator which is perennially voided. It is defined as `@NullGenerator`,
along with generator method bindings for that type.

#### `collectAsMap(generator) <> map`

Takes a generator which must yield map values, and collects all of its
generated results, in generated order, by building up an overall map,
as if by calling `cat({}, map1, map2, ...)` on all the results.

If there are mappings in the yielded results with equal keys, then the
*last* such mapping is the one that "wins" in the final result.

#### `doReduce(reduceFunction, generator, baseValues*) <> list`

Generator iterator with reduce semantics.

This repeatedly iterates on the given generator, calling the given
`reduceFunction` with the generated result as well as additional
arguments, in that order. The additional arguments start as the given
`baseValues` and are updated each time the `reduceFunction` returns non-void.

The iteration stops when the generator becomes voided, at which
point this function returns the most recently returned value from the
`reduceFunction`. If `reduceFunction` never returned a (non-void) value,
this function returns the `baseValues` list.

The `reduceFunction` must only ever return a list or void.

#### `doReduce1(reduceFunction, generator, baseValue) <> .`

Generator iterator with reduce semantics. This is a special case of
`doReduce`, where a single reduction value is used instead of a list of them.

As opposed to `doReduce`, the `reduceFunction` can return any type of
value (not just a list), and similarly the overall result of calling this
function can turn out to be an arbitrary value.

#### `filterAll(filterFunction, generators*) <> list`

Creates a filter generator over the indicated generators, and collects
the results of running it into a list.

This is a convenient and idiomatic shorthand for saying something like:

```
[(makeFilterGenerator(generator, ...) { ... code ... })*]
```

**Syntax Note:** Used in the translation of comprehension forms.

#### `filterPump(filterFunction, generators*) <> void`

Iterates over the given generators, calling the given `filterFunction`
on generated items, iterating until at least one of the generators
is voided. The results from calling the `filterFunction` (if any) are
discarded. This function always returns void.

This is equivalent to calling `generatorPump` on a filter generator
constructed with the same arguments as a call to this function,
that is, something like:

```
generatorPump(makeFilterGenerator(generator, ...) { ... code ... })
```

**Syntax Note:** Used in the translation of `for` forms.

#### `generatorPump(generator) <> void`

Generator iterator, ignoring results. This takes a generator, calling
it repeatedly until it becomes voided. All results yielded by the
generator are ignored.

This function always returns void.

#### `makeCollectionGenerator(coll, optIndex?) <> generator`

Collection generator constructor. This takes a collection and optional
start index, returning a generator which generates the elements of the
collection starting at the index (or at `0` if no index is supplied).

**Note:** This function makes a value of type `"CollectionGenerator"`,
with the collection and index as elements of the payload. That type has
appropriate `Generator` method bindings.

**Note:** When called on a core collection, `nextValue` uses this function.

#### `makeFilterGenerator(filterFunction, generators*) <> generator`

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

#### `makeListWrapGenerator(generator) <> generator`

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

#### `makeOptGenerator(generator) <> generator`

"Optional" generator constructor. This takes an arbitrary generator,
returning a new generator that always yields lists and never
becomes voided. As long as the underlying generator yields a value, the
returned generator yields a single-element list of that value. Once the
underlying generator is voided, the returned generator yields the empty
list, and will continue doing so ad infinitum.

**Note:** This function makes a value of the type `"OptGenerator"` with
the given `generator` as the payload. That type has
appropriate `Generator` method bindings.

#### `makeParaGenerator(generators*) <> generator`

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

#### `makeSerialGenerator(generators*) <> generator`

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

#### `makeValueGenerator(value) <> generator`

Creates an unnbounded generator (one with infinite elements) which always
yields the given `value` upon `nextValue()` call.

**Note:** This function makes a value of type `"ValueGenerator"` with `value`
as the payload. That type has appropriate `Generator` method bindings.
