Samizdat Layer 0: Core Library
==============================

String Formatting
-----------------

<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `format formatStringlet rest* <> stringlet`

Using the given `formatStringlet`, format the remaining arguments, returning
a stringlet. Formatting syntax is in the style of C's `printf()`. Specifically,
all characters from the `formatStringlet` are passed through to the result
as-is, except that percent (`%`) is used to introduce format conversions
of each of the `rest` arguments. Each time a percent is encountered
(with one exception noted below), another one of the `rest` arguments is
"consumed" and formatted.

The following are the format codes and their meanings:

* `%%` &mdash; Outputs a literal percent. This does not consume an argument.

* `%s` &mdash; Assumes the argument is a stringlet, and includes it in
  the output without additional formatting.

* `%q` &mdash; "Quotes" the argument by passing it through `sourceStringlet`
  (see which).

* `%Q` &mdash; "Quotes" the argument without top-level adornment through
  `sourceStringletUnadorned` (see which).

#### `sourceStringlet value <> stringlet`

Converts an arbitrary value into a stringlet representation form
that is meant to mimic the Samizdat source syntax.

**Note:** The output differs from *Samizdat Layer 0* syntax in that
stringlet forms can include two escape forms not defined in the
language:

* `\0` &mdash; This represents the value 0.

* `\xHEX;` where `HEX` is a sequence of one or more hexadecimal digits
  (using lowercase letters) &mdash; These represent the so-numbered
  Unicode code points, and this form is used to represent all
  non-printing characters other than newline that are in the Latin-1
  code point range.

#### `sourceStringletUnadorned value <> stringlet`

This is just like `sourceStringlet`, except that top-level adornment
(quotes, etc.) are not produced.
