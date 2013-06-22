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
arguments to apply it to, between parentheses. The result of this expression
is the same as the result of the function call. If the function call returns
void, then the expression's result is also a void.

In order to make it convenient to define control-structure-like functions,
any number of block closure literals may follow the closing parenthesis. All
such closures are taken to be additional arguments to the function.
For example, `foo(bar) { baz }` means the same thing as `foo(bar, { baz })`.

If there are no arguments (including no closure arguments), parentheses are
required to unambiguously indicate that function application is to be
performed. That is, `foo()` and `foo { block }` are both function calls,
but plain `foo` is just a variable reference.

As with list literal syntax, an argument whose type is a list can have
its contents "interpolated" into a function call argument list by following
the argument with a star. For example, `foo(bar, [1, 2]*)` means the
same thing as `foo(bar, 1, 2)`. This works for all argument expressions
(not just literals), so long as the expression evaluates to a list.

The expression to apply and all of the arguments must be non-void.

#### Access collection &mdash; `expression[index, index, ...]`

To index into a collection (list, map, or string) by integer index (lists or
strings) or key (maps), place the index value inside square brackets. The
result of the expression is the value located at the indicated index within
the collection, or void if the indicated element does not exist (e.g., index
out of range or key not in map).

If the value to be indexed has a deeper structure to be accessed, additional
indices may be placed within the square brackets, separated by commas. With
such a form, it is okay for the indexed elements to not exist, in which case
the entire expression has a void result. It is *not* okay for a found element
to not be indexable (resulting in termination of the runtime).

As with function calls, a star after an index expression indicates
interpolation. If used, the so-interpolated expression must evaluate to
a list.

The expression to index into and all of the indices must be non-void.

**Note:** The difference between `x[y, z]` and `x[y][z]` is that it is not
an error in the former for `x[y]` to be void, whereas in the latter `x[y]`
being void would be a fatal error.

#### Access collection by string literal &mdash; `expression.name`

Similar to defining map keys literally, as a shorthand one can index into
maps by literal string key by placing the literal key after the map,
separated by a dot. If the key has the token syntax for an identifier, then
double quotes are optional.

If the value to be indexed has a deeper structure to be accessed, additional
literal keys may be appended to the expression, again separated by dots.

The meanings of `expression.name` and `expression["name"]` are identical.
Similarly, the meanings of `expression.name1.name2` and
`expression["name1", "name2"]` are identical.

#### Convert Value-or-void to list &mdash; `expression?`

In case an expression might legitimately result in a value or void, that
inner expression can be converted into a list by appending a question mark to
it. If the inner expression results in a value, the outer expression results
in a single-element list of the result. If the inner expression results in
void, the outer expression results in the empty list.

#### Interpolate list value &mdash; `expression*`

The star postfix operator is the converse of the question mark postfix
operator (above). It takes an expression whose value must be a list of
either zero or one element, and results in the lists's sole value or
void (the latter given the empty list).

It is valid to use this operator to possibly-yield a value (that is, yield
either a value or void) from a function. Inside expressions, it is only
valid to use it to extract the sole value from a single-element list
(since void expressions aren't valid other than at yield points).

**Note:** A postfix star expression as an element of a function call
argument list, as a list literal element, or as a map literal key has
a slightly different (but related) meaning. See the documentation on
those constructs for more details.


### Prefix Operators (Precedence 7)

Prefix operators are higher in precedence than infix operators, but lower
in precedence than postfix operators.

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
the logical sense of the expression. If the inner expression is the
boolean value `false`, the outer expression's result is `true`. In all
other cases, the outer expression's result is `false`.

**Future direction:** Samizdat may switch to a logic model where, for
the purpose of conditionals, void means false, and non-void means true.

#### Bitwise complement &mdash; `!!!expression`

Placing a triple-bang in front of an expression asserts that the inner
expression results in an int, and results in the bitwise complement of
the inner expression's result.


### Range Operator (Precedence 6) &mdash; `expression..expression`

This indicates a range of values of ints or characters. The two expressions
must be of the same type, either ints or strings. If strings, the expressions
must each be of size 1. It is an error (terminating the runtime) if types
and sizes are inappropriate. It is also an error (terminating the
runtime) if the left expression sorts as after the right expression
(that is, if `left > right`).

The result of evaluating this expression is an interpolation of the
range of elements. As such, if the range is more than one value, then
it is only valid where general interpolations are valid (e.g. as list
elements).


### Multiplicative Infix Operators (Precedence 5)

#### Multiplication &mdash; `expression * expression`

This asserts that both expressions result in numbers, and results in the
product of the two numbers.

#### Division &mdash; `expression / expression`

This asserts that both expressions result in numbers, and results in the
quotient of the two numbers (first over second).

#### Remainder &mdash; `expression % expression`

This asserts that both expressions result in numbers, and results in the
remainder after division of the two numbers (first over second).

#### Bitwise shift left &mdash; `expression <<< expression`

This asserts that both expressions result in ints, and results in the
first one shifted left by the number of bits indicated by the second one.
If the second expression results in a negative number, this instead becomes
a right shift.

#### Bitwise shift right &mdash; `expression <<< expression`

This asserts that both expressions result in ints, and results in the
first one shifted right by the number of bits indicated by the second one.
If the second expression results in a negative number, this instead becomes
a left shift.


### Additive Infix Operators (Precedence 4)

#### Addition &mdash; `expression * expression`

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
`(x < y) && (y <= z)` with the additional guarantee that `y` is only
evaluated once.

#### Same-type comparison &mdash; `== != < > <= >=`

These are the same-type comparison operators. These result in `false` if
the two expressions are not of the same type. If they *are* of the same
type, then the comparison result depends on the type.

TODO: Expand on this.

#### Total-order comparison &mdash; `\== \!= \< \> \<= \>=`

These are total-order comparison operators. There is a total ordering of
values within Samizdat, and these operators' results are based on that
ordering.

**Note:** This can sometimes have surprising results, e.g. when comparing
ints and floating point numbers.

TODO: Expand on this.

### Logical And Operator (Precedence 2) &mdash; `expression && expression`

This is a short-circuit logical and. When evaluating this operator, the first
(left-hand) expression is evaluated. If that results in `false`, then the
entire expression results in `false`. Otherwise, the second (right-hand)
expression is evaluated, and its result becomes the result of the outer
expression.

**Future direction:** Samizdat may switch to a logic model where, for
the purpose of conditionals, void means false, and non-void means true.

### Logical Or Operator (Precedence 1, lowest / loosest) &mdash; `expression || expression`

This is a short-circuit logical and. When evaluating this operator, the first
(left-hand) expression is evaluated. If that results in anything but `false`,
then the entire expression results in that same value. Otherwise, the second
(right-hand) expression is evaluated, and its result becomes the result of
the outer expression.

**Future direction:** Samizdat may switch to a logic model where, for
the purpose of conditionals, void means false, and non-void means true.
