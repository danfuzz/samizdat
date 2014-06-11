Samizdat Language Guide
=======================

Appendix: Layer 1 Token Grammar
-------------------------------

The following is the complete token grammar for Samizdat Layer 1,
written in Samizdat Layer 1, with commentary calling out the parts
that are needed specifically for Layer 1 as well as how Layer 2 hooks in.
Anything left unmarked is also needed for Layer 0.

A program is tokenized by matching the `file` rule, resulting in a
list of all the tokens. Tokenization errors are represented in the
result as tokens of type `error`.

```
import core.Generator;
import core.Lang0Node :: intFromDigits;
import core.Peg;


##
## Private Definitions
##

## Map of all the keywords, from their string name to valueless tokens. These
## are (to a first approximation) operators whose spellings match the
## tokenization syntax of identifiers.
def KEYWORDS = $Generator::collectAsMap(
    $Generator::makeFilterGenerator([
        "break", "continue", "def", "export", "fn", "import", "return",
        "var", "yield",
        ## Layer 2 defines additional keywords here.
        []*])
        { name -> {(name): @(@@(name))} });


##
## Layer 0 Rules
##
## This section consists of the definitions required to implement Layer 0,
## with comments indicating the "hooks" for higher layers.
##

## Forward declarations required for layer 2. These are all add-ons to
## layer 0 or 1 rules, used to expand the syntactic possibilities of the
## indicated base forms.
def tokInt2;
def tokMultilineComment;
def tokStringPart2;

## Parses any amount of whitespace, comments, and directives (including
## nothing at all). **Note:** The yielded result is always ignored.
def tokWhitespace = {:
    ## The lookahead here is to avoid the bulk of this rule if there's
    ## no chance we're actually looking at whitespace.
    &["# \n"]

    (
        [" \n"]+
    |
        "#" ["#!="] [! "\n"]*
    |
        ## Introduced in Layer 2.
        &"#:" ## Avoid calling the rule unless we know it will match.
        %tokMultilineComment
    )+
:};

## Parses punctuation and operators.
##
## **Note:** This rule is expanded significantly in Layer 2.
def tokPunctuation = {:
    ## The lookahead here is done to avoid bothering with the choice
    ## expression, except when we have a definite match. The second
    ## string in the lookahead calls out the characters that are only
    ## needed in Layer 1. Yet more characters are included in Layer 2.
    &["@:.,=-+?;*/<>{}()[]" "&|!%"]

    (
        ## Layer 2 introduces additional definitions here.
    ## |
        "->" | ":=" | "::" | ".." | "@@"
    |
        ## These are introduced in Layer 1.
        "{:" | ":}"
    ## |
        ## Layer 2 introduces additional definitions here.
    |
        ## Single-character punctuation / operator.
        .
    )
:};

## Parses a single decimal digit (or spacer), returning its value.
def tokDecDigit = {:
    ch = ["_" "0".."9"]
:};

## Parses an integer literal.
def tokInt = {:
    ## Note: Introduced in Layer 2.
    &"0" ## Avoid calling the rule unless we know it will match.
    %tokInt2
|
    digits = tokDecDigit+
    { @int(intFromDigits(10, digits)) }
:};

## Parses a run of regular characters or an escape / special sequence,
## inside a quoted string.
##
## **Note:** Additional rules for string character parsing are defined
## in Layer 2.
def tokStringPart = {:
    (
        chars = [! "\\" "\"" "\n"]+
        { $Peg::stringFromTokenList(chars) }
    )
|
    ## This is the rule that ignores spaces after newlines.
    "\n"
    " "*
    { "\n" }
|
    "\\"
    (
        "\\" { "\\" } |
        "\"" { "\"" } |
        "n"  { "\n" } |
        "r"  { "\r" } |
        "t"  { "\t" } |
        "0"  { "\0" }
    )
|
    ## Introduced in Layer 2.
    %tokStringPart2
:};

# Parses a quoted string.
def tokString = {:
    "\""
    parts = tokStringPart*

    (
        "\""
        { @string(cat("", parts*)) }
    |
        { @error("Unterminated string literal.") }
    )
:};

## Parses an identifier (in the usual form). This also parses keywords.
def tokIdentifier = {:
    one = ["_" "$" "a".."z" "A".."Z"]
    rest = ["_" "$" "a".."z" "A".."Z" "0".."9"]*

    {
        def string = $Peg::stringFromTokenList([one, rest*]);
        ifValueOr { get(KEYWORDS, string) }
            { @identifier(string) }
    }
:};

## Parses the quoted-string identifier form.
def tokQuotedIdentifier = {:
    "\\"
    s = tokString

    { @identifier(dataOf(s)) }
:};

## "Parses" an unrecognized character. This also consumes any further
## characters on the same line, in an attempt to resynch the input.
def tokError = {:
    badCh = .
    [! "\n"]*

    {
        def msg = cat("Unrecognized character: ", get_typeName(badCh));
        @error(msg)
    }
:};

## Parses an arbitrary token or error.
tokToken := {:
    tokString | tokIdentifier | tokQuotedIdentifier
|
    ## This needs to be listed after the quoted identifier rule, to
    ## prevent `\"...` from being treated as a `\` token followed by
    ## a string.
    tokPunctuation
|
    ## This needs to be listed after the identifier rule, to prevent
    ## an identifier-initial `_` from triggering this rule. (This is
    ## only significant in Layer 2 and higher.)
    tokInt
|
    tokError
:};

## Parses a file of tokens, yielding a list of them.
def tokFile = {:
    tokens = (tokWhitespace? tokToken)*
    tokWhitespace?

    { tokens }
:};


##
## Layer 2 Rule Stubs
##

## In layer 2, these are all non-trivial, but here they are simply
## always-fail.

tokInt2             := {: !() :};
tokMultilineComment := {: !() :};
tokStringPart2      := {: !() :};
```
