Samizdat Layer 0
================

Token Syntax
------------

The following is a BNF/PEG-like description of the tokens. This definition
uses syntax which is nearly identical to the parser syntax provided at the
higher layer.

A program is tokenized by matching the `file` rule, resulting in a
list of all the tokens. Tokenization errors are represented in the
result as tokens of type `"error"`.

```
# A map from strings to their corresponding keywords, one mapping for each
# identifier-like keyword.
def KEYWORDS = [def: @def, fn: @fn, return: @return];

# Note: The yielded result is always ignored.
def whitespace = {/
    [" " "\n"]
|
    "#" [! "\n"]* "\n"
/};

def punctuation = {/
    "@@" | "::" | ".." | "<>" | "()" | ["@:.,=-+?;*<>{}()[]"]
/};

def stringChar = {/
    (
        ch = [! "\\" "\""]
        { <> tokenType(ch) }
    )
|
    (
        "\\"
        (
            "\\" { <> "\\" } |
            "\"" { <> "\"" } |
            "n"  { <> "\n" } |
            "0"  { <> "\0" }
        )
    )
/};

def string = {/
    "\""
    chars = stringChar*
    "\""
    { <> @[string: stringAdd(chars*)] }
/};

def identifier = {/
    first = ["_" "a".."z" "A".."Z"]
    rest = ["_" "a".."z" "A".."Z" "0".."9"]*
    {
        def string = stringFromTokenList([first, rest*]);
        <> ifValue { <> mapGet(KEYWORDS, string) }
            { keyword :: <> keyword }
            { <> @[identifier: string] }
    }
/};

def quotedIdentifier = {/
    "\\"
    s = string
    { <> @[identifier: tokenValue(s)] }
/};

def int = {/
    digits = (
        ch = ["0".."9"]
        { <> intFromDigitChar(ch) }
    )+

    { <> ... @[int: ...] }
/};

def error = {/
    badCh = .
    [! "\n"]*
    { <> @[error: ... tokenType(badCh) ...] }
/};

def token = {/
    int | punctuation | string | identifier | quotedIdentifier | error
/};

def file = {/
    tokens=(whitespace* token)* whitespace*
    { <> tokens }
/};
```
