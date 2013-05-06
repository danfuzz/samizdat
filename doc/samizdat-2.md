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

keyword ::=
    @"if"    | # result: [:@"if":]
    @"else"  | # result: [:@"else":]
    @"fn"    | # result: [:@"fn":]
    @"while"   # result: [:@"while":]
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
    "~"    # result: [:@"~":]
;

hexInteger ::= "0x" "-"? hexDigit+ ;
# result: [:@integer <intlet>:]

hexDigit ::=
    "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" |
    "a" | "b" | "c" | "d" | "e" | "f" |
    "A" | "B" | "C" | "D" | "E" | "F" ;
# result: <intlet>

binaryInteger ::= "0b" "-"? binaryDigit+ ;
# result: [:@integer <intlet>:]

binaryDigit ::= "0" | "1" ;
# result: <intlet>

identifier2 ::= "\\" string ;
# result: [:@identifier (highletValue string):]
```


<br><br>
Tree Syntax
-----------

The following are additional tree parsing rules beyond the
*Samizdat Layer 0* parsing rules, along with overrides of underlying
rules.

TODO: Sort this all out.

```
#
# Statement rules
#

statement ::=
    ifStatement | whileStatement | functionStatement |
    varDef | expression
;
# result: <same as whatever choice matched>

ifStatement ::=
    [:@"if":] [:@"(":] expression [:@")":] function
    (@"else" (ifStatement | function))?
# result code: ifTrue expression function (if|function)?

whileStatement ::=
    [:@"while":] [:@"(":] expression [:@")":] function
# result code:
# { <break> ::
#     loop { ifTrue { <> expression } function { <break> } }
# }()

functionStatement ::=
    [:@"fn":] [:@identifier:] formals? yieldDef? [:@"{":] statement* [:@"}":]
;
# result: varDef of identifier to function.


#
# Expression rules. These are ordered by precedence, low to high.
#

expression ::= orExpression ;
# result: orExpression

orExpression ::= andExpression (([:@"||":]) andExpression)* ;
# result code: or { <> expr1 } { <> expr2 } ...

andExpression ::= compareExpression ([:@"&&":] compareExpression)* ;
# result code: and { <> expr1 } { <> expr2 } ...

compareExpression ::=
    bitExpression
    (([:@"==":] | [:@"!=":] | [:@"<":] | [:@">":] | [:@"<=":] | [:@">=":])
     bitExpression)*
;
# result code:
# {
#     e1 = expr1; e2 = expr2; ...
#     <> and { <> e1 op e2 } { <> e2 op e3 } ...
# }()

bitExpression ::=
    additiveExpression
    (([:@"<<":] | [:@">>":] | [:@"&":] | [:@"|":] | [:@"^":]) bitExpression)?
;
# result code: \"op" expr1 expr2

additiveExpression ::=
    multiplicativeExpression
    (([:@"+":] | [:@"-":]) additiveExpression)?
;
# result code: \"op" expr1 expr2

multiplicativeExpression ::=
    unaryExpression
    (([:@"*":] | [:@"/":] | [:@"%":]) multiplicativeExpression)?
;
# result code: \"op" expr1 expr2

unaryPrefixExpression ::=
    ([:@"!":] | [:@"~":] | [:@"-":])* unaryPostfixExpression
;
# result code: \"op1" (\"op2" (\"op3" ... expr))

unaryPostfixExpression ::=
    atom
    ([:@"(":] [:@")":] | [:@"[":] expression [:@"]":] ))*
;
# result code: atom() | \"[]" atom expression
# etc.

atom ::=
    varRef | intlet | [:@integer:] | stringlet | [:@string:] |
    emptyListlet | listlet | emptyMaplet | maplet |
    uniqlet | highlet | function | parenExpression ;
# result: <same as whatever choice matched>
```
