Samizdat Layer 0
================

Tree Syntax
-----------

The following is a BNF/PEG-like description of the node / tree syntax.
This definition uses syntax which is nearly identical to the parser
syntax provided at the higher layer.

A program is parsed by matching the `program` rule, which yields a
`function` node.

```
# Function that returns an appropriately-formed `call` node.
makeCall = { function actuals* ::
    <> @["call" ["function"=function "actuals"=actuals]]
};

# forward-declaration: atom
# forward-declaration: expression
# forward-declaration: function

int ::= @int ;
# result: @["literal" (highletValue int)]

string ::= @string ;
# result: @["literal" (highletValue string)]

emptyList ::= @"[" @"]" ;
# result: @["literal" []]

list ::= @"[" atom+ @"]" ;
# result: makeCall @["varRef" "makeList"] atom+

emptyMap ::= @"[" @"=" @"]" ;
# result: @["literal" [=]]

binding ::= atom @"=" atom ;
# result: [atom atom] # key then value

map ::= @"[" binding+ @"]" ;
# result: apply makeCall @["varRef" "makeMap"] (listAdd binding+)

highlet ::=
    @"@" @"[" atom atom? @"]"
    # result: makeCall @["varRef" "makeHighlet"] atom atom?
|
    @"@" @string
    # result: makeCall @["varRef" "makeHighlet"]
    #             @["literal" (highletValue string)]
|
    @"@" @identifier
    # result: makeCall @["varRef" "makeHighlet"]
    #             @["literal" (highletValue identifier)]
;

uniqlet ::= @"@@";
# result: makeCall @["varRef" "makeUniqlet"]

varRef ::= @identifier ;
# result: @["varRef" (highletValue identifier)]

varDef ::= @identifier @"=" expression ;
# result: @["varDef" ["name"=(highletValue identifier) "value"=expression]]

parenExpression ::= @"(" expression @")";
# result: expression

atom ::=
    varRef | int | string |
    emptyList | list | emptyMap | map |
    uniqlet | highlet | function | parenExpression ;
# result: <same as whatever choice matched>

callExpression ::= atom atom+ ;
# result: makeCall atom atom+

unaryCallExpression ::= atom (@"()")+ ;
# result: (... (makeCall (makeCall atom))
# Note: One `makeCall` per pair of parens.

unaryExpression ::= unaryCallExpression | atom ;
# result: <same as whatever choice matched>

expression ::= callExpression | unaryExpression ;
# result: <same as whatever choice matched>

statement ::= varDef | expression ;
# result: <same as whatever choice matched>

nonlocalExit ::= @"<" @identifier @">" expression? ;
# result: makeCall identifier expression?

yield ::= @"<>" expression ;
# result: expression

yieldDef ::= @"<" @identifier @">" ;
# result: highletValue identifier

formal ::= (@identifier | @".") repeat=(@"?" | @"*" | @"+")? ;
# result: [
#             ("name" = (highletValue identifier))?
#             ("repeat" = (highletType repeat))?
#         ]
# Note: Binding for `"name"` omitted if name is specified as `.`.

programBody ::=
    @";"*
    (statement @";"+)*
    (statement | nonlocalExit | yield)?
    @";"*
;
# result: ["statements"=[statement*] ("yield"=yield)?]
# Note: `nonLocalExit` results in a statement.

programDeclarations ::= formal* yieldDef? ;
# result: [("formals"=[formal*])? ("yieldDef"=yieldDef)?]
# Note: The "formals" mapping should only be included when there is at
# least one formal.

program ::= (programDeclarations @"::")? programBody ;
# result: @[
#             "function"
#             (mapAdd programDeclarations programBody)
#         ]

function ::= @"{" program @"}" ;
# result: <program>
```
