Parsing in Samizdat
===================

[It's not yet clear what layer this will end up at.]

Since parsing is something so many programs have to do, Samizdat offers
language-level facilities for building parsers.

The foundation of parsing is the parsing function. A parsing function is
a regular in-language function with specific requirements for formal
arguments and behavior.

A parsing function always accepts exactly two arguments. The first argument
is a `yield` function. The `yield` function is used to indicate the result of
parsing, which can either be an arbitrary non-void value to indicate
parsing success, or void to indicate parsing failure. The second argument
is the `input`. The input is a list of elements to be parsed.

A parsing function, upon success when called, must do two things: It must
call its `yield` to indicate the non-void result of parsing. Then,
it must return a replacement `input` for use as the input to subsequent
parsers, that reflects the removal of whatever elements were consumed
by the parsing (including none). If the parsing function consumed all of
its given input, then it must return `[]` (that is, the empty list).

A parsing function, upon failure when called, must do two things: It must
call its `yield` with no argument to indicate a void result of parsing.
Then, it must also return void.

The syntax below provides a convenient way to define standard-form
parsing functions. It is perfectly acceptable to mix-and-match the parsing
functions defined using this syntax with ones defined more "manually".


Syntax and Semantics
--------------------

The following list of parser forms. With respect to the
expression forms, they are is in precedence, from loosest to tightest.

### Parsing functions

Parsing functions are written as a single parsing expression inside
"parsing braces" `{/ ... /}`. Just as regular braces enclose an anonymous
function / closure, parsing braces enclose an anonymous parsing
function / closure.

The result of calling a parsing function is the same as the result of
the expression it contains.

### Matching one of multiple alternates

To match any one of a series of alternates, the alternates are listed in
priority sequence (first one listed gets first "dibs", etc.), separated
by vertical bars (`|`). The result of matching is the same as the result
of whichever alternate was matched.

For example, the parser `{/ "f" | "foobar" /}` will match the string
`"foobar"`, resulting in the yielded value `@f` and a remainder of
`"oobar"`. Note that because of the prioritized ordering, the second
alternate could never get picked in this case.

### Matching sequences of items

To match a sequence of items, the items are simply listed in order.
The result of matching a sequence of items is the yielded result from
the *last* item listed.

For example, the parser `{/ @foo @bar /}` will match the token
list `[@foo @bar @baz]`, resulting in the yielded value
`@bar` and a remainder of `[@baz]`.

### Binding a named variable to an item match

In order to use the result of a matched item in a code block (see below),
it is possible to bind a variable to the result. To do so, precede the
item with an identifier name followed by an equal sign (`=`).

Variables are in scope at any point after the successful match of the
item they are bound to. Notably, a variable from one `|`-delimited
alternate will *not* be bound for any other alternate.

For example, the parser `{/ f=@foo b=@bar X /}` will match the token
list `[@foo @bar @baz]`, resulting in the yielded value
`@bar` and a remainder of `[@baz]`. At the point marked `X`, a local
variable `f` will be bound to the matched yield of `@foo`, and a local
variable `b` will be bound to the matched yield of `@bar`.

### Lookahead

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

* The parser `{/ &"foobar" "foo" /}` will match the string
  `"foobar"`, resulting in the yielded value `@foo` and a
  remainder of `"bar"`. However, the same parser will *fail* to match
  `"foobaz"` because the lookahead `&"foobar"` will fail.

* The parser `{/ !"foobaz" "foo" /}` will match the string
  `"foobar"`, resulting in the yielded value `@foo` and a remainder of
  `"bar"`. However, the same parser will *fail* to match `"foobaz"` because
  the lookahead `!"foobaz"` will fail (because a `"foobaz"` lookahead
  will *succeed* in matching).

### Repeat matching

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

* The parser `{/ "f"? /}` will match the string `"foobar"`, resulting
  in the yielded value `[@f]` and a remainder of `"oobar"`. The
  same parser will match the string `"blort"`, resulting in the
  yielded value `[]` and a remainder of `"blort"`.

* The parser `{/ "f"* /}` will match the string `"ffffuuuu"`, resulting
  in the yielded value `[@f @f @f @f]` and a remainder of `"uuuu"`. The
  same parser will match the string `"blort"`, resulting in the
  yielded value `[]` and a remainder of `"blort"`.

* The parser `{/ "f"+ /}` will match the string `"ffizmo"`, resulting
  in the yielded value `[@f @f]` and a remainder of `"izmo"`. The
  same parser will fail to match the string `"blort"`, since there is
  not even a single `"f"` at the start of the input.

### Grouping

To override the default precedence of the syntax, a parsing expression
can be placed between parentheses (`( ... )`). The result of a parenthesized
expression is the same as result of the expression so parenthesized.

For example*

* The parser `{/ (@foo @bar) /}` is equivalent to the parser
  `{/ @foo @bar /}`.

* The parser `{/ (@foo | @bar) @baz /}` is equivalent to the parser
  `{/ @foo @baz | @bar @baz /}`.

* The parser `{/ zamboni=(@foo | @bar) X }` will match either a `@foo`
  token or a `@bar` token, and at the point marked `X` a variable named
  `zamboni` will be bound with the result of parsing the `@foo`-or-`@bar`.

### Matching using other parser functions (terminal)

A parser function can delegate to another parser function by naming
that other parser function (as a variable reference). The result of parsing
is identical to whatever that other parsing function returns.

For example:

* The parser `{/ foo /}` will match whatever `foo` matches,
  assuming that `foo` is a properly-written parser function. The yielded
  result will be the same as the `foo` parser's yield.

### Matching a single token (terminal)

Tokens are the basic terminals in the context of tree parsing. When
performing tree parsing, tokens are represented &mdash; hopefully
unsurprisingly &mdash; as values of the Samizdat type `token`, with the
token type indicating the parsed type of the token, and the token value
optionally indicating a data payload.

To match a single token, name the token in one of the shorthand
token forms `@type` or `@"type"`. This will succeed in matching any
token whose type tag is as given, yielding that token directly
(including any payload data) as the result.

For example, the parser `{/ @foo /}` will match the token list
`[@foo @bar]`, resulting in the yielded value `@foo` and a
remainder of `[@bar]`.

### Matching a sequence of one or more characters (terminal)

Characters are the basic terminals in the context of tokenization.
When performing tokenization, characters are represented as single-element
strings.

To match a single literal character, write the character as a regular
string literal. To match a sequence of two or more characters, write the
character list as a multi-character string literal. In both cases, a
successful match will result in a *single* token (token) whose type
tag is the matched string.

For example:

* The parser `{/ "f" /}` will match the string `"foobar"`, resulting in
the yielded value `@f` and a remainder of `"oobar"`.

* The parser `{/ "foo" /}` will match the string `"foobar"`, resulting in
the yielded value `@foo` and a remainder of `"bar"`.

### Matching an arbitrary terminal item (terminal)

To match an arbitrary terminal item (character or token), use a
plain dot (`.`).

For example, the parser `{/ . . . /}` matches an arbitrary
sequence of three terminals, with the result being the value of the
third terminal.

### Matching the end-of-input (terminal)

To match the end of input, use a not-dot (`!.`). This only ever matches
when there is no input available (that is, when the input is `[]`). When
matched, this always yields the result value `null`.

For example, the parser `{/ "foo" !. /}` will match the string `"foo"` but
only if it's at the end of input, resulting in the yielded value `null`
and a remainder of `[]`.

### Successfully matching nothing (terminal)

To explicitly match an empty list of terminals, use an empty pair of
parentheses (`()`). This always succeeds without consuming any input,
yielding the result value `null`.

For example:

* The parser `{/ () /}` always succeeds, resulting in the yielded
  value `null`.

* The parser `{/ "foo" | () /}` always succeeds, resulting in the yielded
  value `@foo` if the input begins with `"foo"`, or resulting in the
  yielded value `null` if not.

### Matching character or token sets (terminal)

To match a set of characters (for tokenization) or tokens (for tree parsing),
list them between square brackets (`[ ... ]`). Characters of a character
set can be combined into a single string literal for convenience, e.g.
`["x" "y"]` and `["xy"]` are equivalent.

To match any terminal *other than* items from a particular set, precede the
set contents with a not/bang (`!`), *inside* the brackets. Note that
there is a difference between a complemented set (that is, this form), which
will consume one input terminal when successful, and a lookahead failure of
a set (`![ ... ]`), which never consumes input (see above for details).

For example:

* The parser `{/ ["blort"] /}` or its equivalent `{/ ["b" "l" "o" "r" "t"] /}`
  will match any of the characters `b` `l` `o` `r` or `t`.

* The parser `{/ [! "\n"] /}` will match any terminal but a newline.

* The parser `{/ [@foo @bar] /}` will match either a `@foo` or a `@bar` token.

* The parser `{/ [! @foo @bar] /}` will match any terminal but a `@foo`
  or a `@bar` token.

Future direction: It may become possible to name ranges of characters,
e.g. `["a".."z" "0".."9"]`, and it may become possible to name symbolic
sets, e.g. `[whitespace punctuation "z"]`.

### Running arbitrary code instead of consuming input (terminal)

To cause arbitrary code to run in the context of parsing, place that code
between regular braces (`{ ... }`). This is a variant of the anonymous
function syntax, where it is valid to define a yield variable (`<name>`)
but not any formal arguments. In order for parsing to succeed, the code must
yield a value, which then interacts with the rest of the parsing rules as
would any other parsing result.

Any bound parsing result variables (see above) that are in scope of the
code block are available to be used in the code block.

For example:

* The parser `{/ { <> 23 } /}` will always succeed, yielding the int
  value `23` and consuming no input.

* The parser `{/ { "stuff" } /}` will always fail, since the code block never
  yields a value.

* The parser `{/ f=@foo { <> [[[f]]] } /}` will match the input `[@foo @bar]`,
  resulting in the yielded value `[[[@foo]]]` and a remainder of `[@bar]`.

* The parser `{/ f=@foo { <out> :: <out> [[[f]]] } /}` is just like the
  previous example, except it is written with an explicit yield definition.

### Future direction: Destructuring bind

If in the future a "destructuring bind" form is supported, then it
will probably be introduced by an approximate/tilde (`~`).


Token Syntax
------------

The following is an in-language description of the parser tokens, as
modifications to the *Samizdat Layer 0* tokenization syntax.

```
punctuation = {/
    # ... original alternates from the base grammar ...
    "{/" |
    "/}" |
    ["&" "|" "!"]
/};
```


Tree Syntax
-----------

The following is an in-language description of the tree grammar, as
modifications to the *Samizdat Layer 0* tree syntax.

```
atom = {/
    # ... original alternates from the base grammar ...
    | parser
/};

# Note: Using the label "pex" to denote various "Parser EXpression" types.
parser = {/
    @"{/" pex=choicePex @"/}"
    { <> @["parser" pex] }
/};

choicePex = {/
    first = sequencePex
    rest = (@"|" sequencePex)*
    { <> @["choice" (listPrepend first rest)] }
/};

sequencePex = {/
    items = namePex+
    { <> @["sequence" items] }
/};

namePex = {/
    (
        name = @identifier
        @"="
        pex = lookaheadPex
        { <> @["varDef" ["name"=(tokenValue name) "value"=pex]] }
    )
|
    lookaheadPex
/};

lookaheadPex = {/
    (
        lookahead = [@"&" @"!"]
        pex = repeatPex
        { <> @[lookahead pex] }
    )
|
    repeatPex
/};

repeatPex = {/
    atom = parserAtom
    (
        repeat = [@"?" @"*" @"+"]
        { <> @[repeat atom] }
    |
        { <> atom }
    )
/};

parserAtom = {/
    varRef
|
    parserString
|
    parserToken
|
    parserSet
|
    @"."
|
    @"()"
|
    @"{"

    yieldDef = (
        y = yieldDef
        @"::"
        { <> ["yieldDef" = y] }
    |
        @"::"?
        { <> [=] }
    )

    body = programBody
    @"}"

    { <> @["function" (mapAdd yieldDef body)] }
/};

parserSet = {/
    @"["

    type = (
        @"!" { <> "setComplement" }
    |
        { <> "set" }
    )

    terminals = (parserString* | parserToken*)

    @"]"
    {
        <> @[type terminals]
    }
/};

parserString = {/
    str = @string
    { <> @["chars" str] }
/};

parserToken = {/
    @"@"
    type = (@identifier | @string)
    { <> @["token" (tokenValue type)] }
/};
```


Example
-------

The classic "four function calculator" as an example. In this case, it is
done as a unified tokenizer / tree parser. For simplicity, we don't bother
with whitespace-related rules.

Note: This example doesn't use all of the syntactic forms mentioned above.

```
digit = {/
    ch=["0123456789"]
    {
        <> isub (intFromString ch) (intFromString "0")
    }
/};

number = {/
    digits=digit+
    {
        <> listReduce 0 digits
            { result digit :: <> iadd digit (imul result 10) }
    }
/};

atom = {/
    number
|
    "(" ex=addExpression ")"
    { <> ex }
/};

addExpression = {/
    ex1=mulExpression op=addOp ex2=addExpression
    { <> op ex1 ex2 }
/};

addOp = {/
    "+" { <> iadd }
|
    "-" { <> isub }
/};

mulExpression = {/
    ex1=unaryExpression op=mulOp ex2=mulExpression
    { <> op ex1 ex2 }
/};

mulOp = {/
    "*" { <> imul }
|
    "/" { <> idiv }
/};

unaryExpression = {/
    op=unaryOp
    ex=unaryExpression
    { <> op ex }
|
    atom
/};

unaryOp = {/
    "-" { <> ineg }
/};

main = {/
    (
        ex = addExpression
        "\n"
        {
            io0Note (format "%q" ex)
            # Explicit yield here to indicate successful parsing.
            <> null
        }
    )*
/};
```

Example Translation to Samizdat Layer 0
---------------------------------------

```
digit = pegMakeMainSequence
    (pegMakeCharSet "0123456789")
    (pegMakeCode { ch ::
        <> isub (intFromString ch) (intFromString "0")
    });

number = pegMakeMainSequence
    (pegMakePlus digit)
    (pegMakeCode { digits ::
        <> listReduce 0 digits
            { result digit :: <> iadd digit (imul result 10) }
    });

atom = pegMakeChoice
    number
    (pegMakeMainSequence
        (pegMakeChar "(")
        addExpression
        (pegMakeChar ")")
        (pegMakeCode { . ex . :: <> ex }));

addExpression = pegMakeMainSequence
    mulExpression
    addOp
    addExpression
    (pegMakeCode { ex1 op ex2 :: <> op ex1 ex2 });

addOp = pegMakeChoice
    (pegMakeSequence
        (pegMakeChar "+")
        (pegMakeCode { <> iadd }))
    (pegMakeSequence
        (pegMakeChar "-")
        (pegMakeCode { <> isub }));

mulExpression = pegMakeMainSequence
    unaryExpression
    mulOp
    mulExpression
    (pegMakeCode { ex1 op ex2 :: <> op ex1 ex2 });

mulOp = pegMakeChoice
    (pegMakeSequence
        (pegMakeChar "*")
        (pegMakeCode { <> imul }))
    (pegMakeSequence
        (pegMakeChar "/")
        (pegMakeCode { <> idiv }));

unaryExpression = pegMakeMainSequence
    unaryOp
    unaryExpression
    (pegMakeCode { op ex :: <> op ex });

unaryOp = pegMakeMainSequence
    (pegMakeChar "-")
    (pegMakeCode { <> ineg });

main = pegMakeStar
    (pegMakeMainSequence
        addExpression
        (pegMakeChar "\n")
        (pegMakeCode { ex . ::
            io0Note (format "%q" ex);
            <> null
        }));
```
