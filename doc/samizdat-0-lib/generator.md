Samizdat Layer 0: Core Library
==============================

Generators
----------


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `filterGenerator(filterFunction, value) <> generator`

Filtering generator constructor. This takes an arbitrary generator or
value (which is coerced to a generator as if by calling `generatorFromValue`
on it), and returns a generator which filters the generated results
with the given filter function. This works as follows:

Each time the outer (result) generator is called, it calls the argument
generator. If the argument generator has been voided, then the outer
generator also becomes voided.

Otherwise, the value yielded from the inner generator is passed to the
`filterFunction` as its sole argument. If that function returns a value,
then that value in turn becomes the yielded result of the outer generator.
If the filter function yields void, then the value-in-progress is discarded,
and the inner generator is retried, with the same void-or-value behavior.

#### `generatorForExclusiveRange(first, increment, limit) <> generator`

End-exclusive range generator for int or single-character strings.
Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first call to the
resulting generator yields the `first` value, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `first` is a string). If the value yielded
would be the same as or beyond the given `limit`, the generator becomes
voided.

As a special case, if `increment` is `0`, the resulting generator just
yields `first` and then becomes voided.

#### `generatorForInclusiveRange(first, increment, limit) <> generator`

End-inclusive range generator for int or single-character strings.
Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first call to the
resulting generator yields the `first` value, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `first` is a string). If the value yielded
would be beyond the given `limit`, the generator becomes voided.

As a special case, if `increment` is `0`, the resulting generator just
yields `first` and then becomes voided.

#### `generatorForOpenRange(first, increment) <> generator`

Open (never voided) range generator for int or single-character strings.
Takes an initial value, which must either be an int or a single-character
string, and an int (always an int) increment. The first call to the
resulting generator yields the `first` value, and each subsequent call
yields the previous value plus the given increment (converted to a
single-character string if `first` is a string).

As a special case, if `increment` is `0`, the resulting generator just
yields `first` and then becomes voided.

#### `generatorFromValue(value) <> generator`

Collection iteration generator constructor. This takes an arbitrary
collection value &mdash; a list, a map, or a string &mdash; and returns a
generator which successively yields elements of that collection, per the
specification for generators.

If passed a function, `generatorFromValue` returns the function directly,
on the assumption that it is already a generator, in order to make it easy
to write functions that can take either values or generators, coercing the
former to generators and transparently not-transforming generators passed
directly. That is, a function that wants to take values-or-generators can
safely call `generatorFromValue(valueOrGenerator)` without any up-front
type checking.

#### `listFromGenerator(value) <> list`

Takes an arbitrary generator or value (which is coerced to a generator
as if by calling `generatorFromValue` on it), and collects all of its
generated results, in generated order, into a list, returning that list.

This function could be defined as something like:

```
fn listFromGenerator(value) {
    <> collect for(v in value) { <> v }
}
```

#### `optGenerator(value) <> generator`

"Optional" generator constructor. This takes an arbitrary generator or
value (which is coerced to a generator as if by calling `generatorFromValue`
on it), returning a new generator that always yields lists and never
becomes voided. As long as the underlying generator yields a value, the
returned generator yields a single-element list of that value. Once the
underlying generator is voided, the returned generator yields the empty
list, and will continue doing so ad infinitum.

#### `paraGeneratorFromValues(values*) <> generator`

Parallel generator combination constructor. This takes an arbitrary number of
values or generators, and returns a generator that yields lists.
Non-generator arguments are "coerced" into generators as if by calling
`generatorFromValue` on them.

Each yielded list consists of values yielded from the individual generators,
in passed order. The generator becomes voided when *any* of the individual
generators is voided.

#### `reduceGenerator(reduceFunction, base) <> generator`

Reducing generator constructor. This takes a reducer function and
an arbitrary `base` (initial) value, and returns a generator which
acts on its own previously-yielded values.

Each time the outer (result) generator is called, it calls the
`reduceFunction`, passing it a single argument. The first time it is
called, the argument is `base`. After that, it is the most-recently
yielded non-void value from the call to `reduceFunction`. The generator
yields whatever non-void value is yielded from the latest call to
`reduceFunction`. If `reduceFunction` should yield void, then the
generator yields the previous value (possibly the `base`).

The outer generator never becomes voided.

#### `seqGeneratorFromValues(values*) <> generator`

Sequential generator combination constructor. This takes an arbitrary number
of values or generators, and returns a generator that yields from each of
the generators in argument order. Non-generator arguments are "coerced"
into generators as if by calling `generatorFromValue` on them.

As each generator becomes voided, the next one (in argument order) is called
upon to generate further elements. The generator becomes voided after the
final argument is voided.

#### `tokenGeneratorFromValue(value) <> generator`

Filter generator that produces a sequence of valueless (type-only)
tokens from whatever the underlying generator produces. This is,
in particular, the generator used by default to produce single-character
tokens when using the library function `pegApply` to perform
string tokenization.
