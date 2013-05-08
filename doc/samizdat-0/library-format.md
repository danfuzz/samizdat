Samizdat Layer 0: Core Library
==============================

String Formatting
-----------------

<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `format formatString rest* <> string`

Using the given `formatString`, format the remaining arguments, returning
a string. Formatting syntax is in the style of C's `printf()`. Specifically,
all characters from the `formatString` are passed through to the result
as-is, except that percent (`%`) is used to introduce format conversions
of each of the `rest` arguments. Each time a percent is encountered
(with one exception noted below), another one of the `rest` arguments is
"consumed" and formatted.

The following are the format codes and their meanings:

* `%%` &mdash; Outputs a literal percent. This does not consume an argument.

* `%s` &mdash; Assumes the argument is a string, and includes it in
  the output without additional formatting.

* `%q` &mdash; "Quotes" the argument by passing it through `sourceString`
  (see which).

* `%Q` &mdash; "Quotes" the argument without top-level adornment through
  `sourceStringUnadorned` (see which).

#### `sourceString value <> string`

Converts an arbitrary value into a string representation form
that is meant to mimic the Samizdat source syntax.

**Note:** The output differs from *Samizdat Layer 0* syntax in that
string forms can include two escape forms not defined in the
language:

* `\0` &mdash; This represents the value 0.

* `\xHEX;` where `HEX` is a sequence of one or more hexadecimal digits
  (using lowercase letters) &mdash; These represent the so-numbered
  Unicode code points, and this form is used to represent all
  non-printing characters other than newline that are in the Latin-1
  code point range.

#### `sourceStringUnadorned value <> string`

This is just like `sourceString`, except that top-level adornment
(quotes, etc.) are not produced.
