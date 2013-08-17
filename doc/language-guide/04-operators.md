Samizdat Language Guide
=======================

Expression Operators
--------------------

Samizdat provides many of the same operators found throughout the C family,
as well as a few new ones. In Samizdat, all infix operators are
left-associative, that is 'x op y op z' is equivalent to `(x op y) op z`.

The following list is ordered from highest (tightest binding) to lowest
(loosest binding) precedence.


### Postfix Operators (Precedence 8, highest / tightest)

Postfix operators have the highest precedence in the language, binding
more tightly than any other operators, including prefix operators.

#### Apply function &mdash; `expression(arg, arg, ...) { block } { block } ...`

To apply an expression as a function, follow it with a list of comma-separated
arguments to apply it to, between parentheses. The arguments are evaluated
in order, and then the function is called with these arguments. The result
of the apply expression is the same as the result of the function call.
If the function call returns void, then the expression's result is also void.

If an argument evaluates to void, then this causes an immmediate error
(terminating the runtime), *unless* that argument is prefixed with the
`&` operator (see `Void contagion`, below). In the latter case, the function
isn't called, and the expression yields void.

In order to make it convenient to define control-structure-like functions,
any number of block closure literals may follow the closing parenthesis. All
such closures are taken to be additional *initial* arguments to the function.
For example, `foo(bar) { baz }` means the same thing as `foo({ baz }, bar)`.
This ordering is done based on the principal that for functions which take
a mix of function and non-function arguments, the function arguments are
more likely to be fixed in quantity (e.g., always just one) and fixed in
meaning.

If there are no arguments (including no closure arguments), parentheses are
required to unambiguously indicate that function application is to be
performed. That is, `foo()` and `foo { block }` are both function calls,
but plain `foo` is just a variable reference.

As with list literal syntax, an argument whose type is a generator or
a collection (list, map, or string) can have its contents "interpolated"
into a function call argument list by following the argument with a star.
For example, `foo(bar, [1, 2]*)` means the same thing as `foo(bar, 1, 2)`.
This works for all argument expressions (not just literals), so long as the
expression evaluates to an appropriate value.

The expression to apply (before the open parenthesis) must be non-void.

#### Message binding lookup &mdash; `expression.nameExpr`

The dot infix syntax is used to find the function used to respond to a named
message. `nameExpr` typically, but not necessarily, evaluates to a string.
As a convenient shorthand, the parentheses and quotes can be omitted if
if `nameExpr` is a string literal matching the identifier token syntax.

The result of a dot infix expression is a function which, when called,
sends the named message to the evaluated (left-hand) expression result.
A typical usage looks like `def method = x.foo; ... method(y, z)`. More
typically, a message is applied more directly using the related application
syntax (immediately below).

#### Message binding lookup and application &mdash; `expression.nameExpr(arg, arg, ...)`

This combines message binding lookup with function application and is
semantically equivalent to calling the result of a message binding lookup
with the indicated arguments. That is, `x.foo(bar, baz)` is equivalent
to `(x.foo)(bar, baz)`.

#### Access collection &mdash; `expression[index, index, ...]`

To index into a collection (e.g., a list, map, or string) or collection-like
value, by integer index (e.g., for a list or string) or arbitrary key value
(e.g., for a map), place the index value inside square brackets after an
expression evaluating to a collection to index into. The result of the
expression is the value "located" at the indicated index within the
collection, or void if the indicated element does not exist (e.g., index out
of range or key not in map).

Within the brackets, one can list as many indices as there are dimensions
in the collection. Note that the core collection types are all
single-dimensioned. As with function calls, a star after an index expression
indicates interpolation.

The expression to index into and all of the indices must be non-void, except
that index expressions can be marked with a `&` prefix to indicate void
contagion (see which for details).

A collection access expression is identical to a message application of
the message `"get"` to the value to be accessed. That is, `x[y]` means
the same thing as `x.get(y)`.

#### Convert Value-or-void to list &mdash; `expression?`

In case an expression might legitimately result in a value or void, that
inner expression can be converted into a list by appending a question mark to
it. If the inner expression results in a value, the outer expression results
in a single-element list of the result. If the inner expression results in
void, the outer expression results in the empty list.

#### Interpolate generator or value &mdash; `expression*`

The star postfix operator is, in a way, the converse of the question mark
postfix operator (above). It takes an expression whose value must be a
generator or collection value (list, map, or string) of either zero or one
element, and results in the sole value or void (the latter given a voided
generator or an empty collection).

It is valid to use this operator to possibly-yield a value (that is, yield
either a value or void) from a function. Inside expressions, a void
interpolation causes the entire expression to evaluate to void.

**Note:** A postfix star expression as an element of a function call
argument list, as a collection index, as a list literal element, or as
a map literal key has a slightly different (but related) meaning. See
the documentation on those constructs for more details.

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

This operator, which is essentially the converse of the double-question
postfix operator, is useful in order to perform conditional operations
on a boolean variable or data structure element.


### Prefix Operators (Precedence 7)

Prefix operators are higher in precedence than infix operators, but lower
in precedence than postfix operators.

#### Void contagion &mdash; `&expression`

Placing an ampersand in front of an expression indicates that the
expression *may* yield void, and if so, causes the void to "spread"
outward to the immediately-surrounding function call, making that function
call itself yield void, instead of actually performing the call.

This operator is *only* valid when placed directly before an argument
to a function call, element of a list construction, or index argument
to a `[]` expression (and not, e.g., embedded any deeper in any
sort of expression).

#### Numeric positive &mdash; `+expression`

Placing a plus sign in front of an expression asserts that the inner
expression results in a number, and results in the same value as the
inner expression.

#### Numeric negative &mdash; `-expression`

Placing a minus sign in front of an expression asserts that the inner
expression results in a number, and results in the negative value of the
inner expression's result.

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
expression results in an int, and results in the bitwise complement of
the inner expression's result.


### Range Operators (Precedence 6)

The range operators are used to build generators of ranges of values
of ints or characters (the latter in the form of single-element strings).

Closed ranges all consist of at least two sub-expressions, namely a `first`
(initial) value and a `limit` value. Open-ended ranges need only have a
`first` value.

Ranges can also optionally be specified with an `increment` expression.
If not specified, the `increment` defaults to `1`. Even if the `first` and
`limit` are strings, the `increment` if specified must always be an int.

Ranges all bottom out in their evaluation to calls to one of the
functions `exclusiveRange`, `inclusiveRange`,
or `openRange`. Refer to the documentation on those functions
for how to interpret the various combinations.

**Note:** Unlike most binary operators, the range operators have no
operator associativity, in that `x..y..z..huhwhat` is a syntax error.

#### Inclusive range with increment 1 &mdash; `first..limit`

This is equivalent to `inclusiveRange(first, 1, limit)`.

#### Inclusive range with arbitrary increment &mdash; `first..increment..limit`

This is equivalent to `inclusiveRange(first, increment, limit)`.

#### Exclusive range with increment 1 &mdash; `first..!limit`

This is equivalent to `exclusiveRange(first, 1, limit)`.

#### Exclusive range with arbitrary increment &mdash; `first..increment..!limit`

This is equivalent to `exclusiveRange(first, increment, limit)`.

#### Open range with increment 1 &mdash; `first..+`

This is equivalent to `openRange(first, 1)`.

#### Open range with arbitrary increment &mdash; `first..increment..+`

This is equivalent to `openRange(first, increment)`.


### Multiplicative Infix Operators (Precedence 5)

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

#### Euclidean Division &mdash; `expression / expression`

This asserts that both expressions result in numbers, and results in the
quotient of the two numbers (first over second), using Euclidean
division.

#### Euclidean Modulo &mdash; `expression % expression`

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


### Additive Infix Operators (Precedence 4)

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


### Comparison Infix Operators (Precedence 3)

Comparisons in Samizdat are chainable: `x < y <= z` is the same as saying
`(x < y) & (y <= z)` with the additional guarantee that `y` is only
evaluated once. This falls naturally out of the value-or-void logic
model of the language.

#### Same-type comparison &mdash; `== != < > <= >=`

These are the type-aware comparison operators. Every type can &mdash;
and all the core types do &mdash; define an order, and these operators
are how to use those type-specific orders. The general contract is that
evaluating these operators results in the right-hand value if the ordering
is satisfied (true), or void if not. If the left-hand and right-hand sides
are not comparable, then the contract is to cause a terminal error (rather
than returning void).

These expressions correspond to making message calls of the left-hand
side, using one of the message names `eq` `ne` `lt` `gt` `le` or `ge`,
with the usual correspondence between the operators and those names.
The message is called on the left-hand value, passing it the right-hand
value as its sole argument.

For core values in particular: The result of comparison is the same as
a call to the similarly-named core library functions, assuming the two
arguments are of the same core type. It is a terminal error if the two
expressions are not of the same core type. It is also a terminal error if
the left-hand expression is a core value, and the right-hand value is a
derived value (for example, trying to compare `10` and
`@[Int: "not-really-an-int"]`).


#### Total-order comparison &mdash; `\== \!= \< \> \<= \>=`

These are total-order comparison operators. There is a total ordering of
values within Samizdat, and these operators' results are based on that
ordering.

These expressions correspond to calls to the library functions
`eq` `ne` `lt` `gt` `le` and `ge` (with the obvious mapping of operator
to function). See the definition of those functions for more details.

**Note:** This can sometimes have surprising results, e.g. when comparing
ints and floating point numbers.


### Value/Void Logical-And Operator (Precedence 2) &mdash; `expression & expression`

This is short-circuit logical-and (conjunction). When evaluating this
operator, the first (left-hand) expression is evaluated. If that results
in void, then the entire expression results in void. Otherwise, the second
(right-hand) expression is evaluated, and its result (whether a value or
void) becomes the result of the outer expression.

The value of the left-hand side can be referred to on the right-hand side
by adding a name binding on the left. Do this by enclosing the left-hand
expression in parentheses, and prefixing it with a def-assignment, e.g.
`(name := expression) & somethingWith(name)`.


### Value/Void Logical-Or Operator (Precedence 1) &mdash; `expression | expression`

This is a short-circuit logical-or (disjunction). When evaluating this
operator, the first (left-hand) expression is evaluated. If that results
in a value (but not void), then the entire expression results in that same
value. Otherwise, the second (right-hand) expression is evaluated, and its
result (whether a value or void) becomes the result of the outer expression.

**Note:** The question-mark-colon trinary operator from C (and descendants)
is obviated in Samizdat by this and the logical-and operator.
`x ? y : z` in C can generally be turned into `x & y | z` in Samizdat,
as long as `y` is never void. If `y` can legitimately be void, then the
slightly longer form `(x & y? | z?)*` is an equivalent that ensures that
a void `y` won't improperly cause `z` to be evaluated.
