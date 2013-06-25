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


### Conditional expression &mdash; `if...else`

An `if` expression can be used both to perform logical conditional
dispatch, per se, as well as to act on a result from an expression that
might turn out to be void.

The purely conditional form of `if` is:

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
any number of statements but must not have any argument declarations,
nor may they have a yield definition.

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

It is possible to define a yield name for the expression as a whole.
To do so, place it immediately after the initial `if`, like so:

```
if <out> (expression) {
    ... <out> ...
} else ... {
    ... <out> ...
}
```

Having done so, it is possible to use named yield in any of the
consequent blocks with the so-defined name.

If the value of a test expression is of interest, it can be bound
to a variable by turning the expression into an assignment, like so:

```
if (name = expression) {
    ... name ...
} ...
```

Having done so, it is then possible to refer to the indicated name
in the consequent block associated with that particular test.

TODO: Name binding is not yet implemented.

### Value dispatch &mdash; `switch`

To dispatch to a block based on the value of an expression, use
a `switch` expression. It takes the following form:

```
switch (dispatchExpression) {
    testExpression1: {
        block1
    }
    testExpression2: {
        block2
    }
    default: {
        defaultBlock
    }
    else: {
        elseBlock
    }
}

There can be any number of test expressions (including none), and
both the `default` and `else` blocks are optional. The `default`
and `else` blocks can appear anywhere in the list of test cases.
Each of the block bodies can contain any number of statements but
must not have any argument declarations, nor may they have a yield
definition.

This is evaluated by first evaluating the `dispatchExpression`. If
that yields void, then the `elseBlock` (if any) is evaluated, and
that result becomes the result of the overall expression. If the
`dispatchExpression` yields a value (the usual expected case), then
the test expressions are evaluated in order (note, they do not
have to be constant expressions). If one of the test expressions results
in a value that is equal to the dispatch expression's value, then its
corresponding block is evaluted, and that becomes the overall expression
result (and no further test expressions are evaluated). If none of
the test expressions matched, then the `defaultBlock` (if any)
is evaluated, and that result becomes the result of the overall expression.
In case no consequent block is evaluated, the overall expression result
is void.

If multiple different expressions should match a single consequent
block, they can be listed before the block, separated by commas. In
addition, range and argument interpolation syntax works here:

```
switch (expression) {
    test1, test2, test3: {
        block123
    }
    4..6: {
        block456
    }
    ["fizmo", "igram", secretSpellName()]*: {
        spellBlock
    }
}
```

It is possible to define a yield name for the expression as a whole.
To do so, place it immediately after the `switch`, like so:

```
switch <out> (expression) {
    ... <out> ...
}
```

Having done so, it is possible to use named yield in any of the
consequent blocks with the so-defined name.

If the value of the dispatch expression is of interest, it can be bound
to a variable by turning the expression into an assignment, like so:

```
switch (name = expression) {
    ... name ...
} ...
```

Having done so, it is then possible to refer to the indicated name
in any of the consequent blocks.

**Note:** Unlike other languages in the C tradition, *Samizdat* does
not allow consequent blocks to fall through.

TODO: Neither `else` handling nor name binding has yet been implemented.

### Unconditional loop &mdash; `do`

To loop unconditionally, simply place a block after the `do` keyword:

```
do {
    block
}
```

The block can have any number of statements, but it must not declare
any arguments. It *may* have a yield definition (as described below).

This form is evaluated by evaluating the given block, ignoring any
result it may yield, and then evaluating the block again, ad infinitum.

In order to yield a value from the expression, a `break` statement can
be used. This is a non-local yield and as such must be the final statement
of the block it appears in. `break` takes an optional expression argument.
If present, its value becomes the overall value of the outer `do`
expression. If not present, the other expression's result is void.

```
do {
    ...
    break expression
}
```

It is possible to cut evaluation of an iteration short by using a
`continue` statement. As with `break`, `continue` is a non-local
yield statement. Unlike `break`, `continue` must not be given
an argument.

```
do {
    ...
    continue
}
```

It is possible to define a yield name for the expression as a whole.
To do so, place it immediately after the `do`, like so:

```
do <out> {
    ... <out> ...
}
```

With this variant, the named yield can be used just like `break`. The
purpose of this variant is to allow breaking out of nested loops.

Similarly, it is possible to define a yield name for a single
iteration. To do so, place it in the declaration position of the block,
like so:

```
do { <next> ::
    ... <next> ...
}

With this variant, the named yield van be used just like `continue`.
As with the overall yield name, the purpose of this variant is to
allow continuation within nested loops.

### Conditional loop &mdash; `while` and `do ... while`

Loops may be iterated conditionally with a `while` or a `do...while`
statement. These are a straightforward extension of the `do` loop,
described above.

To execute a conditional test before each iteration, replace `do` with
`while (expression)`:

```
while (expression) {
    block
}
```

With this form, `expression` is evaluated at the start of each iteration,
and the block is only evaluated if the expression evaluates to
logical-true. If not, the outer expression terminates, yielding void.

If the value of the test expression is of interest, it can be bound
to a variable by turning the expression into an assignment, like so:

```
while (name = expression) {
    ... name ...
} ...
```

Having done so, it is then possible to refer to the indicated name
in the block.

If the test is better done after at least one iteration, the
`do...while` form can be used:

```
do {
    block
} while (expression)
```

Name binding is *not* valid with this variant.

`break`, `continue`, and the two forms of yield definition all work
with `while` and `do...while` expressions in the same way as
with unconditional `do` expressions.

TODO: Name binding is not yet implemented.
