Samizdat Language Guide
=======================

Appendix: *Samizdat Layer 1* Token Grammar
------------------------------------------

The following is a nearly complete token grammar for *Samizdat Layer 1*,
written in *Samizdat Layer 1*, with commentary calling out the parts
that are needed specifically for *Layer 1*. Anything left unmarked is
needed for *Layer 0*.

A program is tokenized by matching the `file` rule, resulting in a
list of all the tokens. Tokenization errors are represented in the
result as tokens of type `error`.

```
# A map from strings to their corresponding keywords, one mapping for each
# identifier-like keyword.
#
# Note: Additional keywords are defined in *Layer 2*.
def KEYWORDS = [def: @def, fn: @fn, return: @return];

# Note: The yielded result is always ignored.
def tokWhitespace = {/
    [" " "\n"]
|
    "#" [! "\n"]* "\n"
/};

def tokPunctuation = {/
    "@@" | "::" | ".." | "<>" |
    "{/" | "/}" |                 # These are only needed in *Layer 1*.
    ["&@:.,=-+?;*<>{}()[]" "|!"]  # The latter string is just for *Layer 1*.
/};

# Note: Additional rules for string character parsing are defined in *Layer 2*.
def tokStringPart = {/
    (
        chars = [! "\\" "\"" "\n"]+
        { <> stringFromTokenList(chars) }
    )
|
    # This is the rule that ignores spaces after newlines.
    "\n"
    " "*
    { <> "\n" }
|
    (
        "\\"
        (
            "\\" { <> "\\" } |
            "\"" { <> "\"" } |
            "n"  { <> "\n" } |
            "r"  { <> "\r" } |
            "t"  { <> "\t" } |
            "0"  { <> "\0" }
        )
    )
/};

def tokString = {/
    "\""
    chars = tokStringPart*
    "\""
    { <> @[string: stringAdd(chars*)] }
/};

def tokIdentifier = {/
    first = ["_" "a".."z" "A".."Z"]
    rest = ["_" "a".."z" "A".."Z" "0".."9"]*
    {
        def string = stringFromTokenList([first, rest*]);
        <> ifValueOr { <> mapGet(KEYWORDS, string) }
            { <> @[identifier: string] }
    }
/};

def tokQuotedIdentifier = {/
    "\\"
    s = tokString
    { <> @[identifier: tokenValue(s)] }
/};

def tokInt = {/
    digits = (
        ch = ["0".."9"]
        { <> intFromDigitChar(ch) }
    )+

    {
        def value = doReduce1(digits, 0)
            { digit, result :: <> iadd(digit, imul(result, 10)) };
        <> @[int: value]
    }
/};

def error = {/
    badCh = .
    [! "\n"]*
    { <> @[error: ... tokenType(badCh) ...] }
/};

def tokToken = {/
    tokInt | tokPunctuation | tokString |
    tokIdentifier | tokQuotedIdentifier |
    tokError
/};

def tokFile = {/
    tokens = (whitespace* token)*
    whitespace*
    { <> tokens }
/};
```
