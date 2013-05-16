Samizdat Layer 0
================

Token Syntax
------------

The following is a BNF/PEG-like description of the tokens. A program
is tokenized by matching the top `file` rule, which results in a
list of all the tokens.

```
file ::= (whitespace* token)* whitespace* ;
# result: [token*]

token ::= punctuation | int | string | identifier | quotedIdentifier ;
# result: same as whichever alternate was picked.

punctuation ::=
    "@@" | "::" | "<>" | "()" |
    ["@:.=+?;*<>{}()[]"]
;
# result: a valueless highlet with the matched string as its type tag.

int ::= "-"? ["0".."9"]+ ;
# result: @["int" <int>]

string ::= "\"" ([! "\\" "\""] | ("\\" ["\\" "\"" "n" "0"]))* "\"" ;
# result: @["string" <string>]

identifier ::=
    ["_" "a".."z" "A".."Z"] ["_" "a".."z" "A".."Z" "0".."9"]* ;
# result: @["identifier" <string>]

quotedIdentifier ::= "\\" string
# result: @["identifier" (highletValue string)]

whitespace ::= [" " "\n"] | "#" [! "\n"]* "\n" ;
# result: n/a; automatically ignored.
```
