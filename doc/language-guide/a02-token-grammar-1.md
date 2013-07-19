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
# **Note:** Additional keywords are defined in *Layer 2*.
def KEYWORDS = [def: @def, fn: @fn, return: @return];

# These are all the int digits, as a map from strings to
# digit values.
def INT_CHARS = [
    "0": 0, "1": 1, "2": 2, "3": 3, "4": 4,
    "5": 5, "6": 6, "7": 7, "8": 8, "9": 9
];

# Given a decimal digit, returns the digit value.
fn intFromDigitChar(ch) { <> mapGet(INT_CHARS, tokenType(ch)) };

# Forward declaration of `tokToken`, for use in the interpolated string
# rule. (This is only significant as of *Layer 2*.)
def tokToken = forwardFunction();

# Parses whitespace and comments. **Note:** The yielded result is always
# ignored.
def tokWhitespace = {/
    [" " "\n"]
|
    "#" [! "\n"]* "\n"
#|
    # Note: Layer 2 defines additional rules here.
/};

# Parses punctuation and operators.
#
# **Note:** This rule is expanded significantly in *Layer 2*.
def tokPunctuation = {/
    # The lookahead here is done to avoid bothering with the choice
    # expression, except when we have a definite match. The second
    # string in the lookahead calls out the characters that are only
    # needed in *Layer 1*. Yet more characters are included in *Layer 2*.
    &["&@:.,=-+?;*<>{}()[]" "/|!"]

    (
        # *Layer 2* introduces additional definitions here.
    #|
        "@@" | "::" | ".." | "<>"
    |
        # These are only needed in *Layer 1*.
        "{/" | "/}"
    |
        # Single-character punctuation / operator.
        .
    )
/};

# Parses an integer literal.
#
# **Note:** This rule is expanded significantly in *Layer 2*.
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

# Parses a run of regular characters or an escape / special sequence,
# inside a quoted string.
#
# **Note:** Additional rules for string character parsing are defined
# in *Layer 2*.
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

# Parses a quoted string.
#
# **Note:** In Layer 2, this rule is expanded to yield either a `@string`
# token or an `@interpolatedString` token (or an `@error` token).
def tokString = {/
    "\""
    chars = tokStringPart*
    "\""
    { <> @[string: stringAdd(chars*)] }
/};

# Parses an identifier (in the usual form). This also parses keywords.
def tokIdentifier = {/
    first = ["_" "a".."z" "A".."Z"]
    rest = ["_" "a".."z" "A".."Z" "0".."9"]*
    {
        def string = stringFromTokenList([first, rest*]);
        <> ifValueOr { <> mapGet(KEYWORDS, string) }
            { <> @[identifier: string] }
    }
/};

# Parses the quoted-string identifier form.
def tokQuotedIdentifier = {/
    "\\"
    s = tokString
    { <> @[identifier: tokenValue(s)] }
/};

# "Parses" an unrecognized character. This also consumes any further characters
# on the same line, in an attempt to resynch the input.
def error = {/
    badCh = .
    [! "\n"]*
    {
        def msg = stringAdd("Unrecognized character: ", tokenType(badCh));
        <> @[error: msg]
    }
/};

# Parses an arbitrary token or error.
def implToken = {/
    tokString | tokIdentifier | tokQuotedIdentifier
|
    # This needs to be listed after the quoted identifier rule, to
    # prevent `\"...` from being treated as a `\` token followed by
    # a string.
    tokPunctuation
|
    # This needs to be listed after the identifier rule, to prevent
    # an identifier-initial `_` from triggering this rule. (This is
    # only significant in *Layer 2* and higher.)
    tokInt
|
    tokError
/};
tokToken(implToken);

# Parses a file of tokens, yielding a list of them.
def tokFile = {/
    tokens = (whitespace* token)*
    whitespace*
    { <> tokens }
/};
```
