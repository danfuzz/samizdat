Samizdat Layer 0
================

Data Syntax And Semantics In A Nutshell
---------------------------------------

This section provides a few quick examples of what *Samizdat
Layer 0* data types look like as text, and what they mean.

First and foremost on the topic of semantics, all data values are
immutable. For example, appending to or deleting from a list always
results in a new list, with the original remaining unchanged.

Most of the types mentioned here are primitive types implemented
at a fairly low level of the language. A couple, though, are
types that are defined as primitive values with a particular
structure, and are universally a form of the `token` type
(defined below) at the lowest layer.

Keep in mind that many of the restrictions and caveats mentioned
(such as, for example, what can be backslash-escaped in a string)
are specific to *Samizdat Layer 0*, with higher layers providing
much fuller expressivity. It may be useful to think of this layer
as being analogous to the bytecode layer of bytecode-based systems.


### Comments

Comments start with `#` and continue to the end of a line.

```
#
# I am commentary.
thisIsNotCommentary   # ...but this *is* commentary.
```


### Int

An `int` is a signed arbitrary-precision integer value, sometimes
called a "bigint" or "BigInteger" (even though they aren't always actually
that big). In the C implementation, there is actually a limitation that
ints only have a 32-bit signed range, with out-of-range arithmetic
results causing failure, not wraparound.

Ints are written as an optional `-` (minus sign), followed by one or
more decimal digits. The digits are interpreted in base 10 to form the
number. (There is no hexadecimal representation.)

```
0
20
-234452
```


### String

A `string` is a sequence of zero or more Unicode code points.

Strings are written as an initial `"`, followed by zero or
more character representations, followed by a final `"`.

Characters are self-representing, except that there are four
(and only four) backslash-quoted escapes:

* `\\` &mdash; backslash itself
* `\"` &mdash; a double quote
* `\n` &mdash; newline (Unicode U+0010)
* `\0` &mdash; the null character (Unicode U+0000)

```
""                            # the empty string
"Hello, Самиздат!"
"\"blort\" -- potion that enables one to see in the dark.\n"
"fizmo"
```


### List

A `list` is a sequence of zero or more other values.

Lists are written as an initial `[`, followed by zero or
more value representations, followed by a final `]`.

```
[]                            # the empty list
[1]
["blort" "fizmo" "igram"]
[[1] 242 -23]
```


### Map

A `map` is a sequence of zero or more mappings (also called bindings)
from arbitrary keys to arbitrary values. Keys and values are both
allowed to be any type of value. Notably, keys are *not* restricted to
only being strings (or string-like things).

Non-empty maps are written as an initial `[`, followed by one or
more mappings, followed by a final `]`. Mappings are written as
the key representation, followed by an `=`, followed by the value
representation.

To avoid ambiguity with the empty list, the empty map is
written as `[=]`.

```
[=]                           # the empty map
[1="number one"]
["blort" = "potion; the ability to see in the dark"
 "fizmo" = "spell; unclogs pipes"
 "igram" = "spell; make purple things invisible"]
[["complex" "data" "as" "key"] = "Handy!"]
```


### Token

A `token` is a combination of a type tag value and an optional
payload value. Tokens are the bridge between low-layer data and
high-layer data (hence the name). Tokens are also used as the
low-layer type returned by parsing (including tokenization) functions.

Tokens are written as an initial `@[`, followed by a type tag
representation (an arbitrary value), optionally followed by a payload
value representation (another arbitrary value), followed by a final
`]`.

Valueless highlets whose type tag is a string constant can be abbreviated
as `@"type"`. If that string constant has the form of a valid
identifier, then the token can be further abbreviated as just `@type`.

```
@["null"]                     # the value usually just written as `null`
@"null"                       # same as above
@null                         # same as above
@["boolean" 0]                # the value usually just written as `false`
@["boolean" 1]                # the value usually just written as `true`
@[
  "spell"
  ["name"="frotz" "purpose"="cause item to glow"]
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
false = @["boolean" 0]
true = @["boolean" 1]
```


### `.`, `!.`, and void

In *Samizdat Layer 0*, it is possible for functions to return without
yielding a value. Such functions are referred to as "void functions"
and one can say that such a function "returns void" or "returns a void
result".

Unlike some languages (notably JavaScript), it is invalid to try to
assign the void non-value (`undefined` is the JavaScript equivalent)
to a variable or to pass it as a parameter to some other
function. However, the *Samizdat Layer 0* library provides the
`ifValue` and `ifVoid` functions to let code act sensibly when it needs
to call a function but doesn't know up-front whether or not it will yield
a value.

In code-like function descriptions, a void result is written `!.`,
which can be read as "not anything" (or with more technical accuracy,
"the failure to match any value"). Relatedly, a result that is a value
but without any further specifics is written as `.`. If a function can
possibly return a value *or* return void, that is written as `. |
!.`. None of these forms is *Samizdat Layer 0* syntax, but it is meant
to be suggestive of the matching syntax used in higher layers of the
system (where `.` means "any value", `!` means lookahead-failure, and
`|` is used to represent parsing alternatives).
