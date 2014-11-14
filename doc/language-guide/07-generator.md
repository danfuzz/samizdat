Samizdat Language Guide
=======================

Generators and Filters
----------------------

Samizdat provides a number of facilities around the concept of generators.
A generator is a value or object which represents a (possibly infinite)
sequence of other values. A similar concept is sometimes referred to as
an "iterator" in other languages.

At the lowest level, a generator is any value which implements the
`Generator` protocol. This protocol specifies four methods, three of which
have default implementations. The one method that must be provided is
`.nextValue()`, which is called to both produce a value and gain access
to the generator to use to get at the next value (and so on).

TODO: The "default" implementations have to be hooked up in a fairly manual
way right now. It should actually be via some kind of inheritance mechanism.

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

There are a number of different generator expressions, as described in
the subsections immediately below.

The body is evaluated on sets of corresponding elements from the generator
expressions. If the body yields a (non-void) value, then that value gets
included in the resulting comprehension yield (either as a generated value
or included in the result list). If the body yields void for an iteration,
then there is no corresponding comprehension yield for that particular set
of generated values.

#### Generator collection iteration

The most common type of generator expression is one which directly
iterates over the contents of a generator.

These consist of an optional name (either an identifier, or `.` to indicate
an anonymous item), the keyword `in`, and an expression which must evaluate
to a generator, including possibly a "generatable value" such as a list or
map.

For example:

```
## Infinite generator of even whole numbers, by multiplying by 2.
( x in (0..) -> x * 2 )
## => 0, 2, 4, 6, ...

## Infinite generator of even whole numbers, by filtering out odd ones.
( x in (0..) -> ((x % 2) == 0) & x )
## => 0, 2, 4, 6, ...

## List of just the strings.
[ v in ["b", 10, "l", @foo, "o", "r", ["wow"], "t"] -> String.accepts(v) ]
## => ["b", "l", "o", "r", "t"]

## List of sum of corresponding pairs.
[ n1 in (1..9), n2 in (100.. by 100) -> n1 + n2 ]
## => [101, 202, 303, 404, 505, 606, 707, 808, 909]

## Sum of three ranges.
[ a in (0..4), b in (50..90 by 10), c in (300..700 by 100) ->
    def sum = a + b + c;
    sum
]
## => [350, 461, 572, 683, 794]
```

#### Simple repetition counts

If a comprehension is just to have a fixed number of iterations (fixed at
evaluation time), then that can be expressed with the generator expression
`for expression`, where `expression` evaluates to an integer value.
This indicates that the comprehension is only to be run that many times.

This is typically combined with other generator forms to place an upper limit
on the number of values yielded by a comprehension.

For example:

```
[ for 5 -> "Yay!" ]
## => ["Yay!", "Yay!", "Yay!", "Yay!", "Yay!"]

[ x in (0..), for 7 -> x ]
## => [0, 1, 2, 3, 4, 5, 6]
```


### Ranges

Range expressions are used to build generators or lists of ranges of values.
The ranges can consist of either ints or characters, the latter in the form
of single-element strings.

Closed ranges all consist of at least two sub-expressions, namely a `first`
(initial) value and a `limit` value. Open-ended ranges need only have a
`first` value.

Ranges can also optionally be specified with an `increment` expression.
If not specified, the `increment` defaults to `1`. Even if the `first` and
`limit` are strings, the `increment` if specified must always be an int.

Ranges all bottom out in their evaluation to calls to one of the class methods
on `$Range::ClosedRange` or `$Range::OpenRange`. Refer to the documentation on
those classes for how to interpret the various combinations.

Syntactically, a range expression consists of one or more expressions
in either parentheses `(...)` or square brackets `[...]`, with the
expressions internally delimited with `..`, `..!`, and optionally `by`.
As with the comprehension syntax, parentheses are used to indicate a range
generator, and square brackets are used to indicate a range list.

Details for the generator variants are as follows:

* `(first..limit)` &mdash; Inclusive range with increment 1. This is
  equivalent to `$Range::ClosedRange.newInclusive(first, limit)`.

* `(first..limit by increment)` &mdash; Inclusive range with arbitrary
  increment. This is equivalent to
  `$Range::ClosedRange.newInclusive(first, limit, increment)`.

* `(first..!limit)` &mdash; Exclusive range with increment 1. This is
  equivalent to `$Range::ClosedRange.newExclusive(first, limit)`.

* `(first..!limit by increment)` &mdash; Exclusive range with arbitrary
  increment. This is equivalent to
  `$Range::ClosedRange.newExclusive(first, limit, increment)`.

* `(first..)` &mdash; Open range with increment 1. This is equivalent to
  `$Range::OpenRange.new(first)`.

* `(first.. by increment)` &mdash; Open range with arbitrary increment. This
  is equivalent to `$Range::OpenRange.new(first, increment)`.

All of the *closed* range variants are valid using the square bracket list
form, but no open range variants are allowed to be bracketed, as Samizdat
does not support unbounded-size lists. Semantically, the list forms are
equivalent to `[(first..limit by increment)*]` (and so on).
