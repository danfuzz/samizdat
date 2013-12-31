Samizdat Language Guide
=======================

Generators and Filters
----------------------

Samizdat provides a number of facilities around the concept of generators.
A generator is a value or object which represents a (possibly infinite)
sequence of other values. A similar concept is sometimes referred to as
an "iterator" in other languages.

At the lowest level, a generator is any value which implements the
`proto::Generator` protocol. This protocol specifies three methods, two
of which have default implementations. The one method that must be provided
is `nextValue()`, which is called to both produce a value and gain access
to the generator to use to get at the next value (and so on).

Samizdat both provides a number of generator implementations in the core
library and provides convenient syntax for creating them.

### Comprehensions

Samizdat provides a concise syntax for building filters that process
generators and collections. The result of such an expression is either
a generator or a list, depending on delimiter. As used here,
the term "comprehension" applies to either variant.

The complete form for a comprehension consists of an opening delimiter,
one or more comma-separated generator expressions, a right-arrow (`->`)
to introduce the body, one or more body statements, and a closing delimeter.

With the delimiters `(...)`, the result is a generator. With the delimiters
`[...]`, the result is a list.

The generator expressions are similar to those used in `for` expressions,
consisting of an optional name (either an identifier, or `.` to indicate
an anonymous item), the keyword `in`, and an expression which must
evaluate to either a "generatable value" (such as a list or map) or to
a generator.

The body is evaluated on sets of corresponding elements from the generator
expressions. If the body yields a (non-void) value, then that value gets
included in the resulting comprehension yield (either as a generated value
or included in the result list). If the filtering expression yields void,
then there is no corresponding comprehension yield for that particular set
of values.

If the body *only* consists of a single local-yield statement (`<> ...`),
then the `->` may be omitted.

For example:

```
# Infinite generator of even whole numbers, by multiplying by 2.
(x in (0..) <> x * 2)
# => 0, 2, 4, 6, ...

# Infinite generator of even whole numbers, by filtering out odd ones.
(x in (0..) <> ((x % 2) == 0) & x)
# => 0, 2, 4, 6, ...

# List of just the strings.
[v in ["b", 10, "l", @foo, "o", "r", ["wow"], "t"] <> isString(v)]
# => ["b", "l", "o", "r", "t"]

# List of sum of corresponding pairs.
[n1 in (1..9), n2 in (100.., +100) <> n1 + n2]
# => [101, 202, 303, 404, 505, 606, 707, 808, 909]

# Sum of three ranges.
[a in (0..4), b in (50..90, +10), c in (300..700, +100) =>
    def sum = a + b + c;
    <> sum
]
# => [350, 461, 572, 683, 794]
```

### Ranges

Range expressions are used to build generators of ranges of values
of ints or characters (the latter in the form of single-element strings).

Closed ranges all consist of at least two sub-expressions, namely a `first`
(initial) value and a `limit` value. Open-ended ranges need only have a
`first` value.

Ranges can also optionally be specified with an `increment` expression.
If not specified, the `increment` defaults to `1`. Even if the `first` and
`limit` are strings, the `increment` if specified must always be an int.

Ranges all bottom out in their evaluation to calls to one of the
functions `Range::makeExclusiveRange`, `Range::makeInclusiveRange`,
or `Range::makeOpenRange`. Refer to the documentation on those functions
for how to interpret the various combinations.

Syntactically, a range expression consists of one or more expressions
in parentheses, with the expressions internally delimited with `..`, `..!`,
and/or `,`, as follows:

* `(first..limit)` &mdash; Inclusive range with increment 1. This is
  equivalent to `Range::makeInclusiveRange(first, limit, 1)`.

* `(first..limit, increment)` &mdash; Inclusive range with arbitrary
  increment. This is equivalent to
  `Range::makeInclusiveRange(first, limit, increment)`.

* `(first..!limit)` &mdash; Exclusive range with increment 1. This is
  equivalent to `Range::makeExclusiveRange(first, limit, 1)`.

* `(first..!limit, increment)` &mdash; Exclusive range with arbitrary
  increment. This is equivalent to
  `Range::makeExclusiveRange(first, limit, increment)`.

* `(first..)` &mdash; Open range with increment 1. This is equivalent to
  `Range::makeOpenRange(first)`.

* `(first.., increment)` &mdash; Open range with arbitrary increment. This
  is equivalent to `Range::makeOpenRange(first, increment)`.

**Note:** Though `+` is not a real unary prefix operator, it is allowed
(but not required) to prefix increment expressions, as a way to mnemonically
emphasize those expressions' role.
