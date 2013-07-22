Samizdat Language Guide
=======================

Comprehensions and Filters
--------------------------

*Samizdat* provides a concise syntax for building filters that process
generators and collections. The result of such an expression is either
a generator or a list, depending on delimiter or keyword. As used here,
the term "comprehension" applies to either variant.

### Concise comprehensions &mdash; `(... <> ...)` and `[... <> ...]`

A concise comprehension expression that is surrounded by `(...)` produces a
generator, and one that is surrounded by `[...]` produces an immediate
list result.

Inside the delimiters, a concise comprehension consists of one or more
comma-separated generator expressions, followed by a yield diamond (`<>`),
and finally followed by a filtering expression.

The generator expressions are similar to those used in `for` expressions,
consisting of an optional name (either an identifier, or `.` to indicate
an anonymous item), the keyword `in`, and an expression which must
evaluate to either a "generatable value" (such as a list or map) or to
a generator.

The filtering expression is applied to sets of corresponding elements
from the generator expressions. If the filtering expression yields a
(non-void) value, then that value gets included in the resulting
comprehension yield (either as a generated value or included in the
result list). If the filtering expression yields void, then there is
no corresponding comprehension yield for that particular set of values.

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
```

These expressions, when translated into low-level executable form,
use the library functions `filterGenerator` to produce generators
and `collectFilter` to produce list results.

### Extended comprehensions &mdash; `generate (...) {...}` and `collect ... {...}`

Extended comprehension syntax allows for clean expression of larger
filtering expressions. The keyword `generate` introduces a
generator-producing comprehension, and the keyword `collect` introduces
a list-producing comprehension. Other than the keyword, the two forms
are identical.

After the keyword, a parenthesized comma-separated list of generator
name bindings needs to be listed. These name bindings take the same form
as in the concise version of the expression. After the name bindings,
a curly-brace delimited block indicates the filter code to run.

In general, `generate (genExprs) { <> filterBlock }` is equivalent to
`(genExprs <> filterBlock)`, and `collect (genExprs) { <> filterBlock }`
is equivalent to `[genExprs <> filterBlock]`. The extended versions,
however, don't restrict the block body to being just a one-line yield.

For example:

```
# Sum of three ranges.
collect (a in 0..4, b in 50..10..90, c in 300..100..700)
    {
        def sum = a + b + c;
        <> sum
    }
# => [350, 461, 572, 683, 794]
```
