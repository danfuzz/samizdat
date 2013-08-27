Samizdat Language Guide
=======================

Data Types
----------

Samizdat has a small handful of core data types, including a few atomic
types and a few compound data types. Samizdat also predefines a few
derived types, and it allows for any number of user-specified types.
Finally, Samizdat defines a number of special named constant values.

The examples in this section all use literal value syntax for all parts of
values, but it is worth noting that the language syntax allows arbitrary
expressions anywhere where a literal value occurs in these examples.

### Core types

#### Int

An `Int` is a signed arbitrary-precision integer value, sometimes
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

#### String

A `String` is a sequence of zero or more Unicode code points.

Strings are written as an initial `"`, followed by zero or
more character representations, followed by a final `"`. Characters are
self-representing, with a few exceptions, as follows.

In order to aid in formatting multi-line strings, any spaces after
a newline within a string constant are ignored. To start a line with
a space, use `\/` before it, as described below.

A backslash (`\`) introduces an escape sequence. Most (but not all)
such escape sequences cause one or more characters to be substituted
for the escape sequence.

The following are the character substitution escape sequences:

* `\\` &mdash; Backslash itself.
* `\"` &mdash; Double quote itself.
* `\n` &mdash; Newline (Unicode U+000a).
* `\r` &mdash; Carriage return (Unicode U+000d).
* `\t` &mdash; Tab (Unicode U+0009).
* `\0` &mdash; The null character (Unicode U+0000).
* `\xNNNN;` &mdash; Arbitrary character escape. `NNNN` represents one
  or more hexadecimal digits. Additional adjacent hexadecimal character
  specifiers can be included by separating them with commas. As with int
  literals, underscores are ignored and may be used for readability.
* `\&name;` &mdash; XML entity name. `name` represents any of the standard
  XML entity names. As with hexadecimal escapes, multiple entity names
  can be included by separating them with commas. This form also accepts
  the `&#...` and `&#x...` forms for numeric character references See [the XML
  spec for entity names](http://www.w3.org/TR/xml-entity-names/bycodes.html)
  for a full list of names.

There are two additional backslash escapes, neither of which cause any
characters to be included in the result:

* A backslash that is at the end of a line, or only followed by spaces
  at the end of the line, causes the immediately-subsequent newline to
  be ignored. That is, this allows one to continue a string constant
  across lines without introducing newlines.

* The sequence `\/` is converted into nothingness. Its point is to mark the
  end of ignored spaces at the beginning of a line. It is valid to use `\/`
  in front of every line's main content, or *just* in front of lines that
  need it, depending on visual appeal and personal taste.

In addition, the escape sequences `\(...)` and `\{...}` can be used to
include string interpolations in otherwise-literal strings. This syntax
is covered in the section on string formatting.

```
""                            # the empty string
"a"
"fizmo"
"Hello, Самиздат!"
"\x0;"                        # same as "\0"
"\x46,75,7a,7a;"              # same as "Fuzz"
"\&#70,#117,#x7a,#x7a;"       # same as "Fuzz"
"\x1_F60F;"                   # same as "😏"
"\&zigrarr;"                  # same as "\x21dd;" or "⇝"
"\&mu,nu;"                    # same as "μν"
"\"blort\" \&mdash; potion that enables one to see in the dark.\n"

# same as "* A handful\n  of separate\n  lines.\n"
"\
    * A handful
  \/  of separate
  \/  lines.
"
```

#### List

A `List` is a sequence of zero or more other values.

Lists are written as an initial `[`, followed by zero or
more value representations, followed by a final `]`. Values
are separated with commas (`,`).

The contents of generators and collections (lists, maps, strings) can be
"interpolated" into a list (that is, have their elements become elements
of the result) by placing a `*` after the value or expression to
interpolate.

```
[]                            # the empty list
[1]
["blort", "fizmo", "igram"]
[[1], 242, -23]
[[1, 2]*, (3..5)*, [f: 6]*]   # the same as [1, 2, 3, 4, 5, [f: 6]]
```

#### Map

A `Map` is a sequence of zero or more mappings (also called bindings)
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
can be written in a short-hand using the same interpolation syntax as with
lists. In addition, a comma-separated list of keys enclosed in parentheses
can be used in the key position; in this form, naked identifiers have the
same string-literal interpretation as with individual identifiers.

An entire other map can be interpolated into a new map by listing the
map to interpolate followed by a `*`. To avoid ambiguity between a
list and a map that is just full of interpolation, a map may optionally
start with a "degenerate" element of just `:` (as demonstrated in the
final example below).

To avoid ambiguity with the empty list, the empty map is
written as `[:]`.

```
[:]                           # the empty map
[1: "number one"]
[two: 2]                      # the same as ["two": 2]
[true: "yes"]                 # the same as ["true": "yes"]
[(true): "yes"]               # key is (the boolean value) `true`, not a string
[favorites: ["biscuits", "muffins"]]

[
    "blort":  "potion; the ability to see in the dark",
    "borch":  "spell; insect soporific",
    "fizmo":  "spell; unclogs pipes",
    "ignatz": "potion; unknown effect",
    "igram":  "spell; make purple things invisible"
]

[["complex", "data", "as", "key"]: "Handy!"]
[0..9: "digits", 10: "not a digit"]

# The first two here are equivalent. The third contains a variable reference
# to `the`.
[["these", "map", "to", "the"]*: "same value"]
[(these, map, "to", the): "same value"]
[(these, map, "to", (the)): "same value"]

# These are all equivalent.
[first: 1, second: 2, third: 3]
[first: 1, [second: 2, third: 3]*]
[:, [first: 1]*, [second: 2, third: 3]*]
```

#### Function

A `Function` is an encapsulated potential computation. As with many
other languages, functions are generally defined in terms of source code
and an execution context.

See the language guide section on functions for more details.


#### Generic

A `Generic` is a generic function. That is, it is an encapsulated mapping
from types to functions, in this case based on the type of the first
argument passed to the generic function when called.

See the language guide section on generic functions for more details.


#### Box

A `Box` is a holder for some other value. Boxes are sometimes also known
as "cells".

In addition to the box constructor functions, the three functions that
deal with boxes are `fetch` to get the contents of a box (or void if
the box value has yet to be set), `store` to set the contents of
a box, and `canStore` to indicate whether it is okay to call
`store` (which is not the same as `fetch` returning non-void).

As a special case, the special value `nullBox` is a box that is permanently
un-stored. Notably, `store(nullBox, value)` is effectively a no-op. This
arrangement is done in order to make it easy to pass a box into functions
that require one, but where the box value is never needed.


#### Generator

Generators in *Samizdat* are the closest analog to what are sometimes
called "iterators" in other languages (and are sometimes called generators,
to be clear). When called in an appropriate manner, a related series of
generators yields a related sequence of values. Put another way,
generators can be used to "spread" a computation across a series of values.

Most basically, a generator is just a value &mdash; possibly and
preferably, but not necessarily, pure data &mdash; with associated
method bindings (preferably pure functions) and a particular contract.

The contract is as follows:

* The generic function `nextValue` always accepts exactly two arguments,
  a generator and a "yield box" (or equivalent). (Boxes are described
  more completely in a different section.)

* When a generator is not "voided" (out of values to yield), calling
  `nextValue` causes two things to be done:

  * It calls `store(box, value)` on its argument in order to yield
    one value out of itself.
  * It returns a new generator as a result which, when applied to
    `nextValue`, yields the *next* value, and so on.

* When a generator has yielded its final element, it returns a voided
  generator.

* When `nextValue` is called on a voided generator, it does these two things:

  * It calls `store(box)` (with no payload argument) on its argument
    in order to yield void.
  * It returns void.

**Note:** Clients of generators should rely primarily on the return value
from `nextValue` to determine whether the generator has been voided, rather
than on what gets done to the box passed in as the first argument.

Generators also bind a couple other generic functions. See the library
specification for `Generator` for more details.

As a convenience, the global `nullGenerator` is a voided generator.


#### Uniqlet

A `Uniqlet` is a bit of an odd duck. Uniqlets are opaque, except that
no uniqlet is equal to any other uniqlet. In practice, uniqlets are
used to help bridge the divide between data and not-data.

There is no special syntax for uniqlets. They can be constructed by
using the function `makeUniqlet`.

One way of thinking about uniqlets is that they are an "atomic
unit of identity" which can be represented as pure data. Another
way is that, in Lisp terms, `makeUniqlet` is similar to the function
`gensym` or `make-symbol`, but without any string-like name associated with
the result.

```
makeUniqlet()
```


#### Type

A `Type` value represents the type of a value. Every type has a name which
is typically, but not necessarily, a string. There are three major categories
of type:

* All core values (described above, and values of type `type` as described
  here) have a "core type" as their type. The name of each core type is a
  string of the "human" name of the type. By convention, core type names
  are capitalized.

* The type of a data value created using the syntax `@[type: value]`
  is a transparent derived type, where the type's name is the `type` specified
  in the syntax. For example, the type of `@["stuff": [1, 2, 3]]` is a
  transparent type with name `"stuff"`. This is described more fully below
  under "Derived types".

  There is a one-to-one correspondence between a value and a
  transparent derived type with that value as its name. As such, Samizdat
  does not expose transparent derived types directly. These are represented
  as the name of the type. That is, `typeOf(value)` is not actually a
  `Type` in these cases.

* TODO: The type of a data value explicitly created using a type is that type.
  In the language, the only way to do this is with an opaque derived type.
  Such types are created with both a name and a secret. The same secret must
  be used both the create values of the type as well as access the data
  payload of values of the type.


#### Value

This is the supertype of all other types in the language. It is mostly
useful as the type to use in order to bind a default method to a generic
function.


### Derived types

A derived value is one that was constructed with an explicit type tag and
optional data payload.

Samizdat allows one to name a value with an explicit type tag
by placing the type and value within `@[...]` delimiters. Inside
the delimiters, a single binding of the form `type: value` indicates
the type tag and payload data, or if the there is no payload, just a
`type` need be mentioned.

If the type tag is a string that abides by the syntax for identifiers
in the language, then the double quotes may be omitted.

If the type tag is a string and the value has no payload, then the
square brackets may be omitted. If, furthermore, the type tag
abides by the syntax for identifiers in the language, then the double quotes
may be omitted in this already-shortened form.

```
@["heartState": @pure]        # a "heart state" value
@[heartState: @pure]          # shorthand for same

@[
  "spell":
  [name: "frotz", purpose: "cause item to glow"]
]

@["lozenge"]                  # a payload-free value of type "lozenge"
@[lozenge]                    # shorthand for same
@"lozenge"                    # shorthand for same
@lozenge                      # shorthand for same

@["Null"]                     # the value usually just written as `null`
@[Null]                       # same as above
@[(null)]                     # a type-only value with type `null`

@[Boolean: 0]                 # the value usually just written as `false`
@[Boolean: 1]                 # the value usually just written as `true`
```

#### Boolean

The two boolean values `true` and `false` represent truth values.
The language defines these as named constants, which can be defined as:

```
false = @[Boolean: 0]
true = @[Boolean: 1]
```

These values are most useful when placed into variables and passed
as parameters to indicate flags. They are not particularly useful
for combination into logical expressions. See the section on "Logic" below
for details.

#### Null

The value `null` is used when a value is needed for some reason or other
but no particular value is otherwise suitable. The language defines
a named constant `null` to refer to this value. This constant can be
defined as:

```
null = @Null
```
