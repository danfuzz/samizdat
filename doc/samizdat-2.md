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
    keyword | identifier2 | identifier
;

keyword ::=
    "break"    | # result: [:@break:]
    "continue" | # result: [:@continue:]
    "if"       | # result: [:@if]
    "else"     | # result: [:@else:]
    "fn"       | # result: [:@fn:]
    "return"   | # result: [:@return:]
    "while"      # result: [:@while:]
;

punctuation2 ::=
    "==" | # result: [:"==":]
    "!=" | # result: [:"!=":]
    "<=" | # result: [:"<=":]
    ">=" | # result: [:">=":]
    "<<" | # result: [:"<<":]
    ">>" | # result: [:">>":]
    "&&" | # result: [:"&&":]
    "||" | # result: [:"||":]
    "&"  | # result: [:"&":]
    "|"  | # result: [:"|":]
    "^"  | # result: [:"^":]
    "+"  | # result: [:"+":]
    "/"  | # result: [:"/":]
    "%"  | # result: [:"%":]
    "!"  | # result: [:"!":]
    "~"    # result: [:"~":]
;

hexInteger ::= "0x" "-"? hexDigit+ ;
# result: [:@integer <integer>:]

hexDigit ::=
    "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" |
    "a" | "b" | "c" | "d" | "e" | "f" |
    "A" | "B" | "C" | "D" | "E" | "F" ;
# result: <integer>

binaryInteger ::= "0b" "-"? binaryDigit+ ;
# result: [:@integer <integer>:]

binaryDigit ::= "0" | "1" ;
# result: <integer>

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
    breakStatement | continueStatement | ifStatement | functionStatement |
    returnStatement | whileStatement |
    varDef | expression
;
# result: <same as whatever choice matched>

breakStatement ::= [:@break:] ([:"<":] identifier [:">";])? ;
# result code: break() | \"break-<identifier>"()

continueStatement ::= [:@continue:] ([:"<":] identifier [:">";])? ;
# result code: continue() | \"continue-<identifier>"()

ifStatement ::=
    [:@if:] [:"(":] expression [:")":] function
    ([:@else:] (ifStatement | function))?
# result code: ifTrue expression function (if|function)?

functionStatement ::=
    [:@fn:] [:@identifier:] formals? [:"{":] programBody [:"}":]
;
# result code: identifier = { formals? <return> ::
#                  \"return-<identifier>" = return;
#                  programBody
#              }

returnStatement ::= [:@return:] ([:"<":] identifier [:">";])? expression? ;
# result code: return expression? | \"return-<identifier>" expression?

whileStatement ::=
    [:@while:] [:@identifier:]? [:"(":] expression [:")":] function
# result code:
# { <break> ::
#     (\"break-<identifier>" = break;)?
#     loop { <continue> ::
#         (\"continue-<identifier>" = continue;)?
#         ifTrue { <> expression } function { <break> }
#     }
# }()


#
# Expression rules. These are ordered by precedence, low to high.
#

expression ::= orExpression ;
# result: orExpression

orExpression ::= andExpression (([:"||":]) andExpression)* ;
# result: makeCall [:@varRef \"or":]
#             (makeThunk expr1) (makeThunk expr2) ...

andExpression ::= compareExpression ([:"&&":] compareExpression)* ;
# result: makeCall [:@varRef \"and":]
#             (makeThunk expr1) (makeThunk expr2) ...

compareExpression ::=
    bitExpression
    (([:"==":] | [:"!=":] | [:"<":] | [:">":] | [:"<=":] | [:">=":])
     bitExpression)*
;
# result: makeCall [:@varRef @orderChain:]
#             (makeThunk expr1) [:@varRef "<op1>":]
#             (makeThunk expr2) [:@varRef "<op2>":]
#             ...;
# Note: The orderChain function is defined to call the indicated ops with
# pairs of values derived from the thunks, guaranteeing that each thunk
# is only called once, and that it properly short-circuits.

bitExpression ::=
    additiveExpression
    (([:"<<":] | [:">>":] | [:"&":] | [:"|":] | [:"^":]) bitExpression)?
;
# result: makeCall [:@varRef "<op>":] expr1 expr2

additiveExpression ::=
    multiplicativeExpression
    (([:"+":] | [:"-":]) additiveExpression)?
;
# result: makeCall [:@varRef "<op>":] expr1 expr2

multiplicativeExpression ::=
    unaryExpression
    (([:"*":] | [:"/":] | [:"%":]) multiplicativeExpression)?
;
# result: makeCall [:@varRef "<op>":] expr1 expr2

unaryPrefixExpression ::=
    ([:"!":] | [:"~":] | [:"-":])* unaryPostfixExpression
;
# result: ... (makeCall [:@varRef "<op>":] (makeCall [:@varRef "<op>":] expr))
#         (etc.)

unaryPostfixExpression ::=
    atom
    ([:"(":] [:")":] | [:"[":] expression [:"]":] ))*
;
# result: ... (makeCall [:@varRef "[]":] (makeCall atom) expression) ...
#         (etc.)

atom ::=
    varRef | integer | string |
    emptyList | list | emptyMap | map |
    uniqlet | highlet | function | parenExpression ;
# result: <same as whatever choice matched>

# Makes a thunk (function node) out of an expression node. The
# result when evaluated is a no-args function which computes and returns
# the indicated expression.
makeThunk = { expression ::
    <> [:@function [@statements=[] @yield=expression]]
};
```
