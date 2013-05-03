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
    integer2 | integer |
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

integer2 ::= "0x" hexDigit+ ;
# result: [:@integer <intlet>:]

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
