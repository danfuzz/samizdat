Samizdat Layer 0: Core Library
==============================

Generators
----------

In general, the generator-oriented functions accept either generators
or collections as "generator" arguments. Collections and ints are implicitly
converted to generators, as if by a call to `generatorFromValue` (defined
below).

Collections generate each of their elements in order. Ints generate each
of their bits as an int, from low- to high-order, ending with the sign
bit.


<br><br>
### Generic Function Definitions

(none)


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `collectFilter(filterFunction, generators*) <> list`

Creates a filter generator over the indicated generators, and collects
the results of running it into a list.

This is a convenient and idiomatic shorthand for saying something like:

```
[(filterGenerator(generator, ...) { ... code ... })*]
```

**Syntax Note:** Used in the translation of comprehension forms.

#### `collectGenerator(generator) <> list`

Takes an arbitrary generator, and collects all of its generated results,
in generated order, into a list, returning that list.

#### `doFilter(filterFunction, generators*) <> void`

Iterates over the given generators, calling the given `filterFunction`
on generated items, iterating until at least one of the generators
is voided. The results from calling the `filterFunction` (if any) are
discarded. This function always returns void.

This is equivalent to calling `doGenerator` on a filter generator
constructed with the same arguments as a call to this function,
that is, something like:

```
doGenerator(filterGenerator(generator, ...) { ... code ... })
```

#### `doGenerator(generator) <> void`

Generator iterator, ignoring results. This takes a generator, calling
it repeatedly until it becomes voided. All results yielded by the
generator are ignored.

This function always returns void.

#### `doReduce(reduceFunction, [generators*], [baseValues*]) <> list`

Generator iterator with reduce semantics. This is similar to &mdash; but
not quite exactly &mdash; creating a `reduceGenerator`, iterating it, and
returning its last yielded value.

This repeatedly iterates on the given generators, calling the given
`reduceFunction` with the generated results as well as additional
arguments, in that order. The additional arguments start as the given
`baseValues` and are updated each time the `reduceFunction` returns non-void.

The iteration stops when any of the generators becomes voided, at which
point this function returns the most recently returned value from the
`reduceFunction`. If `reduceFunction` never returned a (non-void) value,
this function returns the `baseValues` list.

The `reduceFunction` must only ever return a list or void.

#### `doReduce1(reduceFunction, generator, baseValue) <> .`

Generator iterator with reduce semantics. This is a special case of
`doReduce`, where a single generator and single reduction value are used
instead of lists of each.

As opposed to `doReduce`, the `reduceFunction` can return any type of
value (not just a list), and similarly the overall result of calling this
function can be an arbitrary value.

#### `exclusiveRange(firstValue, increment, limit) <> generator`

End-exclusive range generator for int or single-character strings.
Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first call to the
resulting generator yields the `firstValue`, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string). If the value yielded
would be the same as or beyond the given `limit`, the generator becomes
voided.

As a special case, if `increment` is `0`, the resulting generator just
yields `firstValue` and then becomes voided.

**Syntax Note:** Used in the translation of `expression..!expression`
forms.

#### `filterGenerator(filterFunction, generator*) <> generator`

Filtering generator constructor. This takes any number of arbitrary generator,
and returns a generator which filters the generated results
with the given filter function. This works as follows:

Each time the outer (result) generator is called, it calls the argument
generators. If any of the argument generators has been voided, then the outer
generator also becomes voided.

Otherwise, the values yielded from the inner generators are passed to the
`filterFunction` as its arguments (in generator order). If that function
returns a value, then that value in turn becomes the yielded result of
the outer generator. If the filter function yields void, then the
value-in-progress is discarded, and the inner generator is retried, with
the same void-or-value behavior.

**Syntax Note:** Used in the translation of comprehension forms.

#### `generatorFromValue(value) <> generator`

Collection iteration generator constructor. This takes an arbitrary
"generator-amenable" value and returns a generator which successively yields
elements of that collection, per the specification for generators.

If passed a function, `generatorFromValue` returns the function directly,
on the assumption that it is already a generator, in order to make it easy
to write functions that can take either values or generators, coercing the
former to generators and transparently not-transforming generators passed
directly. That is, a function that wants to take values-or-generators can
safely call `generatorFromValue(valueOrGenerator)` without any up-front
type checking.

#### `inclusiveRange(firstValue, increment, limit) <> generator`

End-inclusive range generator for int or single-character strings.
Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first call to the
resulting generator yields the `firstValue`, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string). If the value yielded
would be beyond the given `limit`, the generator becomes voided.

As a special case, if `increment` is `0`, the resulting generator just
yields `firstValue` and then becomes voided.

**Syntax Note:** Used in the translation of `expression..expression`
forms.

#### `mapFromGenerator(generator) <> map`

Takes a generator which must yield map values, and collects all of its
generated results, in generated order, by building up an overall map,
as if by successive calls to `cat(map, map)`.

If there are mappings in the yielded results with equal keys, then the
*last* such mapping is the one that "wins" in the final result.

#### `openRange(firstValue, increment) <> generator`

Open (never voided) range generator for int or single-character strings.
Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first call to the
resulting generator yields the `firstValue`, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `firstValue` is a string).

As a special case, if `increment` is `0`, the resulting generator just
yields `firstValue` and then becomes voided.

**Syntax Note:** Used in the translation of `expression..+` forms.

#### `optGenerator(generator) <> generator`

"Optional" generator constructor. This takes an arbitrary generator,
returning a new generator that always yields lists and never
becomes voided. As long as the underlying generator yields a value, the
returned generator yields a single-element list of that value. Once the
underlying generator is voided, the returned generator yields the empty
list, and will continue doing so ad infinitum.

#### `paraGenerator(generators*) <> generator`

Parallel generator combination constructor. This takes an arbitrary number of
generators, and returns a generator that yields lists.

Each yielded list consists of values yielded from the individual generators,
in passed order. The generator becomes voided when *any* of the individual
generators is voided.

**Syntax Note:** Used in the translation of `for` forms.

#### `reduceGenerator(reduceFunction, [generators*], [baseValues*]) <> generator`

Reducing filter generator constructor. This takes a reducer function, a
list (per se) of other generators, and arbitrary further `baseValues`
(initial value) arguments, and returns a generator which can act as a filter
on the values generated by the sub-generators as well as on its own
previously-yielded values.

Each time the outer (result) generator is called, it calls the inner
generators to produce values, and then calls the `reduceFunction`, passing
it (in order) the newly-generated values as well as the list of values
most recently yielded by a previous `reduceFunction` call. For the
first iteration, and for any iterations before the `reduceFunction` yields
a value, the `baseValues` are used as the additional arguments.

The generator yields whatever non-void value is yielded from the latest
call to `reduceFunction`. If the `reduceFunction` call yields void, then the
value-in-progress is discarded, and the inner generator is retried, with
the same void-or-value behavior.

When called, the `reduceFunction` must either return a list or return void.

The result generator becomes voided when any of the inner generators
become voided, in which case the `reduceFunction` is not called for that
iteration.

#### `seqGenerator(generators*) <> generator`

Sequential generator combination constructor. This takes an arbitrary number
of generators, and returns a generator that yields from each of
the generators in argument order.

As each generator becomes voided, the next one (in argument order) is called
upon to generate further elements. The generator becomes voided after the
final argument is voided.

#### `stringFromGenerator(generator) <> list`

Takes a generator which must yield only strings, and collects all of its
generated results, in generated order, into a single concatenated string.

#### `tokenGenerator(generator) <> generator`

Filter generator that produces a sequence of type-only derived values
from whatever the underlying generator produces. This is, in particular,
the generator used by default to produce single-character
tokens when using the library function `pegApply` to perform
string tokenization. The function name is meant to be suggestive of the
expected use case.
