Samizdat Layer 0
================

Token Syntax
------------

The following is a BNF/PEG-like description of the tokens. A program
is tokenized by repeatedly matching the top `token` rule.

```
token ::=
    whitespace*
    (punctuation | integer | string | identifier)
    whitespace*
;
# result: same as the non-whitespace payload.

punctuation ::=
    "@@" | # result: [:@"@@":]
    "::" | # result: [:@"::":]
    "@"  | # result: [:@"@":]
    ":"  | # result: [:@":":]
    "*"  | # result: [:@"*":]
    ";"  | # result: [:@";":]
    "="  | # result: [:@"=":]
    "-"  | # result: [:@"-":]
    "?"  | # result: [:@"?":]
    "{"  | # result: [:@"{":]
    "}"  | # result: [:@"}":]
    "("  | # result: [:@"(":]
    ")"  | # result: [:@")":]
    "["  | # result: [:@"[":]
    "]"    # result: [:@"]":]
;

integer ::= ("0".."9")+ ;
# result: [:@integer <intlet>:]

string ::= "\"" (~("\\"|"\"") | ("\\" ("\\"|"\""|"n")))* "\"" ;
# result: [:@string <stringlet>:]

identifier ::=
    ("_" | "a".."z" | "A".."Z") ("_" | "a".."z" | "A".."Z" | "0".."9")* ;
# result: [:@identifier <stringlet>:]

whitespace ::= " " | "\n" | "#" (~("\n"))* "\n" ;
# result: none; automatically ignored.
```
