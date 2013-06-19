Samizdat Language Guide
=======================

Expression Operators
--------------------

Samizdat provides many of the same operators found throughout the C family,
as well as a few new ones.


### Postfix Operators (Precedence 7, highest / tightest)

Postfix operators have the highest precedence in the language, binding
more tightly than any other operators, including prefix operators.

#### Apply function &mdash; `expression(arg, arg, ...) { closure } { closure } ...`

To apply an expression as a function, place arguments to apply it to
between parentheses. The result of this expression is the same as the result
of the function call. If the function call returned void, then
the expression's result is also a void.

In order to make it convenient to define control-structure-like functions,
any number of closure literals may follow the closing parenthesis. All such
closures are taken to be additional arguments to the function. For example,
`foo(bar) { baz }` means the same thing as `foo(bar, { baz })`.

If there are no arguments (including no closure arguments), parentheses are
required to unambiguously indicate that function application is to be
performed.

All of the expression to apply and all of the arguments must be non-void.

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

All of the expression to index into and all of the indices must be non-void.

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


### Prefix Operators (Precedence 6)

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


### Infix Operators

As is usual for C family languages, infix operators are grouped into
several precedence layers. The following list is ordered from highest
(tightest binding) to lowest (loosest binding) precedence.

### Multiplicative Infix Operators (Precedence 5)

### Additive Infix Operators (Precedence 4)

### Comparison Infix Operators (Precedence 3)

### Logical And Operator (Precedence 2)

### Logical Or Operator (Precedence 1, lowest / loosest)
