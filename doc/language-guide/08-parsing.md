Samizdat Language Guide
=======================

Parsing
-------

### Overview

Since parsing is something so many programs have to do, Samizdat
offers language-level facilities for building parsers.

The foundation of parsing is a parser value. A parer value is
a value which binds the `parse` method, with specific requirements
for formal arguments and behavior. See the `Peg` section in the library
specification for more details.

The syntax below provides a convenient way to define standard-form
parsers. It is perfectly acceptable to mix-and-match the parsers
defined using this syntax with ones defined more "manually."


### Details

The following list of parser forms. With respect to the
expression forms, they are listed in precedence order, from loosest to
tightest.

#### Parser values

Parser values (hereafter just "parsers") are written as a single parsing
expression inside "parser braces" `{: ... :}`. Just as regular braces
enclose an anonymous function / closure, parser braces enclose an anonymous
parser.

The result of calling a parser's `parse` method is the same as the result of
the expression it contains.

The `parse` method takes two fixed arguments, a promise to hold the result
of parsing and a generator used to produce input to parse. In addition,
the method should accept a set of additional arguments representing the
items that have already been parsed (in the context of where the parser
is called). **Note:** These details are only important if you are writing
your own parser "manually." When using parser syntax, the language
automatically generates parser values that abide by the expected contract.

The easiest (and most typical) way of calling a parser from non-parser code
is to use the utility function `$Peg::apply(parser, input)`. This takes a
`parser` value and an `input` generator, and returns either the value
successfully parsed by the parser or void if parsing was unsuccessful. See
the `Peg` library section for more details.

#### Matching one of multiple alternates

To match any one of a series of alternates, the alternates are listed in
priority sequence (first one listed gets first "dibs," etc.), separated
by vertical bars (`|`). The result of matching is the same as the result
of whichever alternate was matched.

For example, the parser `{: "f" | "foobar" :}` will match the string
`"foobar"`, resulting in the yielded value `@f` and a remainder of
`"oobar"`. Note that because of the prioritized ordering, the second
alternate could never get picked in this case.

#### Matching sequences of items

To match a sequence of items, the items are simply listed in order.
The result of matching a sequence of items is the yielded result from
the *last* item listed.

For example, the parser `{: @foo @bar :}` will match the token
list `[@foo, @bar, @baz]`, resulting in the yielded value
`@bar` and a remainder of `[@baz]`.

#### Binding a named variable to an item match

In order to use the result of a matched item in a code block (see below),
it is possible to bind a variable to the result. To do so, precede the
item with an identifier name followed by an equal sign (`=`).

Variables are in scope immediately after the successful match of the
item they are bound to, and until the sequence the item is part of
is finished (e.g. syntactically via a close paren or a close parsing
brace). Notably, a variable from one `|`-delimited alternate will *not*
be bound for any other alternate.

For example, the parser `{: (f=@foo b=@bar X) Y :}` will match the token
list `[@foo, @bar, @baz]`, resulting in the yielded value
`@bar` and a remainder of `[@baz]`. At the point marked `X`, a local
variable `f` will be bound to the matched yield of `@foo`, and a local
variable `b` will be bound to the matched yield of `@bar`. At the point
marked `Y`, `f` and `b` are no longer in scope.

#### Lookahead

To perform matching tests without "consuming" any input, an item
can be preceded by a marker to indicate that the item *must* or *must not*
be matched.

To indicate that an item must be matched, precede it by an ampersand (`&`).
When so marked, the item's yield is the same as what it would have
yielded without the mark.

To indicate that an item must not be matched, precede it by a
not/bang (`!`). When so marked, the item's yield is always
`null` when successful, where "success" means *failing* to match the
item in question.

For example:

* The parser `{: &"foobar" "foo" :}` will match the string
  `"foobar"`, resulting in the yielded value `@foo` and a
  remainder of `"bar"`. However, the same parser will *fail* to match
  `"foobaz"` because the lookahead `&"foobar"` will fail.

* The parser `{: !"foobaz" "foo" :}` will match the string
  `"foobar"`, resulting in the yielded value `@foo` and a remainder of
  `"bar"`. However, the same parser will *fail* to match `"foobaz"` because
  the lookahead `!"foobaz"` will fail (because a `"foobaz"` lookahead
  will *succeed* in matching).

#### Repeat matching

It is possible to alter the number of times an item matched by
appending one of three suffixes to the item. In all such cases, the
result of matching is a *list* of the items matched. It is possible
with two of these suffixes to successfully match zero items; in
these cases, the result of matching is the empty list.

To match an item either zero or one time, follow it with a question
mark (`?`). If the item *can* be matched, it *will* be matched.

To match an item zero or more times, follow it with a star (`*`). The
item will be matched as many times as possible, including none.

To match an item one or more times, follow it with a plus (`+`).
The item will be matched as many times as possible. If it could not
be matched at all, then the match will fail.

For example:

* The parser `{: "f"? :}` will match the string `"foobar"`, resulting
  in the yielded value `[@f]` and a remainder of `"oobar"`. The
  same parser will match the string `"blort"`, resulting in the
  yielded value `[]` and a remainder of `"blort"`.

* The parser `{: "f"* :}` will match the string `"ffffuuuu"`, resulting
  in the yielded value `[@f, @f, @f, @f]` and a remainder of `"uuuu"`. The
  same parser will match the string `"blort"`, resulting in the
  yielded value `[]` and a remainder of `"blort"`.

* The parser `{: "f"+ :}` will match the string `"ffizmo"`, resulting
  in the yielded value `[@f, @f]` and a remainder of `"izmo"`. The
  same parser will fail to match the string `"blort"`, since there is
  not even a single `"f"` at the start of the input.

#### Grouping

To override the default precedence of the syntax, a parsing expression
can be placed between parentheses (`( ... )`). The result of a parenthesized
expression is the same as result of the expression so parenthesized.

For example:

* The parser `{: (@foo @bar) :}` is equivalent to the parser
  `{: @foo @bar :}`.

* The parser `{: (@foo | @bar) @baz :}` is equivalent to the parser
  `{: @foo @baz | @bar @baz :}`.

* The parser `{: zamboni=(@foo | @bar) X }` will match either a `@foo`
  token or a `@bar` token, and at the point marked `X` a variable named
  `zamboni` will be bound with the result of parsing the `@foo`-or-`@bar`.

#### Matching using other parser functions (terminal)

A parser function can delegate to another parser function by naming
that other parser function (as a variable reference). The result of parsing
is identical to whatever that other parsing function returns.

For example:

* The parser `{: foo :}` will match whatever `foo` matches,
  assuming that `foo` is a properly-written parser function. The yielded
  result will be the same as the `foo` parser's yield.

#### Matching a single token (terminal)

Tokens are the basic terminals in the context of tree parsing. When
performing tree parsing, tokens are represented as records,
with the class indicating the parsed class of the token, and the
payload optionally used to hold the token's value.

To match a single token, name the token in one of the shorthand
token forms `@type` or `@"type"`. This will succeed in matching any
token whose class tag is as given, yielding that token directly
(including any payload data) as the result.

For example, the parser `{: @foo :}` will match the token list
`[@foo, @bar]`, resulting in the yielded value `@foo` and a
remainder of `[@bar]`.

#### Matching a sequence of one or more characters (terminal)

Characters are the basic terminals in the context of tokenization.
When performing tokenization, characters are represented as single-element
strings.

To match a single literal character, write the character as a regular
string literal. To match a sequence of two or more characters, write the
character list as a multi-character string literal. In both cases, a
successful match will result in a *single* token whose class
tag is the matched string.

For example:

* The parser `{: "f" :}` will match the string `"foobar"`, resulting in
the yielded value `@f` and a remainder of `"oobar"`.

* The parser `{: "foo" :}` will match the string `"foobar"`, resulting in
the yielded value `@foo` and a remainder of `"bar"`.

#### Matching an arbitrary terminal item (terminal)

To match an arbitrary terminal item (character or token), use a
plain dot (`.`).

For example, the parser `{: . . . :}` matches an arbitrary
sequence of three terminals, with the result being the value of the
third terminal.

#### Matching the end-of-input (terminal)

To match the end of input, use a not-dot (`!.`). This only ever matches
when there is no input available (that is, when the input is `[]`). When
matched, this always yields the result value `null`.

For example, the parser `{: "foo" !. :}` will match the string `"foo"` but
only if it's at the end of input, resulting in the yielded value `null`
and a remainder of `[]`.

#### Successfully matching nothing (terminal)

To explicitly match an empty list of terminals, use an empty pair of
parentheses (`()`). This always succeeds without consuming any input,
yielding the result value `null`.

For example:

* The parser `{: () :}` always succeeds, resulting in the yielded
  value `null`.

* The parser `{: "foo" | () :}` always succeeds, resulting in the yielded
  value `@foo` if the input begins with `"foo"`, or resulting in the
  yielded value `null` if not.

#### Matching character or token sets (terminal)

To match a set of characters (for tokenization) or tokens (for tree parsing),
list them between square brackets (`[ ... ]`). Characters of a character
set can be combined into a single string literal for convenience; e.g.
`["x" "y"]` and `["xy"]` are equivalent. In addition, ranges of characters
can be represented as the characters (in the form of single-character
strings) separated with the token `..`; e.g. `["a".."d"]` and `["abcd"]`
are equivalent.

To match any terminal *other than* items from a particular set, precede the
set contents with a not/bang (`!`), *inside* the brackets. Note that
there is a difference between a complemented set (that is, this form), which
will consume one input terminal when successful, and a lookahead failure of
a set (`![ ... ]`), which never consumes input (see above for details).

For example:

* The parser `{: ["blort"] :}` or its equivalent `{: ["b" "l" "o" "r" "t"] :}`
  will match any of the characters `b` `l` `o` `r` or `t`.

* The parser `{: [! "\n"] :}` will match any terminal but a newline.

* The parser `{: ["A".."Z"] :}` will match any upper-case ASCII letter.

* The parser `{: [@foo @bar] :}` will match either a `@foo` or a `@bar` token.

* The parser `{: [! @foo @bar] :}` will match any terminal but a `@foo`
  or a `@bar` token.

Future direction: It might become possible to name symbolic character
sets, such as for example `[whitespace punctuation "z"]`.

#### Running arbitrary code instead of consuming input (terminal)

To cause arbitrary code to run in the context of parsing, place that code
between regular braces (`{ ... }`). This is a variant of the anonymous
function syntax, where it is valid to define a yield variable (`/name`)
but not any formal arguments. In order for parsing to succeed, the code must
yield a value, which then interacts with the rest of the parsing rules as
would any other parsing result.

Any bound parsing result variables (see above) that are in scope of the
code block are available to be used in the code block.

For example:

* The parser `{: { 23 } :}` will always succeed, yielding the int
  value `23` and consuming no input.

* The parser `{: { "stuff" } :}` will always fail, since the code block never
  yields a value.

* The parser `{: f=@foo { [[[f]]] } :}` will match the input `[@foo @bar]`,
  resulting in the yielded value `[[[@foo]]]` and a remainder of `[@bar]`.

* The parser `{: f=@foo { /out -> yield /out [[[f]]] } :}` is just like the
  previous example, except it is written with an explicit yield definition.

#### Running code to produce a parser value

Sometimes the actual parsing that needs to be done can't be totally
specified in advance. In these cases, it is possible to prefix a regular
non-parser term (e.g. a simple variable reference or a more complicated
expression surrounded by parentheses) with `%`. The term is *not* evaluated
immediately, but instead is evaluated when encountered during a parsing
operation. When evaluated, it is expected to produce a parser, and then
*that* parser value is called upon to perform parsing.

It is valid to refer to any variables bound by the parse-in-progress in
the term.

The most common use of this is to enable forward or recursive referencing of
parser rules, such as:

```
def parRecursive;
parRecursive := {: "x" %parRecursive "y" | () :};
```

Without the `%`, the above would fail because `parRecursive` isn't yet
bound at the use site.


#### Future direction: Destructuring bind

If in the future a "destructuring bind" form is supported, then it
will probably be introduced by an approximate/tilde (`~`).
