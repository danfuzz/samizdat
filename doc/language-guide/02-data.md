Samizdat Language Guide
=======================

Data Types
----------

Samizdat has a small handful of atomic data types and a few compound data
types. There are also a few special named constant values.


### Int

An `int` is a signed arbitrary-precision integer value, sometimes
called a "bigint" or "BigInteger" (even though they aren't always actually
that big).

Ints are written with an optional base specifier &mdash; `0x` for
hexadecimal or `0b` for binary &mdash; followed by an optional
minus sign (`-`) to indicate a negative value (only valid if a base specifier
is present), and finally followed by one or more digits in the indicated
(or implied) base.

Underscores (`_`) may be placed freely after any digit in an int literal
without changing the meaning. If a base specifier is present, underscores
may also be placed before the first digit. This feature is intended
to aid in the readability of longer constants.

A minus sign in front of an int constant (as in the second example below)
is valid syntax, but the minus sign in this case is *not* part of the
constant; rather, it is an application of the unary minus operator to
the constant.

```
0
-9
20
1_234_452
0x1234abcd
0x-_ABCDEF
0b1011_0111_1110_1111
```


### String

A `string` is a sequence of zero or more Unicode code points.

Strings are written as an initial `"`, followed by zero or
more character representations, followed by a final `"`.

Characters are self-representing, except that the following
backslash-quoted escapes are recognized:

* `\\` &mdash; backslash itself.
* `\"` &mdash; a double quote.
* `\n` &mdash; newline (Unicode U+000a).
* `\r` &mdash; carriage return (Unicode U+000d).
* `\t` &mdash; tab (Unicode U+0009).
* `\0` &mdash; the null character (Unicode U+0000).
* `\xNNNN;` &mdash; arbitrary character escape. `NNNN` represents one
  or more hexadecimal digits. Additional adjacent hexadecimal character
  specifiers can be included by separating them with commas. As with int
  literals, underscores are ignored and may be used for readability.
* `\&name;` &mdash; XML entity name. `name` represents any of the standard
  XML entity names. See [the XML spec for entity
  names](http://www.w3.org/TR/xml-entity-names/bycodes.html) for a full
  list.

```
""                            # the empty string
"a"
"fizmo"
"Hello, Самиздат!"
"\x0;"                        # same as "\0"
"\x46,75,7a,7a;"              # same as "Fuzz"
"\x1_F612;"                   # same as "😒"
"\&zigrarr;"                  # same as "\x21dd;" or "⇝"
"\"blort\" \&mdash; potion that enables one to see in the dark.\n"
```


### List

A `list` is a sequence of zero or more other values.

Lists are written as an initial `[`, followed by zero or
more value representations, followed by a final `]`. Values
are separated with commas (`,`).

A range of either ints or single-character strings can be included
in a list using the syntax `start..end`, where `start` and `end` are
either both ints or both single-character strings.

The contents of other lists can be "interpolated" into a list (that is,
have their elements become elements of the result) by placing a `*`
after the inner list.

```
[]                            # the empty list
[1]
["blort", "fizmo", "igram"]
[[1], 242, -23]
[[1, 2]*, 3..5, "a".."c"]     # the same as [1, 2, 3, 4, 5, "a", "b", "c"]
```


### Map

A `map` is a sequence of zero or more mappings (also called bindings)
from arbitrary keys to arbitrary values. Keys and values are both
allowed to be any type of value. Notably, keys are *not* restricted to
only being strings (or string-like things).

Non-empty maps are written as an initial `[`, followed by one or
more mappings, followed by a final `]`. Mappings are written as
the key representation, followed by an `:`, followed by the value
representation. Mappings are separated with commas.

As a short-hand, a string key with the same form as an identifier can
be written without the quotes.

A group of mappings with multiple keys that map to the same value
can be written in a short-hand using the same range and
interpolation syntax as with lists.

To avoid ambiguity with the empty list, the empty map is
written as `[:]`.

```
[:]                           # the empty map
[1: "number one"]
[two: 2]                      # the same as ["two": 2]
[true: "yes"]                 # the same as ["true": "yes"]
[(true): "yes"]               # key is (the boolean value) `true`, not a string
["blort": "potion; the ability to see in the dark",
 "fizmo": "spell; unclogs pipes",
 "igram": "spell; make purple things invisible"]
[["complex", "data", "as", "key"]: "Handy!"]
[0..9: "digits", 10: "not a digit"]
[["these", "map", "to", "the"]*: "same value"]
```


### Token

A `token` is a combination of a type tag value and an optional
payload value. Tokens are the bridge between low-layer data and
high-layer data. Tokens are also used as the low-layer type
returned by parsing functions (including tokenizers), which is
the direct origin of the name.

Tokens are written as an initial `@[`, followed by a type tag
representation (an arbitrary value), optionally followed by an
`:` and a payload value representation (another arbitrary value),
followed by a final `]`.

Valueless tokens whose type tag is a string constant can be abbreviated
as `@"type"`. If that string constant has the form of a valid
identifier, then the token can be further abbreviated as just `@type`.

Similarly, in the bracketed form, a string type tag which is of identifier
form can be represented without the quotes.

```
@["null"]                     # the value usually just written as `null`
@[null]                       # the value usually just written as `null`
@"null"                       # same as above
@null                         # same as above
@[(null)]                     # a valueless token with type `null`
@["boolean": 0]               # the value usually just written as `false`
@[boolean: 0]                 # same as above
@["boolean": 1]               # the value usually just written as `true`
@[
  "spell":
  [name: "frotz", purpose: "cause item to glow"]
]
```


### Uniqlet

A `uniqlet` is a bit of an odd duck. Uniqlets are opaque, except that
no uniqlet is equal to any other uniqlet. In practice, uniqlets are
used to help bridge the divide between data and not-data, with some of
the main not-data sorts of things in the system being functions.

Uniqlets are written as simply `@@`. Each mention of `@@` refers
to a different value.

One way of thinking about uniqlets is that they are an "atomic
unit of identity" which can be represented as pure data. Another
way is that, in Lisp terms, saying `@@` is like calling `gensym`
or `make-symbol`, but without any string-like name associated with
the result.

```
@@
```


### Null (non-primitive)

The value `null` is used when a value is needed for some reason or other
but no particular value is otherwise suitable. The language defines
a named constant `null` to refer to this value. This constant can be
defined as:

```
null = @null
```


### Boolean (non-primitive)

The two boolean values `true` and `false` represent truth values.
The language defines these as named constants, which can be defined as:

```
false = @[boolean: 0]
true = @[boolean: 1]
```


### `.`, `!.`, and void

It is possible for functions to return without yielding a value.
Such functions are referred to as "void functions" and one can say that
such a function "returns void" or "returns a void result".

Unlike some languages (notably JavaScript), it is invalid to try to
assign the void non-value (`undefined` is the JavaScript equivalent)
to a variable or to pass it as a parameter to some other
function. However, the Samizdat library provides several facilities
to help deal with cases where code needs to act sensibly without
knowing up-front whether or not a given expression will yield a value.
The facilities include the `ifValue` and `ifVoid` library functions,
the question mark (`?`) postfix operator, and argument and list element
interpolation.

**Note:** When describing functions in this (and related) documents,
a void result is written `!.`, which can be read as "not anything"
(or with more technical accuracy, "the failure to match any value").
Relatedly, a result that is a value but without any further specifics is
written as `.`. If a function can possibly return a value *or* return void,
that is written as `. | !.`. While not proper syntax in the language per
se, these are meant to be suggestive of the syntax used in function
argument declaration and in defining parsers.
