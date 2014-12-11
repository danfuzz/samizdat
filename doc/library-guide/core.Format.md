Samizdat Layer 0: Core Library
==============================

core.Format (string formatting)
-------------------------------

This module provides a handful of functions for taking arbitrary values
and formatting them into strings. Many of the functions are used
implicitly by the string interpolation system.


<br><br>
### Functions

#### `formatterFor(formatSpec) -> isa Function`

This takes a formatting specification string and returns a formatter
function which takes a single argument and applies the so specified
formatting to it, yielding a string. The string must be one of the
following, with the indicated meaning:

* `q` &mdash; "Quotes" the argument by calling `source` on it
  (see which).

* `Q` &mdash; "Quotes" the argument without top-level adornment, by
  calling `sourceUnadorned` on it (see which).

* `s` &mdash; "Quotes" a non-string argument, by calling `usual`
  on it (see which).

* `x` &mdash; Converts the argument, which must be an int, into a hexadecimal
  string.

**Syntax Note:** Used in the translation of string interpolation forms.

#### `int(value, optBase?) -> isa String`

Converts an int into a string form, in the given base which defaults to
10. If specified, `optBase*` can be any int in the range `(2..36)`.

#### `intHex(value) -> isa String`

Same as `int(value, 16)`.

#### `source(value?) -> isa String`

Converts an arbitrary value into a string representation form
that is meant to mimic the Samizdat source syntax. If `value` is not passed,
this returns the string `"void"`.

**Note:** The output differs from Samizdat Layer 0 syntax in that
string forms can include two escape forms not defined in the
language:

* `\0` &mdash; This represents the value 0.

* `\xHEX;` where `HEX` is a sequence of one or more hexadecimal digits
  (using lowercase letters) &mdash; These represent the so-numbered
  Unicode code points, and this form is used to represent all
  non-printing characters other than newline that are in the Latin-1
  code point range.

#### `sourceUnadorned(value?) -> isa String`

This is just like `source`, except that top-level adornment
(quotes, etc.) are not produced.

#### `usual(value?) -> isa String`

Converts an arbitrary value into a string representation form, meant
to be useful for producing the "usual" human-oriented output. This is
the formatter used during string interpolation if no format specifier
is given.

* If `value` is a string, it is returned as-is.

* If `value` is a list, its elements are converted as if by calling this
  function on them, and then concatenated together without any separator
  characters.

* If `value` is void (that is, not passed), this returns the empty string.

* Otherwise, this behaves just like `source(value)`.
