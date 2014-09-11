Samizdat Language Guide
=======================

Control Constructs
------------------

Samizdat provides a number of conditional and other control constructs. The
keywords and forms used for them are taken straight from the C tradition.

Unlike many C tradition languages, though, all of these forms are expressions
in the language, meaning that they can combine recursively with other
expressions. While not always necessary, when combining these forms into
complex expressions, it is often a good idea to place them in parentheses
to avoid ambiguity.

### Common features

There are a few features that are common across nearly all control
constructs (with exceptions as noted).

#### Parenthesized test expression

These constructs all use an explicitly parenthesized test expression.

For example:

```
opName (x == 10) ...
```

#### Test expression name binding

When the result of a test expression is of use &mdash; that is, when it is
more than just a simple logical expression &mdash; its result can be
bound to a variable which is then available in the associated consequent
clause. To do that, add a `def name =` define-assignment prefix to the test
expression.

For example:

```
opName (def varName = someCalculation()) {
    ... varName ...
}
```

#### Multiple test expressions

Sometimes it is useful to perform multiple tests in a single control
construct, particularly when the test results are being bound. In
such cases, multple test expressions can be included within parentheses,
separated by commas.

The multiple expressions are treated as a conjunction and evaluated
in order. *All* of the expressions have to evaluate to logical-true
(that is yield a value, and not yield void) in order for the test to
pass.

For example:

```
opName (def varName1 = someExpression1, def varName2 = someExpression2) {
    ... varName1 ... varName2 ...
}
```

#### Nonlocal yield

All control constructs (both loops and non-loops) allow for optional
overall yield definition naming. When invoked, these cause the overall
construct to terminate, yielding the indicated value (or void).

For example:

```
opName /out (expression) {
    ...
    if (...) {
        yield /out resultExpression  # Immediately terminates the construct.
    };
    ...
}
```

All control constructs also allow for optional per-block yield
definition naming. For non-looping constructs, this behaves identically
to an overall yield definition (except that it can help avoid a bit of
duplicated text). For looping constructs, this causes the loop to start
a new iteration.

For example, assuming `opName` is a loop keyword:

```
opName (expression) { /next ->
    ...
    if (...) {
        yield /next  # Immediately starts the next iteration.
    };
    ...
}
```

In addition, loop constructs (but not the other control constructs)
implicitly bind nonlocal exits that pair with the keywords `break`
and `continue`. These can be used in the same contexts as a `yield`
keyword, and they always refer to the closest-enclosing loop.
`break` acts as an overall yield, and `continue` acts as a per-iteration
yield.

As with `yield`, both of these are allowed to take a value, and they can
be followed by `?` and an expression to allow for yielding of value-or-void.

For example:

```
opName (expression) {
    if (...) {
        continue            ## The loop starts a new iteration.
    };
    ...
    break resultExpression  ## The loop terminates, yielding a result.
    ...
}
```

### Conditional expression &mdash; `if...else`

An `if` expression can be used both to perform logical conditional
dispatch, per se, as well as to act on a result from an expression that
might turn out to be void.

The basic form of `if` is:

```
if (expression1) {
    block1
} else if (expression2) {
    block2
} else {
    elseBlock
}
```

There can be any number of `else if` clauses (including none), and the
final `else` clause is optional. Each of the block bodies can contain
any number of statements but must not have any argument declarations.

The expression is evaluated by first evaluating the initial test expression.
If that yields logical-true, then the block `block1` is
evaluated, and the entire expression result is the result of that
block evaluation. Otherwise, the second test expression is evaluated,
similarly calling upon `block2` if the expression yields
logical-true. Likewise, any further `else if` clauses are evaluated.
Finally, if there is an `else` clause and none of the test expressions
yielded logical-true, the block `elseBlock` block is evaluated,
with its result becoming the overall expression result. In case no
consequent block is evaluated, the overall expression result is void.

`if` expressions support explicit yield definition,
test expression name binding, and multiple test expressions.

`if` expressions do *not* define a loop context.

### Value dispatch &mdash; `switch`

To dispatch to a block based on the value of an expression, use
a `switch` expression. It takes the following form:

```
switch (dispatchExpression) {
    testExpression1: {
        block1
    };
    testExpression2: {
        block2
    };
    default: {
        defaultBlock
    };
}
```

There can be any number of test expressions (including none), and
the `default` block is optional. The `default` block can appear anywhere in
the list of test cases. Each of the block bodies can contain any number of
statements but must not have any argument declarations.

This is evaluated by first evaluating the `dispatchExpression`, which must
yield a value. It is an error for it to yield void (terminating the runtime).
Then, the test expressions are evaluated in order (note, they do not
have to be constant expressions). If one of the test expressions results
in a value that is equal to the dispatch expression's value, then its
corresponding block is evaluted, and that becomes the overall expression
result (and no further test expressions are evaluated). If none of
the test expressions matched, then the `defaultBlock` (if any) is evaluated,
and that result becomes the result of the overall expression. In case no
consequent block is evaluated, the overall expression result is void.

If multiple different expressions should match a single consequent
block, the same syntax is used to represent this as is used for
multiple binding of keys to a single value in map literals. This
includes both interpolation and parenthesized lists of expressions.
In addition, the same identifier-as-string shorthands apply:

```
switch (expression) {
    testExpression1(): testExpression2(): testExpression3(): {
        block123
    };
    (4..6)*: {
        block456
    };
    ["fizmo", "igram", secretSpellName()]*: {
        spellBlock
    };
    name: {
        nameBlock
    };
    north: south: east: west: {
        directionBlock
    }
}
```

`switch` expressions support both explicit yield definition and
test expression name binding. `switch` expressions define a break
context.

`if` expressions do *not* define a loop context.

**Note:** Unlike other languages in the C tradition, Samizdat does
not allow consequent blocks to fall through in a `switch`; a block-final
`break` is never necessary. If there is partial code in common
between cases, the thing to do is factor it out into a separate
function. In addition, `break` is not even bound by `switch`, so `break`
appearing in a `switch` refers not to the `switch` but instead to the
closest-enclosing loop that the `switch` appears in.


### Unconditional loop &mdash; `do`

To loop unconditionally, simply place a block after the `do` keyword:

```
do {
    block
}
```

The block can have any number of statements, but it must not declare
any arguments. It *can* have a yield definition, which serves as
an explicit continuation name (as described above).

This form is evaluated by evaluating the given block, ignoring any
result it yields, and then evaluating the block again, ad infinitum.
This construct will only ever yield a value if it is explicitly yielded
(via `break` or a named yield).

Unconditional `do` expressions support explicit yield definition
and define both break and continue contexts.

As they do not have tests at all, unconditional `do` expressions do
not support test expression name binding or multiple test expressions.

**Note:**  The only way an unconditional `do` expression ever yields
a value is by explicit `break` or nonlocal exit.


### Conditional loop &mdash; `while`

Loops can be iterated conditionally with a `while` expression.
These are a straightforward extension of the `do` loop,
described above, replacing `do` with `while (expression)`:

```
while (expression) {
    block
}
```

With this form, `expression` is evaluated at the start of each iteration,
and the block is only evaluated if the expression evaluates to
logical-true. If not, the outer expression terminates, yielding void.
This construct will only ever yield a value if it is explicitly yielded
(via `break` or a named yield).

`while` expressions support explicit yield definition, test expression
name binding, and multple test expressions; and they define both break
and continue contexts.

### Conditional loop with post-condition &mdash; `do ... while`

If a loop test is better done after at least one iteration, the
`do...while` form can be used:

```
do {
    block
} while (expression)
```

With this form, the block is always run at least once. `expression` is
evaluated at the *end* of each iteration, and the block is only re-evaluated
if the expression evaluates to logical-true. As with `while`, this construct
will only ever yield a value if it is explicitly yielded (via `break` or a
named yield).

`do...while` expressions support explicit yield definition,
and define both break and continue contexts.

`do...while` expressions do *not* support test expression name binding,
since the first iteration can't possibly have a binding for the
test expression, and the language doesn't allow variables to be bound
to void. They also do not support multiple test expressions; use `&`
expressions instead if needed.


### Generator iteration loop &mdash; `for`

The `for` construct is used to iterate over the contents of collections
or the values yielded from generators. It has the following form:

```
for (name1 in generatorExpression1, name2 in generatorExpression2) {
    ... name1 ...
    ... name2 ...
}
```

`for` accepts one or more comma-separated generator binding expressions in
parentheses. A generator binding form takes the same syntax as with
standalone generators, and is most typically of the form `name in expression`,
where `name` is an arbitrary variable name, and `expression` is an
expression that must yield a generator (including possibly a collection).
See the section on generators for more info.

Within the block body of a `for`, each of the named variables is bound
to one element from its corresponding generator. The block is called
once per set of elements. The loop terminates when *any* of the generators
becomes voided.

This construct yields the last value (not void) yielded by its inner
block. If the block never yields a value, then this construct yields
void. In addition, if `break` or a named yield is used, then this construct
of course yields the indicated value (or void).

`for` expressions support explicit yield definition, and they define both
break and continue contexts.

`for` expressions do *not* support test expression name binding
(since there are no tests to bind), nor multiple test expressions.
