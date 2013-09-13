Samizdat Language Guide
=======================

Comprehensions and Filters
--------------------------

*Samizdat* provides a concise syntax for building filters that process
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
(x in 0..+ <> x * 2)
# => 0, 2, 4, 6, ...

# Infinite generator of even whole numbers, by filtering out odd ones.
(x in 0..+ <> ((x % 2) == 0) & x)
# => 0, 2, 4, 6, ...

# List of just the strings.
[v in ["b", 10, "l", @foo, "o", "r", ["wow"], "t"] <> isString(v)]
# => ["b", "l", "o", "r", "t"]

# List of sum of corresponding pairs.
[n1 in 1..9, n2 in 100..100..+ <> n1 + n2]
# => [101, 202, 303, 404, 505, 606, 707, 808, 909]

# Sum of three ranges.
[a in 0..4, b in 50..10..90, c in 300..100..700 =>
    def sum = a + b + c;
    <> sum
]
# => [350, 461, 572, 683, 794]
```
