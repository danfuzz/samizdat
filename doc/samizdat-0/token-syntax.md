Samizdat Layer 0
================

Token Syntax
------------

The following is a BNF/PEG-like description of the tokens. This definition
uses syntax which is nearly identical to the parser syntax provided at the
higher layer.

A program is tokenized by matching the `file` rule, resulting in a
list of all the tokens. For simple error handling, the `fileOrError`
rule can be used instead.

```
# Note: The yielded result is always ignored.
whitespace = {/
    [" " "\n"]
|
    "#" [! "\n"]* "\n"
/};

punctuation = {/
    "@@" | "::" | "<>" | "()" | ["@:.=+?;*<>{}()[]"]
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
    { <> @["identifier" (apply stringFromTokenList first rest)] }
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

token = {/
    punctuation | int | string | identifier | quotedIdentifier
/};

file = {/
    tokens=(whitespace* token)* whitespace*
    { <> tokens }
/};

errorLine = {/
    &.
    chars=[! "\n"]*
    ("\n" | !.)
    { <> stringFromTokenList chars }
/};

fileOrError = {/
    tokens=file
    (
        errorLines=errorLine+
        { ... io0Die ... }
    )?
    { <> tokens }
/};
```
