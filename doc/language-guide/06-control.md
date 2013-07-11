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
clause. To do that, add a `name = ` assignment prefix to the test expression.

For example:

```
opName (varName = someCalculation()) {
    ... varName ...
}
```

#### Nonlocal yield

Most control constructs (all but `if`) implicitly define a "break
context" that goes with a special `break` keyword. This keyword can be used in
yield context (that is, as the last statement of a block).
`break expression` causes the control construct to immediately yield
the evaluated value of the given expression. `break` with no expression
causes the control construct to immediately yield void.

For example:

```
opName (expression) {
    ...
    break resultExpression
    ...
}
```

In addition, all control constructs (including `if`) allow explicit
yield definition. To do so, place the yield name between angle brackets
immediately after the control operator name. With this done, the defined
yield name can be used in nonlocal exit statements to exit from the
control construct.

For example:

```
opName <out> (expression) {
    ...
    <out> resultExpression
    ...
}
```

This latter form is meant to make it easier to break out of
a nested context in which there is an inner break context which
is to be skipped over.

#### Early loop continuation

All the looping control constructs define a "continue context" that goes
with a special `continue` keyword. This keyword can be used in
yield context (that is, as the last statement of a block). Calling `continue`
causes the loop to restart at its top. It is never valid to pass an
expression to `continue`.

For example:

```
loopOp (expression) {
    ...
    continue
    ...
}
```

In addition, all the looping control constructs allow explicit
continuation yield definition. To do so, place the yield name in the
usual block definition position. With this done, the defined
yield name can be used in nonlocal exit statements to continue the loop.

For example:

```
opName (expression) { <next> ::
    ...
    <next>
    ...
}
```

This latter form is meant to make it easier to continue a
loop in which other loops are nested.

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

`if` expressions support both explicit yield definition and
test expression name binding.

`if` expressions do *not* define a break or continue context.

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
block, the same syntax is used to represent this as is used for
multiple binding of keys to a single value in map literals. This
includes list interpolation, ranges, and parenthesized lists of
expressions. In addition, the same identifier-as-string shorthands
apply:

```
switch (expression) {
    (testExpression1(), testExpression2(), testExpression3()): {
        block123
    }
    4..6: {
        block456
    }
    ["fizmo", "igram", secretSpellName()]*: {
        spellBlock
    }
    name: {
        nameBlock
    }
    (north, south, east, west): {
        directionBlock
    }
}
```

`switch` expressions support both explicit yield definition and
test expression name binding. `switch` expressions define a break
context.

`switch` expressions do *not* define a continue context.

**Note:** Unlike other languages in the C tradition, *Samizdat* does
not allow consequent blocks to fall through. So, a block-final
`break` isn't ever necessary. If there is partial code in common
between cases, the thing to do is factor it out into a separate
function.

### Unconditional loop &mdash; `do`

To loop unconditionally, simply place a block after the `do` keyword:

```
do {
    block
}
```

The block can have any number of statements, but it must not declare
any arguments. It *may* have a yield definition, which serves as
an explicit continuation name (as described above).

This form is evaluated by evaluating the given block, ignoring any
result it may yield, and then evaluating the block again, ad infinitum.

Unconditional `do` expressions support explicit yield definition
and define both break and continue contexts.

Unconditional `do` expressions, not having a test, naturally do
not support test expression name binding.

**Note:**  The only way an unconditional `do` expression ever yields
a value is by explicit `break` or nonlocal exit.


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

`while` expressions support explicit yield definition and test expression
name binding, and define both break and continue contexts.

If the test is better done after at least one iteration, the
`do...while` form can be used:

```
do {
    block
} while (expression)
```

`do...while` expressions support explicit yield definition,
and define both break and continue contexts.

`do...while` expressions do *not* support test expression name binding,
since the first iteration can't possibly have a binding for the
test expression, and the language doesn't allow variables to be bound
to void.
