Samizdat Layer 2 Specification
==============================

This document is the specification for the *Samizdat Layer 2* language,
presented as differences from *Samizdat Layer 0*.

(Note: The *Samizdat Layer 1* language is identical to the *Samizdat Layer 0*
language.)


<br><br>
Token Syntax
------------

The following are additional tokenization rules beyond the
*Samizdat Layer 0* tokenization rules, along with overrides of underlying
rules.

```
token ::= punctuation2 | punctuation |
    integer | hexInteger | binaryInteger |
    string |
    identifier2 | identifier
;

punctuation2 ::=
    "==" | # result: [:@"==":]
    "!=" | # result: [:@"!=":]
    "<=" | # result: [:@"<=":]
    ">=" | # result: [:@">=":]
    "<<" | # result: [:@"<<":]
    ">>" | # result: [:@">>":]
    "&&" | # result: [:@"&&":]
    "||" | # result: [:@"||":]
    "&"  | # result: [:@"&":]
    "|"  | # result: [:@"|":]
    "^"  | # result: [:@"^":]
    "+"  | # result: [:@"+":]
    "/"  | # result: [:@"/":]
    "%"  | # result: [:@"%":]
    "!"  | # result: [:@"!":]
    "~"  | # result: [:@"~":]
;

hexInteger ::= "0x" "-"? hexDigit+ ;
# result: [:@integer <intlet>:]

hexDigit ::=
    "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" |
    "a" | "b" | "c" | "d" | "e" | "f" |
    "A" | "B" | "C" | "D" | "E" | "F" | "_" ;
# result: <intlet> of digit value or -1 for "_"
# Note: "_" is ignored (useful for readability).

binaryInteger ::= "0b" "-"? binaryDigit+ ;
# result: [:@integer <intlet>:]
# Note: "_" is ignored (useful for readability).

binaryDigit ::= "0" | "1" | "_" ;
# result: <intlet> of digit value or -1 for "_"
# Note: "_" is ignored (useful for readability).

identifier2 ::= "\\" string ;
# result: [:@identifier <string.value>:]
```


<br><br>
Tree Syntax
-----------

The following are additional tree parsing rules beyond the
*Samizdat Layer 0* parsing rules, along with overrides of underlying
rules.

TODO: Sort this all out.

```
# These are ordered by precedence, low to high.

orExpression ::= (expr (@"||"))* expr ;

andExpression ::= (expr (@"&&"))* expr ;

compareExpression ::=
    (expr (@"==" | @"!=" | @"<" | @">" | @"<=" | @">="))* expr ;

bitExpression ::= (expr (@"<<" | @">>" | @"&" | @"|" | @"^"))* expr ;

addExpression ::= (expr (@"+" | @"-"))* expr ;

multiplyExpression ::= (expr (@"*" | @"/" | @"%"))* expr ;

unaryExpression ::= (@"!" | @"~")* expr ;

atom ::=
    varRef | intlet | [:@integer:] | stringlet | [:@string:] |
    emptyListlet | listlet | emptyMaplet | maplet |
    uniqlet | highlet | function | parenExpression ;
# result: <same as whatever was parsed>
```
