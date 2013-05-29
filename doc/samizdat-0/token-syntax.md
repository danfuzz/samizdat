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
# Note: The yielded result is always ignored.
whitespace = {/
    [" " "\n"]
|
    "#" [! "\n"]* "\n"
/};

punctuation = {/
    "@@" | "::" | "<>" | "()" | ["@:.,=+?;*<>{}()[]"]
/};

stringChar = {/
    (
        ch = [! "\\" "\""]
        { <> tokenType ch }
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

string = {/
    "\""
    chars = stringChar*
    "\""
    { <> @["string" (apply stringAdd chars)] }
/};

identifier = {/
    first = ["_" "a".."z" "A".."Z"]
    rest = ["_" "a".."z" "A".."Z" "0".."9"]*
    { <> @["identifier" (stringFromTokenList (listPrepend first rest))] }
/};

quotedIdentifier = {/
    "\\"
    s = string
    { <> @["identifier" (tokenValue s)] }
/};

int = {/
    sign = ("-" { <> -1 } | { <> 1 })
    digits = (
        ch = ["0".."9"]
        { <> intFromDigitChar ch }
    )+

    { <> ... @["int" ...] }
/};

error = {/
    badCh = .
    [! "\n"]*
    { <> @["error" ... (tokenType badCh) ...] }
/};

token = {/
    int | punctuation | string | identifier | quotedIdentifier | error
/};

file = {/
    tokens=(whitespace* token)* whitespace*
    { <> tokens }
/};
```
