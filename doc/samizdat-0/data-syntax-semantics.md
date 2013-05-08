Samizdat Layer 0
================

Data Syntax And Semantics In A Nutshell
---------------------------------------

This section provides a few quick examples of what *Samizdat
Layer 0* data types look like as text, and what they mean.

First and foremost on the topic of semantics, all data values are
immutable. For example, appending to or deleting from a list always
results in a new list, with the original remaining unchanged.

With regards to naming, to avoid naming conflicts with the type names
used in the final language layer, in the low layer, type names get a
suffix "let" (to imply "little version", like "booklet" is to "book").

As with naming, to avoid higher layer syntactic conflicts, most low
layer types are represented using an initial `@` character.

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


### Intlet

An `intlet` is a signed arbitrary-precision integer value, sometimes
called a "bigint" or "BigInteger" (even though they aren't always actually
that big). In the C implementation, there is actually a limitation that
intlets only have a 32-bit signed range, with out-of-range arithmetic
results causing failure, not wraparound.

Intlets are written as an initial `@`, followed by an optional `-`
(minus sign), followed by one or more decimal digits. The digits are
interpreted in base 10 to form the number. (There is no hexadecimal
representation.)

```
@0
@20
@-234452
```


### String

A `string` is a sequence of zero or more Unicode code points.

Strings are written as an initial `@"`, followed by zero or
more character representations, followed by a final `"`.

Characters are self-representing, except that there are three
(and only three) backslash-quoted escapes:

* `\\` &mdash; backslash itself
* `\"` &mdash; a double quote
* `\n` &mdash; newline (Unicode U+0010)

If a string's contents form a valid identifier (e.g. variable
name) in the *Samizdat Layer 0* syntax, then it is valid to
omit the double-quote delimiters.

```
@""                           # the empty string
@"Hello, Самиздат!"
@"\"blort\" -- potion that enables one to see in the dark.\n"
@fizmo
```


### Listlet

A `listlet` is a sequence of zero or more other values.

Listlets are written as an initial `@[`, followed by zero or
more value representations, followed by a final `]`.

```
@[]                           # the empty listlet
@[@1]
@[@blort @fizmo @igram]
@[@[@1] @242 @-23]
```


### Maplet

A `maplet` is a sequence of zero or more mappings (also called bindings)
from arbitrary keys to arbitrary values. Keys and values are both
allowed to be any type of value. Notably, keys are *not* restricted to
only being strings (or string-like things).

Non-empty maplets are written as an initial `@[`, followed by one or
more mappings, followed by a final `]`. Mappings are written as
the key representation, followed by an `=`, followed by the value
representation.

To avoid ambiguity with the empty listlet, the empty maplet is
written as `@[=]`.

```
@[=]                          # the empty maplet
@[@1=@"number one"]
@[@blort = @"potion; the ability to see in the dark"
  @fizmo = @"spell; unclogs pipes"
  @igram = @"spell; make purple things invisible"]
@[@[@complex @data @as @key] = @"Handy!"]
```


### Uniqlet

A `uniqlet` is a bit of an odd duck. Uniqlets are opaque, except that
no uniqlet is equal to any other uniqlet. In practice, uniqlets are
used to help bridge the divide between data and not-data, with some of
the main not-data sorts of things in the system being functions.

One way of thinking about uniqlets is that they are an "atomic
unit of identity" which can be represented as pure data.

Uniqlets are written as simply `@@`. Each mention of `@@` refers
to a different value.

```
@@
```


### Highlet

A `highlet` is a combination of a type tag value and an optional
payload value. Highlets are the bridge between low-layer data and
high-layer data (hence the name). Highlets are also used as the
low-layer type returned by parsing (including tokenization) functions.

Highlets are written as an initial `[:`, followed by a type tag
representation (an arbitrary value), optionally followed by a payload
value representation (another arbitrary value), followed by a final
`:]`.

```
[:@null:]                     # the value usually just written as `null`
[:@boolean @0:]               # the value usually just written as `false`
[:@boolean @1:]               # the value usually just written as `true`
[:
  @spell
  @[@name=@frotz @purpose=@"cause item to glow"]
:]
```


### `.`, `~.`, and void

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

In code-like function descriptions, a void result is written `~.`,
which can be read as "not anything" (or with more technical accuracy,
"the complement of any value"). Relatedly, a result that is a value
but without any further specifics is written as `.`. If a function can
possibly return a value *or* return void, that is written as `. |
~.`. None of these forms is *Samizdat Layer 0* syntax, but it is meant
to be suggestive of the matching syntax used in higher layers of the
system (where `.` means "any value", and `~` means set-complement).
