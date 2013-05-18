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
    int | hexInt | binaryInt |
    string | keyword | identifier
;
# Note: The punctuation2 rule intentionally gets listed before the
# int rule, so that in this layer `-<digit>` gets interpreted as two
# tokens.

keyword ::=
    "break"    |
    "continue" |
    "else"     |
    "fn"       |
    "if"       |
    "return"   |
    "while"
;

punctuation2 ::=
    "==" | "!=" | "<=" | ">=" | "<<" | ">>" | "&&" | "||" |
    ["&|^+-/%!~"]
;

hexInt ::= "0x" hexDigit+ ;
# result: @["int" <int>]

hexDigit ::= ["0".."9" "a".."f" "A".."F"] ;
# result: <int>

binaryInt ::= "0b" binaryDigit+ ;
# result: @["int" <int>]

binaryDigit ::= ["01"] ;
# result: <int>
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

breakStatement ::= @break (@"<" identifier @">")? ;
# result code: break() | \"break-<identifier>"()

continueStatement ::= @continue (@"<" identifier @">"])? ;
# result code: continue() | \"continue-<identifier>"()

ifStatement ::=
    @if @"(" expression @")" function
    (@else (ifStatement | function))?
# result code: ifTrue expression function (if|function)?

functionStatement ::=
    @fn @identifier formals? @"{" programBody @"}"
;
# result code: identifier = { formals? <return> ::
#                  \"return-<identifier>" = return;
#                  programBody
#              }

returnStatement ::= @return (@"<" identifier @">")? expression? ;
# result code: return expression? | \"return-<identifier>" expression?

whileStatement ::=
    @while @identifier? @"(" expression @")" function
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

orExpression ::= andExpression ((@"||") andExpression)* ;
# result: makeCall @["varRef" \"or"]
#             (makeThunk expr1) (makeThunk expr2) ...

andExpression ::= compareExpression (@"&&" compareExpression)* ;
# result: makeCall @["varRef" \"and"]
#             (makeThunk expr1) (makeThunk expr2) ...

compareExpression ::=
    bitExpression
    ((@"==" | @"!=" | @"<" | @">" | @"<=" | @">=")
     bitExpression)*
;
# result: makeCall @["varRef" "orderChain"]
#             (makeThunk expr1) @["varRef" "<op1>"]
#             (makeThunk expr2) @["varRef" "<op2>"]
#             ...;
# Note: The orderChain function is defined to call the indicated ops with
# pairs of values derived from the thunks, guaranteeing that each thunk
# is only called once, and that it properly short-circuits.

bitExpression ::=
    additiveExpression
    ((@"<<" | @">>" | @"&" | @"|" | @"^") bitExpression)?
;
# result: makeCall @["varRef" "<op>"] expr1 expr2

additiveExpression ::=
    multiplicativeExpression
    ((@"+" | @"-") additiveExpression)?
;
# result: makeCall @["varRef" "<op>"] expr1 expr2

multiplicativeExpression ::=
    unaryExpression
    ((@"*" | @"/" | @"%") multiplicativeExpression)?
;
# result: makeCall @["varRef" "<op>"] expr1 expr2

unaryPrefixExpression ::=
    (@"!" | @"~" | @"-")* unaryPostfixExpression
;
# result: ... (makeCall @["varRef" "<op>"] (makeCall @["varRef" "<op>"] expr))
#         (etc.)

unaryPostfixExpression ::=
    atom
    (@"()" | @"[" expression @"]" ))*
;
# result: ... (makeCall @["varRef" "[]"] (makeCall atom) expression) ...
#         (etc.)

atom ::=
    varRef | int | string |
    emptyList | list | emptyMap | map |
    uniqlet | token | function | parenExpression ;
# result: <same as whatever choice matched>

# Makes a thunk (function node) out of an expression node. The
# result when evaluated is a no-args function which computes and returns
# the indicated expression.
makeThunk = { expression ::
    <> @["function" ["statements"=[] "yield"=expression]]
};
```
