Samizdat Language Guide
=======================

String Formatting
-----------------

Samizdat provides string content interpolation inside literal strings,
in order to make dynamic string construction reasonably straightforward.

Interpolation is placed in a string by surrounding code to interpolate
either in `\(...)` or `\{...}`. The two forms differ in terms of what
they expect within the delimiters.

In addition, after the backslash and before the opening parenthesis or
brace, a format specifier can be included, introduced with the tried
and true percent sign (`%`). These are outlined below, but see also
the documentation for the library function `formatterFor`, which
this functionality is based upon.

### `\(...)` `\[...]` &mdash; Expression interpolation

An arbitrary expression (not statement) can be placed within parentheses,
and a square-bracket-delimited expression (such as a list construction or
comprehension) can be used directly. When the interpolation is evaluated, the
expression gets evaluated, and its result is passed on to the indicated (or
implied) formatter, producing the string to include.

For example:

```
"I have \(5 * 5) muffins." => "I have 25 muffins."
"Digits \[0..9]."          => "Digits 0123456789."
```

Expressions can evaluate to void, in which case the indicated (or implied)
formatter gets called without an argument.

### `\{...}` &mdash; Block interpolation

A nullary (no argument) block can be placed within curly braces.
When the interpolation is evaluated, the block gets evaluated. This
results in a closure value. That closure value is then called with
no arguments, producing a result (or void). That result is passed on
to the indicated (or implied) formatter, producing the string to include.

For example:

```
"I have \{ def x = 5; -> x * x } muffins."

=>

"I have 25 muffins."
```

Blocks can yield void, in which case the indicated (or implied)
formatter gets called without an argument.

### `%s` &mdash; Default string conversion

A format specifier of `%s`, or an omitted format specifier, passes strings
through to the result without change, concatenates list elements which
themselves get converted to strings as if by `%s`, and converts void to the
empty string.

For example:

```
"I like \%s("biscuits")."                     => "I like biscuits."
"I like \("biscuits")."                       => "I like biscuits."
"I like \([@muffins, @scones"])."             => "I like @muffins@scones."
"I like \(["muffins", [" and "], "scones"])." => "I like muffins and scones."
```

### `%q` &mdash; Data quoting

A format specifier of `%q` converts all values to a form meant to hew
closely to the corresponding literal forms in the Samizdat source
language. Void is converted to the string `"void"`.

For example:

```
"I like \%q("biscuits")."           => "I like \"biscuits\"."
"I like \%q([@muffins, @scones"])." => "I like [@muffins, @scones]."
```

### `%Q` &mdash; "Unadorned" data quoting

A format specifier of `%Q` is just like `%q`, except that the outermost
layer of adornment is omitted.

For example:

```
"I like \%Q("biscuits")."            => "I like biscuits."
"I like \%Q([@muffins, @scones"])."  => "I like @muffins, @scones."
"I like \%Q([[@berries], "fruit"])." => "I like [@berries], \"fruit\"."
```

### `%x` &mdash; Hexadecimal conversion

A format specifier of `%x` assumes its argument is an int, and converts it
to a hexadecimal representation (with no intro prefix).

For example:

```
"I can eat \%x(50) eggs." => "I can eat 32 eggs."
```
