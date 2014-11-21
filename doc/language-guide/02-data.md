Samizdat Language Guide
=======================

Data Types
----------

Samizdat has a small handful of core classes, including a few atomic
data types and a few compound data types. Samizdat also allows for any
number of user-specified classes. Finally, Samizdat defines a number of
special named constant values.

Almost all core classes are concrete (instantiable) and immutable once
constructed. The few exceptions are noted as "abstract" (not instantiable)
or "mutable."

The examples in this section all use literal value syntax for all parts of
values, but it is worth noting that the language syntax allows arbitrary
expressions anywhere where a literal value occurs in these examples.

### Core classes

#### Value (abstract)

This is the ultimate superclass (parent of the parent of…) of all other classes
in the language.

#### Symbol

A `Symbol` is an identifier used to name language constructs, such as
classes and methods. On the method front, symbols are the keys bound to
functions in a class's table of methods, and symbols themselves can be
invoked as functions to perform method dispatch on the first argument of
the function call. Every symbol has a string name.

There are two "flavors" of symbol, interned and unlisted. An interned
symbol is one that can be identified uniquely by its name. That is, there
is a one-to-one correspondence between names and interned symbols.
An unlisted symbol has a name, but it is only possible to refer to it
by identity; that is, one can create new unlisted symbols and pass them
around, but &mdash; unlike interned symbols &mdash; one cannot get a
reference to a pre-existing unlisted symbol other than being passed it
(e.g. as an argument to a call). The name is meant to be suggestive of
unlisted phone numbers.

While symbols are often used implicitly, there is also literal syntax
for referring to them. Refer to an interned symbol with an at-sign (`@`),
followed by the symbol's name in double quotes (`"name"`). If the
name happens to match the syntax of an identifier in the language, then
the quotes are optional.

```
@foo
@"foo"          ## Same meaning as above.
@"++ weird ++"  ## Non-identifier names need to be quoted.
```

#### Core (abstract)

This is the parent class (superclass) of all core classes in the language.
Notably, while in general it is possible for two different classes to have
the same name, every core class's name is unique.

#### Object (abstract)

This is the parent of all non-core classes, other than "record" classes.
Every object class has an associated "secret" which is the key used to
allow construction of objects of that class as well as access the inner
data payload of such objects.

The data payload of an object is always a symbol table.

#### Null

The value `null` is used when a value is needed for some reason or other
but no particular value is otherwise suitable. The language keyword
`null` refers to this value.

#### Bool

A `Bool` is a boolean truth value. The two possible values of this class are
`true` and `false`, with their usual interpretations.

These values are most useful when placed into variables and passed
as parameters to indicate flags, and they can also be used in bitwise
expressions.

Unlike in many languages, boolean values are *not* the base type of value
used for conditional expression, though the `**` and `??` operators can
bridge the divide. See the section on "Logic" in the "Basics" section
for details.

#### Int

An `Int` is a signed arbitrary-precision integer value, sometimes
called a "bigint" or "BigInteger" (even though they aren't always actually
that big).

Ints are written with an optional minus sign (`-`) to indicate a negative
value, followed by an optional base specifier &mdash; `0x` for hexadecimal
or `0b` for binary &mdash; and finally followed by one or more digits in the
indicated (or implied) base.

Underscores (`_`) can be placed freely after any digit in an int literal
without changing the meaning. If a base specifier is present, underscores
can also be placed before the first digit. This feature is intended
to aid in the readability of longer constants.

**Note:** In many contexts, a minus sign is a separate operator token,
and not necessarily part of an int constant.

```
0
-9
20
1_234_452
0x1234abcd
-0x_ABCDEF
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
  literals, underscores are ignored and can be used for readability.
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

In addition, the escape sequences `\(...)`, `\{...}`, and `\[...] `can be
used to include string interpolations in otherwise-literal strings. This
syntax is covered in the section on string formatting.

```
""                            ## the empty string
"a"
"fizmo"
"Hello, Самиздат!"
"\x0;"                        ## same as "\0"
"\x46,75,7a,7a;"              ## same as "Fuzz"
"\&#70,#117,#x7a,#x7a;"       ## same as "Fuzz"
"\x1_F60F;"                   ## same as "😏"
"\&zigrarr;"                  ## same as "\x21dd;" or "⇝"
"\&mu,nu;"                    ## same as "μν"
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

The contents of generators and collections (e.g. boxes, lists, maps, and
strings) can be "interpolated" into a list (that is, have their elements
become elements of the result) by placing a `*` after the value or expression
to interpolate.

```
[]                            ## the empty list
[1]
["blort", "fizmo", "igram"]
[[1], 242, -23]
[[1, 2]*, (3..5)*, {f: 6}*]   ## the same as [1, 2, 3, 4, 5, {f: 6}]
```


#### Map and SymbolTable (data)

`Map`s and `SymbolTable`s are both sequences of zero or more mappings
(also called bindings) from keys to arbitrary values. The two classes
place different restrictions on keys: `Map` keys must all be ordered
with respect to each other, using the defined "total order" of values.
`SymbolTable` keys must all be symbols.

Maps are written as an initial `{`, followed by zero or
more mappings, followed by a final `}`. Symbol tables are the same, except
that the opener is `@{`.

Mappings are written as the key representation, followed by an `:`, followed
by the value representation. Mappings are separated with commas.

Syntactically, keys are "terms," that is, simple values, collection literals,
or parenthesized expressions. As a short-hand, a symbol key with the same
form as an identifier can be written without the quotes; the long form of
an identifier (`\"name"`) also works.

A collection of values can be mapped as keys to a single value, using the
same postfix `*` interpolation syntax that is used with lists, e.g.
`{foo*: value}`.

In addition, explicitly listed keys can be mapped to the same value by
omitting the value after all but the last of the list, e.g.
`{foo: bar: commonValue}`.

An entire other map can be interpolated into a new map by naming the
map to interpolate followed by `*` (with no colon).

As one final shorthand, to map a symbol to the contents of a defined variable
with the same name, it is only necessary to name the variable (no colon,
etc.). This is to handle the common case of putting together a map of current
variable definitions.

`{}` denotes the empty map. `@{}` denotes the empty symbol table.

```
{}        ## the empty map
@{}       ## the empty symbol table
{@x: 1}   ## map with one binding
{x: 1}    ## usual shorthand for same
@{@x: 1}  ## symbol table with same mapping
@{x: 1}   ## usual shorthand for same

def blort = "B";
def zorch = "Z";
{blort, zorch}                  ## shorthand to reference variables
{@blort: blort, @zorch: zorch}  ## longhand of the above

# These are all equivalent.
{first: 1, second: 2, third: 3}
{first: 1, {second: 2, third: 3}*}
{{first: 1}*, {second: 2, third: 3}*}

## The first three here are equivalent. The last contains a variable reference
## to `the`.
{[@these, @map, @to, @the]*: "same value"}
{these: map: @"to": the: "same value"}
{[@these, @map]*: to: the: "same value"}
{these: map: @"to": (the): "same value"}

## The rest of these maps have non-symbol keys, and so couldn't be made
## as symbol tables.

{1: "number one"}                ## int
{(true): "yes"}                  ## the boolean `true`
{["complex", "data"]: "Handy!"}  ## a list
{(0..9)*: "digits", 10: "not"}   ## ints

## The keys here are strings.
{
    "blort":  "potion; the ability to see in the dark",
    "borch":  "spell; insect soporific",
    "fizmo":  "spell; unclogs pipes",
    "ignatz": "potion; unknown effect",
    "igram":  "spell; make purple things invisible"
}
```

#### Builtin

A `Builtin` is an encapsulated potential computation, defined at the
lowest layer of the system. It is a kind of `Function` (see which).

#### Box (mutable)

A `Box` is a holder for some other value. (In other systems, boxes are
sometimes called "cells" or "handles.")

In addition to the box constructor functions, the two functions that
deal with boxes are `fetch` to get the contents of a box (or void if
the box value has yet to be set) and `store` to set the contents of
a box.

All boxes are generators, which when called upon as such will generate
their stored value. A box without a stored value is effectively a
voided generator (which will never generate any value).

There are four predefined box variants:

* A "cell" is a fully mutable box, which can be stored to any number of
  times.

* A "promise" is a box which can be stored to at most once. This kind
  of box is often used as something along the lines of a "reified
  return value."

* A "result" is a box which is immutably bound to a particular value, or
  to void.

* The special value `nullBox` is a box that is permanently empty (succeeds
  but does nothing when `store` is called on it).

#### Class (abstract)

A `Class` value represents the class of a value. Every class has a name, which
is a symbol. There are four categories of class:

* All core values (described above) have a "core class" as their class. The
  name of each core class is a string of the "human" name of the class. By
  convention, core class names are capitalized. Core classes are exported as
  variables in the standard global environment, each with a variable name
  that matches its name. For example, the global reference `String` refers to
  the core class named `String`.

* All values not defined by the system itself have an "object class" as their
  class.

* Every class covered by the previous two items is an *indirect* instance
  of the class `Class`, and is a *direct* instance of its own metaclass, which
  always has a name which is identical to the class's with the prefix `meta_`.
  Metaclasses have a heritage hierarchy that exactly matches the hierarchy
  of their (non-meta) companion classes, with the exception of `Value`.
  `Value` has no superclass, while `meta_Value`'s superclass is `Class`.

* Every metaclass is a direct instance of `Metaclass`. `Metaclass` inherits
  from `Class`.

The class and metaclass arrangement can be confusing at first blush. The
point of the arrangement is so that (a) classes can respond to method calls
just like any other value, and (b) classes can define class methods.


### Record

A record is a value that is constructed with an explicit record name and
data payload. Record names can be either symbols or record classes.

Record literals are introduced with an at-sign (`@`). This is followed by
a required name, which must be a symbol; and then a data payload, which must
be a symbol table. The name must be surrounded by parentheses, and the payload
must be surrounded by curly braces.

As an exception, if the name is literal, then it can be represented as its
double-quoted name, directly after the `@`, with no parentheses required.
In addition, if the name abides by the syntax for identifiers in the language,
then the quotes can be omitted.

The payload can use the same `*`-postfix interpolation syntax that is used
on symbol tables. It is valid to interpolate symbol tables, other records (in
which case the interpolated record name is ignored), and maps (as long as
all keys are symbols).

```
@(@heartState){state: "pure"}  ## "heart state" value, symbol table payload
@"heartState"{state: "pure"}   ## shorthand for same
@heartState{state: "pure"}     ## more usual shorthand for same

@(calcName()){calcPayload()*}  ## with name and payload computed, not literal
```

**Note:** As a convenience, the `get` function works on records
by calling through to `get` on the records's payload value.


### Protocols

There are a number of type-like things in Samizdat, which are effectively
defined as types (per se) which have implemented a particular set of methods.

Protocols are not first-class within Samizdat but they have significance
nonetheless. The following are the protocols currently defined.

#### Function

This is the protocol for function-like things, that is, things which can
be applied to an argument list to produce a result.

As with many other languages, at the highest level functions are generally
defined in terms of source code and an execution environment.

See the language guide section on functions and the library documentation
for `Function` for more details.

#### Collection

This is the protocol for values which encapsulate a group of other values.

See the library documentation for `Collection` for more details.

#### Generator

Generators in Samizdat are the closest analog to what are sometimes
called "iterators" in other languages (and are sometimes called generators,
to be clear). When called in an appropriate manner, a related series of
generators yields a related sequence of values. Put another way,
generators can be used to "spread" a computation across a series of values.

Most basically, a generator is just a value &mdash; possibly and
preferably, but not necessarily, pure data &mdash; with associated
method bindings (preferably pure functions) and a particular contract.

The contract is as follows:

* The method `nextValue` always accepts exactly two arguments,
  a generator and a box (or equivalent). (Boxes are described
  more completely in the section on boxes.)

* When a generator is not "voided" (out of values to yield), calling
  `nextValue` causes two things to be done:

  * It calls `box.store(value)` on its argument in order to yield
    one value out of itself.
  * It returns a new generator as a result which, when applied to
    `nextValue`, yields the *next* value, and so on.

* When a generator has yielded its final element, it returns a voided
  generator.

* When `nextValue` is called on a voided generator, it does one thing:

  * It returns void.
  * In particular, it does *not* call `store` on the given `box`.

**Note:** Clients of generators should rely primarily on the return value
from `nextValue` to determine whether the generator has been voided, rather
than on what gets done (or not) to the box passed in as the first
argument.

Generators also bind a couple other methods. See the library
specification for `Generator` for more details.

As a convenience, the global `nullGenerator` is a voided generator.


#### Sequence

This is a sub-protocol of `Collection` for collections which encapsulate
an ordered sequence of other values, indexable by zero-based
sequence number.

See the library documentation for `Collection` for more details.
