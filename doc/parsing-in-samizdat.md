Parsing in Samizdat
===================

[It's not yet clear what layer this will end up at.]

Since parsing is something so many programs have to do, Samizdat offers
language-level facilities for building parsers.

Syntax and Semantics
--------------------

The following list of parser forms is in precedence order, from
tightest to loosest.

### Parsing functions

Parsing functions are written as a parsing expression inside "parsing
braces" `{/ ... /}`. Just as regular braces enclose an anonymous
function / closure, parsing braces enclose an anonymous parsing
function / closure.

A parsing function is a function which takes two parameters, namely a `yield`
function and an `input` state. To indicate successful parsing, it is expected
to (a) call `yield` with the result of parsing, and (b) return a replacement
`input` state meant to reflect what was consumed from the original `input`
(if anything). To indicate a parsing failure, the function is expected to
(a) call `yield` with no argument to indicate a void yield, and (b)
return void.

The input state is expected to be a list of to-be-parsed elements, such
as characters for tokenization or tokens for a tree parser.

If parsing succeeds and a value is `yield`ed, then the return value is
expected to be a list of whatever input remains (including possibly
the empty list).

The default `yield` value of a parsing function is whatever the yielded
result is from the *last* item in the list of items to match in the
function. This default can be overridden by using a filtering clause
(see below).

### Matching character sequences

The basic "terminal" in the context of a tokenizer is a character.
Within a parsing function, a double-quoted string is how to indicate
that a sequence of characters is to be matched. The result of matching
a character sequence is a valueless highlet whose type is the the matched
string.

For example, the parser `{/ "foo" /}` will match the string `"foobar"`,
resulting in the yielded value `@foo` and a remainder of `"bar"`.

### Matching tokens of a particular type

Tokens are generally represented as highlets, with the highlet type
indicating the type of the token, and the highlet value optionally
indicating a data payload. Tokens are the basic terminals in the
context of tree parsing.

Within a parsing function, a valueless highlet is how to indicate
that a token of the indicated type is to be matched. The result of
matching is the full original token, including whatever data payload
it happened to have.

For example, the parser `{/ @foo /}` will match the token list
`[@foo @bar]`, resulting in the yielded value `@foo` and a
remainder of `[@bar]`.

### Matching an arbitrary terminal item

To match an arbitrary terminal item (character or token), use a
plain dot (`.`).

For example, the parser `{/ . . . /}` matches an arbitrary
sequence of three terminals. (See "Matching sequences of items", below.)

### Matching the end-of-input

To match the end of input, use a not-dot (`!.`). This only ever matches
when there is no input available. When matched, this yields the value
`null`.

For example, the parser `{/ "foo" !. /}` will match the string `"foo"` but
only if it's at the end of input.

### Matching character or token sets

To match a set of characters (for tokenization) or tokens (for tree parsing),
place them between "set brackets" `[/ ... /]`. Characters of a character
set can be combined into a single string literal for convenience, e.g.
`[/"x" "y"/]` and `[/"xy"/]` are equivalent.

Note that this syntax is valid at the same level as parsing functions and
other atoms, and *not* limited to appearing within a parsing function's
pattern.

To match any terminal other than items from a particular set, precede the
set contents with a complement (`~`), *inside* the set brackets. Note that
there is a difference between a complemented set, which can consume one
input item, and a lookahead failure of a set (`!&[/ ... /]`, see below),
which never consumes input.

For example:

* The parser `[/"blort"/]` will match any of the characters
  `b` `l` `o` `r` or `t`.

* The parser `[/~ "\n"/]` will match any character but a newline.

* The parser `[/@foo @bar/]` will match either
  a `@foo` or a `@bar` token.

* The parser `[/~ @foo @bar/]` will match any token but a
  `@foo` or a `@bar` token.

### Matching using other parser functions

A parser function can delegate to another parser function by naming
that other parser function (as a variable reference) or including it
in-line as a function literal (either a parser function or a regular
function). The result of parsing is identical to whatever that other
parsing function returns.

For example:

* The parser `{/ foo /}` will match whatever `foo` matches,
  assuming that `foo` is a properly-written parser function. The yielded
  result will be the same as the `foo` parser's yield.

* The parser `{/ {/ foo /} /}` will also match whatever `foo` matches.
  The yielded result will be the same as the `foo` parser's yield.

### Grouping

To override the default precedence of the syntax, use the parser
function literal form, per "Matching using other parser functions,"
immediately above.

### Optionally matching an item

A parser item (character, token, or function) may be made optional by
following it with a question mark (`?`). So marked, parsing this item
always succeeds and results in a list. If the underlying item was
indeed matched, then the result is a single-element list containing
the item's parsing result. If the underlying item was not matched,
then the result is an empty list.

### Matching zero or more occurrences of an item

To match a sequence of zero or more occurrences of an item, it can be
followed by a star (`*`). So marked, parsing this item always
succeeds (because zero items is a possibility), and results in a list
consisting of all the parsed results from matching the item, in order.

### Matching one or more occurrences of an item

To match a sequence of one or more occurrences of an item, it can be
followed by a plus (`+`). So marked, if successful, the result is
just like the result of a `*`-marked rule. However, this will fail in
cases where there is not even one resulting match.

### Matching sequences of items

To match a sequence of items, the items are simply listed in order. Per
the overall definition of parser function semantics, the result of matching
a sequence of items is whatever the yield is from the *last* item listed.

For example, the parser `{/ @foo @bar /}` will match the token
list `[@foo @bar @baz]`, resulting in the yielded value
`@bar` and a remainder of `[@baz]`.

### Lookahead success

To match an item without "consuming" it from the input, the item can
be preceded by an ampersand (`&`). When matched, the yielded value
of a lookahead is identical to what the underlying item yielded.

For example, the parser `{/ &"foobar" "foo" /}` will match the string
`"foobar"`, resulting in the yielded value `@foo` and a
remainder of `"bar"`. However, the same parser will *fail* to match
`"foobaz"` because the lookahead `&"foobar"` will fail.

### Lookahead failure

To make sure an item would *not* match, the item can be preceded by an
not-ampersand (`!&`). As with lookahead success, this will never "consume"
any input. When successful (that is, when lookahead fails), a lookahead
failure yields `null`.

For example, the parser `{/ !&"foobaz" "foo" /}` will match the string
`"foobar"`, resulting in the yielded value `@foo` and a remainder of
`"bar"`. However, the same parser will *fail* to match `"foobaz"` because
the lookahead `!&"foobaz"` will fail (because a `"foobaz"` lookahead *succeeds*
match).

### Filtering results

To override the default result of parsing, arbitrary code may be
specified. To do so, introduce it with `::`. This may be followed
by any number of `;`-separated statements, optionally ending with a standard
yield expression (`<> ...`). In order to refer to the values
that have matched, the items in question may be preceded by an assignment of
the form `name =` (where `name` is an arbitrary identifier). Such
an assignment must occur before any other prefix (such as `&`).

The filtering form, when used, must be the last element of a parsing
function.

Note that, if the statements fail to yield a value, then the parse is
considered to have failed.

For example, the parser `{/ "(" ex=expression ")" :: <> ex /}` will
match any input that starts with an open parenthesis, followed by a
match of the `expression` function (presumed here to be another parser
function), followed by a close parenthesis. The result of parsing will
be the same as the result from the `expression` function.

### Matching one of multiple alternates

To match any one of a series of alternates, the alternates are listed in
priority sequence (first one listed gets first "dibs", etc.), separated
by vertical bars (`|`). The result of matching is the same as the result
of whichever alternate was matched.

This expression is valid at the layer of Samizdat expressions, and
not valid *within* a parser function pattern declaration. Note how
the example below is constructed.

For example, the parser `{/ "f" /} | {/ "foo" /}` will match the string
`"foobar"`, resulting in the yielded value `@f` and a remainder of
`"oobar"`. Note that because of the prioritized ordering, the second
alternate could never get picked in this case.


Token Syntax
------------

The following is an in-language description of the parser tokens, as
modifications to the *Samizdat Layer 0* tokenization syntax.

```
punctuation =
    # ... original alternates from the base grammar ...
    {/ "{/" :: <> @"{/" /} |
    {/ "/}" :: <> @"/}" /} |
    {/ "[/" :: <> @"[/" /} |
    {/ "/]" :: <> @"/]" /} |
    {/ "!." :: <> @"!." /} |
    {/ "!&" :: <> @"!&" /} |
    {/ "&"  :: <> @"&"  /} |
    {/ "|"  :: <> @"|"  /} |
    {/ "~"  :: <> @"~"  /}
;
```


Tree Syntax
-----------

The following is an in-language description of the tree grammar, as
modifications to the *Samizdat Layer 0* tree syntax.

```
choiceExpression = {/
    first = atom
    rest = {/ @"|" atom /}*
    ::
    <> apply makeCall @["varRef" "choiceCombinator"] (listPrepend first rest)
/};

atom =
    # ... original alternates from the base grammar ...
    parserSetFunction |
    parserFunction
;

parserSetFunction = {/
    @"[/"
    complement = @"~"?
    terminals = {/ @string* | @identifier* /}
    @"/]"
    ::
    type = ifTrue { <> eq complement [] }
        { <> "parserSet" }
        { <> "parserNotSet" };
    <> @[ type terminals ]
/};

parserFunction = {/
    @"{/" program=parserProgram @"/}"
    ::
    <> program
/};

parserProgram = {/
    decls = {/
        {/ decls=parserDeclarations @"::" :: <> decls /}
        |
        {/ :: <> [=] /} # Empty declarations are valid.
    /}
    body = programBody
    ::
    <> @[ "parserFunction" (mapAdd decls body) ]
/};

parserDeclarations = {/
    pattern = parserItem*
    yield = {/
        {/ yield = yieldDef :: <> ["yieldDef"=yield] /}
        |
        {/ :: <> [=] /} # No yieldDef.
    ::
    <> mapAdd ["pattern"=pattern] yield
/};

parserItem = {/
    name = {/ identifier @"=" /}?
    lookahead = {/ @"&" | @"!&" /}?
    atom = parserAtom
    repeat = {/ @"*" | @"?" /}
    ::
    <> # TODO: Stuff goes here.
/};

parserAtom =
    {/ s=string :: <> @[ "matchChars" s ] /}
    |
    {/ @"@[" s=string @"]" :: <> @[ "matchToken" s] /}
    |
    parserFunction
    |
    function
    |
    varRef
    |
    {/ "." :: <> @[ "matchAnyTerminal" ] /}
    |
    {/ "!." :: <> @[ "matchEof" ] /}
;
```


Example
-------

The classic "four function calculator" as an example. In this case, it is
done as a unified tokenizer / tree parser. For simplicity, we don't bother
with whitespace-related rules.

Note: This example doesn't use all of the syntactic forms mentioned above.

```
digit = {/
    ch=[/"0123456789"/]
    ::
    <> isub (intFromString ch) (intFromString "0")
/};

number = {/
    digits=digit+
    ::
    <> listReduce 0 digits { result digit :: <> iadd digit (imul result 10) }
/};

atom =
    number
    |
    {/ "(" ex=addExpression ")" :: <> ex /}
;

addExpression = {/
    ex1=mulExpression op=addOp ex2=addExpression
    ::
    <> op ex1 ex2
/};

addOp =
    {/ "+" :: <> iadd /}
    |
    {/ "-" :: <> isub /}
;

mulExpression = {/
    ex1=unaryExpression op=mulOp ex2=mulExpression
    ::
    <> op ex1 ex2
/};

mulOp =
    {/ "*" :: <> imul /}
    |
    {/ "/" :: <> idiv /}
;

unaryExpression =
    {/ op=unaryOp ex=unaryExpression :: <> op ex /}
    |
    atom
;

unaryOp = {/
    "-" :: <> ineg
/}

main = {/
    {/
        ex=addExpression "\n"
        ::
        io0Note (format "%q" ex);
        # Explicit yield here is so that parsing is considered successful.
        <> null
    /}*
/};
```

Example Translation to Samizdat-0
---------------------------------

Note: The `yieldFilter` definitions would need to be refactored in order
to make the translations "hygenic" (that is, avoid having them inadvertently
bind to parser-expansion-internal variables). Basically, this:

```
rule = { yield input ::
    yieldFilter = { ... filter ... }
    ... rest of rule ...
};

=>

rule = { yieldFilter ::
    <> { yield input :: ... rest of rule ... }
}
{ ... filter ... };
```

Assumption: `yield` is only called on a successful parse.

```
digit = { yield input ::
    yieldFilter = { ch ::
        <> isub (integerFromString ch) (integerFromString "0")
    };

    <> parseCharSet yieldFilter input "0123456789"
};

number = { yield input ::
    yieldFilter = { digits ::
        <> listReduce 0 digits
            { result digit :: <> iadd digit (imul result 10) }
    };

    <> parsePlus yieldFilter input digit
};

atom = { yield input ::
    yieldFilter = { value ::
        <> apply { . ex . :: <> ex } value
    };

    <> parseOr yield input
        number
        { yield input ::
            <> parseSeq yieldFilter input
                { yield input :: <> parseChars yield input "(" }
                addExpression
                { yield input :: <> parseChars yield input ")" }
        }
};

addExpression = { yield input ::
    yieldFilter = { value ::
        <> apply { ex1 op ex2 :: <> op ex1 ex2 } value
    };

    <> parseSeq yieldFilter input mulExpression addOp addExpression
};

addOp = { yield input ::
    yieldFilter1 = { value :: <> iadd };
    yieldFilter2 = { value :: <> isub };

    <> parseOr yield input
        { yield input :: <> parseChars yieldFilter1 input "+" }
        { yield input :: <> parseChars yieldFilter2 input "-" }
};

mulExpression = { yield input ::
    yieldFilter = { value ::
        <> apply { ex1 op ex2 :: <> op ex1 ex2 } value
    };

    <> parseSeq yieldFilter input unaryExpression mulOp mulExpression
};

addOp = { yield input ::
    yieldFilter1 = { value :: <> imul };
    yieldFilter2 = { value :: <> idiv };

    <> parseOr yield input
        { yield input :: <> parseChars yieldFilter1 input "*" }
        { yield input :: <> parseChars yieldFilter2 input "/" }
};

unaryExpression = { yield input ::
    yieldFilter = { value ::
        <> apply { op ex :: <> op ex } value
    };

    <> parseOr yield input
        { yield input ::
            <> parseSeq yieldFilter input unaryOp unaryExpression
        }
        atom
};

unaryOp = { yield input ::
    yieldFilter = { value :: <> ineg };

    <> parseChars yieldFilter input "-"
};

main = { yield input ::
    yieldFilter = { value ::
        <> apply { ex . :: io0Note (format "%q" ex) } value
    };

    <> parseStar yield input
        { yield input ::
            <> parseSeq yieldFilter input
                addExpression
                { yield input :: <> parseChars yield input "\n" }
        }
};
```
