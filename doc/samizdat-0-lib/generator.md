Samizdat Layer 0: Core Library
==============================

Generators
----------


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `filterGenerator(value, filterFunction) <> generator`

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

#### `generatorFromValues(values*) <> generator`

Generator combination constructor. This takes an arbitrary number of
values or generators, and returns a generator that yields lists.
Non-generator arguments are "coerced" into generators as if by calling
`generatorFromValue` on them.

Each yielded list consists of values yielded from the individual generators,
in passed order. The generator becomes voided when *any* of the individual
generators is voided.

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

#### `tokenGeneratorFromValue(value) <> generator`

Filter generator that produces a sequence of valueless (type-only)
tokens from whatever the underlying generator produces. This is,
in particular, the generator used by default to produce single-character
tokens when using the library function `pegApply` to perform
string tokenization.
