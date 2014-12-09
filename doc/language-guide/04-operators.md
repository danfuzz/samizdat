Samizdat Language Guide
=======================

Expression Operators
--------------------

Samizdat provides many of the same operators found throughout the C family,
as well as a few new ones. In Samizdat, almost all infix operators are
left-associative, that is `x op y op z` is equivalent to `(x op y) op z`.
The few exceptions are noted explicitly, below.

The following list is ordered from highest (tightest binding) to lowest
(loosest binding) precedence.


### Postfix and Tight Infix Operators (Precedence 9; highest / tightest)

Postfix operators have the highest precedence in the language, binding
more tightly than any other operators, including prefix operators.

In addition, there are a few infix operators which bind tighter than
prefix operators, included here.

#### Invoke method &mdash; `targetExpr.name(arg, arg, ...) { block } { block } ...`

To invoke a method on an expression, follow it with a dot, a name, and
a list of comma-separated arguments to apply it to, between parentheses.
The evaluation order is the target expression, the name, and then the
arguments in order. Once all of these are evaluated, the named method on
the target is looked up and called. The result of the expression is the same
as the result of the method call. If the method call returns void, then the
expression's result is also void.

`name` is typically a literal identifier, which is taken to be a literal
symbol representing the name of the method. Alternatively, `name` can
be an arbitrary parenthesized expression, in which case it must evaluate
to a symbol. Not evaluating to a symbol is a fatal error (terminating the
runtime).

If the target, name, or any argument evaluates to void, then this causes an
immmediate error (terminating the runtime).

In order to make it convenient to define control-structure-like methods
and functions, any number of block closure literals can follow the closing
parenthesis. All such closures are taken to be additional *initial* arguments
to the method call. For example, `x.foo(bar) { baz }` means the same thing
as `x.foo({ baz }, bar)`. This ordering is done based on the principal that
for functions which take a mix of function and non-function arguments, the
function arguments are more likely to be fixed in quantity (e.g., always just
one) and fixed in meaning.

If there is at least one closure argument but no non-closure arguemnts, then
the parentheses are optional. However, if there are no arguments at all,
then parentheses are required, in order to unambiguously indicate that
method invocation is to be performed. That is, `x.foo()` and `x.foo { block }`
are both method calls, but plain `x.foo` is a getter/setter reference.

As with list literal syntax, an argument whose class is a generator or
a collection (e.g., a list, map, or string) can have its contents
"interpolated" into a function call argument list by following the argument
with a star. For example, `x.foo(bar, [1, 2]*)` means the same thing as
`x.foo(bar, 1, 2)`. This works for all argument expressions (not just
literals), so long as the expression evaluates to an appropriate value.

#### Invoke function &mdash; `expression(arg, arg, ...) { block } { block } ...`

This is a variant of the method invocation syntax and is exactly equivalent
to invoking the `call` method on the given target `expression`. This includes
argument interpolation with postfix `*`.

As with method invocation, parentheses are optional if there is at least
one closure argument but no non-closure arguments; and parentheses are
required if there are no arguments at all. That is, `foo()` and
`foo { block }` are both function calls, but plain `foo` is a variable
reference.

```
foo()             is equivalent to  foo.call()
foo(10)           is equivalent to  foo.call(10)
foo { code }      is equivalent to  foo.call { code }
(foo::bar(baz*))  is equivalent to  (foo::bar).call(baz*)
```

#### Invoke getter/setter method &mdash; `targetExpr.identifier` `targetExpr.identifier := expression`

Dot infix syntax as an expression (without explicit method invocation) is
used to call a "getter" or "setter" method. As a getter invocation, the method
is passed `targetExpr` as its sole argument. As a setter, the method is passed
`targetExpr` and `expression` (in that order).

Getter and setter method names are constructed from the indicated `identifier`
by prepending `"get_"` or `"set_"` respectively.

For example, the two lines in each pair here are equivalent to each other:

```
someCall(onSomething).zorch
someCall(onSomething).get_zorch()

blort.spaz := foo + 10
blort.set_spaz(foo + 10)
```

**Note:** The `:=` operator in the setter syntax is the assignment operator,
described below.

#### Access collection with literal symbol or string key &mdash; `expression::name`

A literal symbol key can be looked up in a collection by naming the
collection and following it with a double-colon (`::`) and an identifier
(either the simple or backslash-quoted form). A literal string key can be
similarly looked up by using a quoted string instead of an identifier.

These are equivalent to calling `get` on the collection, passing it the
literal key as the argument. That is, `foo::bar` and `foo::\"bar"` are both
equivalent to `foo.get(@bar)`; and `foo::"bar"` is equivalent to
`foo.get("bar")`.

#### Access collection &mdash; `expression[index]` `expression[^index]`

To index into a collection (e.g., a list, map, or string) or collection-like
value, by integer index (e.g., for a list or string) or arbitrary key value
(e.g., for a map), place the index value inside square brackets after an
expression evaluating to a collection to index into. The result of the
expression is the value "located" at the indicated index within the
collection, or void if the indicated element does not exist (e.g., index out
of range or key not in map).

As with function calls, a star after an index expression indicates
interpolation. However, it is only ever valid to include one index.

To indicate indexing by int index from the end of a sequence, prefix
the index with a caret (`^`). This is only valid for sequences, not
collections in general.

The expression to index into and the index must both be non-void.

A collection access expression is identical to a function call of either
`get` or `.reverseNth` with the value to be accessed as the argument. That is,
`x[y]` means the same thing as `x.get(y)`, and `x[^y]` means the same thing as
`x.reverseNth(y)`.

#### Sequence slice &mdash; `expression[start..end]` `expression[start..!afterEnd]`

To extract a "slice" of a sequence, indicate the start and end positions
of the slice inside square brackets and separated by `..` or `..!`, after
naming the sequence to slice. As with the related range syntax, `..`
indicates that the end is inclusive, and `..!` indicates that the end is
exclusive.

Either or both of the start and end can be omitted. Omitting the start
is equivalent to specifying it as `0`. Omitting the end is equivalent to
specifying it as `#expression - 1` (that is, one less than the size of
the sequence).

As with single-value indexing, either/both of the start and end expressions
can be prefixed with `^` to indicate indexing by position from the end
of the sequence.

A slice expression using `..` is equivalent to calling
`expression.sliceInclusive(start, end)` (with `end` possibly omitted).

A slice expression using `..!` is equivalent to calling
`expression.sliceExclusive(start, end)` (with `end` possibly omitted).

**Note:** `expression[..!]` per se is a convenient shorthand for getting a
sequence of all but the last element of `expression`. `expression[..]` per se
is somewhat pointless (asserts that `expression` is a sequence and
returns it) but still allowed.

#### Maybe value &mdash; `expression?`

In case an expression might legitimately result in a value or void, and that
possibly-void result is to be further used, such an expression can be
converted into a list by appending a question mark to it. If the inner
expression results in a value `v`, then the outer expression results in a
single-element list of the result `[v]`. If the inner expression results in
void, the outer expression results in `[]` (the empty list).

As a special case of this operator, when applied directly to a variable name
(with no intervening parentheses), this operator denotes the box which holds
the so-named variable, similar to the `&name` form in C. Normally, the box
representation of variables is kept "behind the scenes," but sometimes it is
useful to access a variable's box directly. This form is most useful when
using protocols that explicitly want to take boxes, such as notably the
generator protocol. For example:

```
def result;
if (generator.nextValue(result?)) {
    [...]
}
```

#### Interpolate generator or value &mdash; `expression*`

The star postfix operator is, in a way, the inverse of the maybe value
operator (`expression?`, above). It takes an expression whose value must be a
generator or collection value (list, map, or string) of either zero or one
element, and results in the sole value or void (the latter given a voided
generator or an empty collection).

It is valid to use this operator to possibly-yield a value (that is, yield
either a value or void) from a function. Inside an expression, a void
interpolation is generally invalid (resulting in terminal error).

**Note:** A postfix star expression as an element of a function call
argument list, as a list literal element, or as a map or symbol table literal
key has a slightly different (but related) meaning. See the documentation on
those constructs for more details.

#### Convert logical truth value to boolean &mdash; `expression??`

The unary double-question postfix operator takes a logical truth value
&mdash; where any value represents true, and void represents false &mdash;
resulting in the boolean equivalent. That is, if the inner `expression`
yields a value (not void), the outer expression yields `true`. And if the
inner `expression` yields void, the outer expression yields `false`.

This operator is useful in that it allows a logic expression to consistently
bottom out in a bona fide value, for storage in a variable or as part of a
data structure.

#### Convert boolean to logical truth value &mdash; `expression**`

The unary double-star postfix operator takes a boolean truth value
&mdash; `true` or `false` &mdash; yielding an equivalent logical truth
value. That is, if the inner `expression` is `true`, the outer expression
also yields `true`. If the inner `expression` yields `false`, the outer
expression yields void. Any other inner expression is an error (terminating
the runtime).

It is valid to use this operator to possibly-yield a value (that is, yield
either a value or void) from a function.

This operator, which is essentially the inverse of the double-question
postfix operator, is useful in order to perform conditional operations
on a boolean variable or data structure element. For example, it can be
used to take a flag value and incorporate it into a logical
expression (such as might be the expression checked in an `if` statement).


### Prefix Operators (precedence 8)

Prefix operators are higher in precedence than infix operators, but lower
in precedence than postfix operators.

#### Numeric negative &mdash; `-expression`

Placing a minus sign in front of an expression asserts that the inner
expression results in a number, and results in the negative value of the
inner expression's result.

#### Collection size &mdash; `#expression`

Placing a hash mark (also known as a number sign or pound sign) in front
of an expression is equivalent to calling `get_size` on the evaluated result
of that expression.

#### Sequence reverse &mdash; `^expression`

Placing a caret in front of an expression is equivalent to calling
`expr.reverse()` on the evaluated result of that expression.

#### Logical not &mdash; `!expression`

Placing a bang (exclamation point) in front of an expression reverses
the logical sense of the expression. If the inner expression evaluates
to any value (not void), the outer expression's result is void. If
the inner expression evaluates to void, then the outer expression's
result is the value `true`.

**Note:** Samizdat logic expressions are based on the idea of void as
false and any value as true.

#### Bitwise complement &mdash; `!!!expression`

Placing a triple-bang in front of an expression asserts that the inner
expression results in an int or boolean, and results in the bitwise complement
of the inner expression's result.


### Multiplicative Infix Operators (precedence 7)

#### Multiplication &mdash; `expression * expression`

This asserts that both expressions result in numbers, and results in the
product of the two numbers.

#### Truncated Division &mdash; `expression / expression`

This asserts that both expressions result in numbers, and results in the
quotient of the two numbers (first over second), using truncated
division.

#### Truncated Modulo &mdash; `expression % expression`

This asserts that both expressions result in numbers, and results in the
remainder after division of the two numbers (first over second), using
a truncated division definition.

#### Euclidean Division &mdash; `expression // expression`

This asserts that both expressions result in numbers, and results in the
quotient of the two numbers (first over second), using Euclidean
division.

#### Euclidean Modulo &mdash; `expression %% expression`

This asserts that both expressions result in numbers, and results in the
remainder after division of the two numbers (first over second), using
a Euclidean division definition.

#### Bitwise shift left &mdash; `expression <<< expression`

This asserts that both expressions result in ints, and results in the
first one shifted left by the number of bits indicated by the second one.
If the second expression results in a negative number, this instead becomes
a right shift.

#### Bitwise shift right &mdash; `expression >>> expression`

This asserts that both expressions result in ints, and results in the
first one shifted right by the number of bits indicated by the second one.
If the second expression results in a negative number, this instead becomes
a left shift.


### Additive Infix Operators (precedence 6)

#### Addition &mdash; `expression + expression`

This asserts that both expressions result in numbers, and results in the
sum of the two numbers.

#### Subtraction &mdash; `expression - expression`

This asserts that both expressions result in numbers, and results in the
difference of the two numbers (first minus second).

#### Bitwise and &mdash; `expression &&& expression`

This asserts that both expressions result in ints, and results in the
bitwise and of the two numbers.

#### Bitwise or &mdash; `expression ||| expression`

This asserts that both expressions result in ints, and results in the
bitwise or of the two numbers.

#### Bitwise xor &mdash; `expression ^^^ expression`

This asserts that both expressions result in ints, and results in the
bitwise xor of the two numbers.


### Comparison Infix Operators (precedence 5)

In general, comparison operators correspond to function calls to standard
comparison functions. The contract of these functions is to return their
first argument &mdash; the left-hand side &mdash; to represent logical
true, and to return void to represent logical false.

Comparisons in Samizdat are chainable: `x < y <= z` is the same as saying
`(x < y) & (y <= z)` with the additional guarantee that `y` is only
evaluated once.

**Note:** A consequence of the abovementioned rules is that a logical true
result from a chained comparison is the second-from-last value. E.g.,
`x < y <= z` will either yield `y` or void.

#### Per-class comparison &mdash; `== != < > <= >=`

These are the per-class comparison operators. Use of these operators
corresponds to calls to the `Cmp` class methods `.perEq()`, `.perNe()`,
`.perLt()`, `.perGt()`, `.perLe()`, and `.perGe()` (with the obvious mapping
of operator to method), which bottom out in calls to the methods `.perEq()`
or `.perOrder()`. See the documentation of `Cmp` for more details.

#### Cross-classr comparison &mdash; `\== \!= \< \> \<= \>=`

These are cross-class comparison operators. Use of these operators
corresponds to calls to the `Cmp` class methods `.eq()`, `.ne()`, `.lt()`,
`.gt()`, `.le()`, and `.ge()` (with the obvious mapping of operator to
method), which bottom out in calls to the methods `.crossEq()`
or `.crossOrder()`. See the documentation of `Cmp` for more details.

**Note:** This can sometimes have surprising results, e.g. when comparing
ints and floating point numbers.


### Value/Void Logical-And Operator (precedence 4) &mdash; `expression & expression`

This is short-circuit logical-and (conjunction). When evaluating this
operator, the first (left-hand) expression is evaluated. If that results
in void, then the entire expression results in void. Otherwise, the second
(right-hand) expression is evaluated, and its result (whether a value or
void) becomes the result of the outer expression.

The value of the left-hand side can be referred to on the right-hand side
by adding a name binding on the left. Do this by enclosing the left-hand
expression in parentheses, and prefixing it with a def-assignment, e.g.
`(def name = expression) & somethingWith(name)`. Expression-internal variable
binding is *only* allowed on the left-hand side of an `&` expression; in
any other expression context it is a syntax error.


### Value/Void Logical-Or Operator (precedence 3) &mdash; `expression | expression`

This is a short-circuit logical-or (disjunction). When evaluating this
operator, the first (left-hand) expression is evaluated. If that results
in a value (but not void), then the entire expression results in that same
value. Otherwise, the second (right-hand) expression is evaluated, and its
result (whether a value or void) becomes the result of the outer expression.

**Note:** The question-mark-colon trinary operator from C (and descendants)
is obviated in Samizdat by this and the logical-and operator.
`x ? y : z` in C can generally be turned into `x & y | z` in Samizdat,
as long as `y` will never evaluate to void. If `y` can legitimately evaluate
to void, then the slightly longer form `(x & y? | z?)*` is an equivalent that
ensures that a void `y` won't improperly cause `z` to be evaluated.


### Type calculus (precedence 2)

These are operators that are concerned with the types / classes of
values.

Unlike most infix expressions, type calculus expressions are
*non-associative*. This means that it is not syntactically correct to
chain these expressions together without intervening parentheses.

#### Type cast &mdash; `expression as expression`

This is equivalent to calling the class method
`Class.typeCast(expression, expression)`, with the arguments in the opposite
order. For example, `"foo" as Symbol` is equivalent to
`Class.typeCast(Symbol, "foo")`.

#### Type check &mdash; `expression isa expression`

This is equivalent to calling the class method
`Class.typeAccepts(expression, expression)`, with the arguments in the same
order. For example, `1 isa Int` is equivalent to `Class.typeIs(Int, 1)`.


### Assignment (precedence 1; lowest / loosest) &mdash; `lvalue := expression`

The `:=` operator indicates assignment. `lvalue` is an expression that must
be a valid value reference (assignment target). Unlike most infix expressions,
assignment expressions are *right-associative*. That is, `a := b := c` is
equivalent to `a := (b := c)`.

In general, the `lvalue` is evaluated before the `expression`, and the
result of the overall expression is the same as the evaluated result
of `expression`. `expression` must not evaluate to void.

Any number of `lvalue :=` left-hand sides can be included (e.g.,
`a := b := expr`). Unlike all other infix operator forms, the assignment
operator is right-associative.

Beyond that, the specific meaning of an assignment expression depends on
what sort of reference `lvalue` is; see those for more details.

Lvalues include:

* Simple named variable references, e.g. `blort`. In this case, the variable
  in question must be settable (e.g. a mutable variable).

* An interpolation reference, that is, an arbitrary expression suffixed
  with `*`. In this case, the expression must evaluate to a value which
  supports the `Box` protocol.

* A collection index reference, that is, the form `value[key]`. In
  this case, `value` must evaluate to a mutable collection, and assignment is
  equivalent to calling `value.set(key, expr)`. **Note:** The core collection
  classes are all immutable and do not define the `.set()` method.

* A getter/setter expression, that is, an arbitrary expression followed
  by `.memberName` and *without* method application parentheses after that.
  In this case, assignment is equivalent to calling `.set_memberName()` with
  one argument.

**Note:** If an otherwise-valid lvalue expression is surrounded with
parentheses, then it loses its "lvalue-ness."
