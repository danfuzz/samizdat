Samizdat Layer 0
================

Tree Syntax
-----------

The following is a BNF/PEG-like description of the node / tree syntax.
A program is parsed by matching the `program` rule, which yields a
`function` node. On the right-hand side of rules, a string literal
indicates a token whose `type` is the literal value, and an identifier
indicates a tree syntax rule to match. All of `*` `+` `?` `|` `(` `)` have
their usual PEG interpretations (similar to their interpretation in
regular expressions).

```
function ::= [:"{":] program [:"}":] ;
# result: <program>

program ::= (programDeclarations [:"::":])? programBody ;
    [:";":]* (statement [:";":]+)* (statement | nonlocalExit | yield)?
    [:";":]*
;
# result: [:
#             @function
#             (mapAdd programDeclarations programBody)
#         :]

programDeclarations ::= formal* yieldDef? ;
# result: [(@formals=[formal*])? (yieldDef=yieldDef)?]
# Note: The @formals mapping should only be included when there is at
# least one formal.

programBody ::=
    [:";":]*
    (statement [:";":]+)*
    (statement | nonlocalExit | yield)?
    [:";":]*
;
# result: [@statements=[statement*] (@yield=yield)?]
# Note: `nonLocalExit` results in a statement.

formal ::= ([:@identifier:] | [:".":]) ([:"*":] | [:"?":])? ;
# result: [(@name=(highletValue identifier))? (@repeat=[:("*"|"?"):])?]
# Note: Binding for `@name` omitted if name is specified as `.`.

yieldDef ::= [:"<":] [:@identifier:] [:">":] ;
# result: highletValue identifier

yield ::= [:"<>":] expression ;
# result: expression

nonlocalExit ::= [:"<":] [:@identifier:] [:">":] expression? ;
# result: makeCall identifier expression?

statement ::= varDef | expression ;
# result: <same as whatever choice matched>

expression ::= callExpression | unaryExpression ;
# result: <same as whatever choice matched>

unaryExpression ::= unaryCallExpression | atom ;
# result: <same as whatever choice matched>

unaryCallExpression ::= atom ([:"(":] [:")":])+ ;
# result: (... (makeCall (makeCall atom))
# Note: One `makeCall` per pair of parens.

callExpression ::= atom atom+ ;
# result: makeCall atom atom+

atom ::=
    varRef | integer | string |
    emptyList | list | emptyMap | map |
    uniqlet | highlet | function | parenExpression ;
# result: <same as whatever choice matched>

parenExpression ::= [:"(":] expression [:")":];
# result: expression

varDef ::= [:@identifier:] [:"=":] expression ;
# result: [:@varDef [@name=(highletValue identifier) @value=expression]:]

varRef ::= [:@identifier:] ;
# result: [:@varRef (highletValue identifier):]

integer ::= [:@integer:] ;
# result: [:@literal (highletValue integer):]

string ::= (@string | [:"@":] @identifier);
# result: [:@literal (highletValue (string|identifier)):]

emptyList ::= [:"[":] [:"]":] ;
# result: [:@literal []:]

list ::= [:"[":] atom+ [:"]":] ;
# result: makeCall [:@varRef @makeList:] atom+

emptyMap ::= [:"[":] [:"=":] [:"]":] ;
# result: [:@literal [=]:]

map ::= [:"[":] binding+ [:"]":] ;
# result: apply makeCall [:@varRef @makeMap:] (listAdd binding+)

binding ::= atom [:"=":] atom ;
# result: [atom atom] # key then value

uniqlet ::= [:"@@":];
# result: makeCall [:@varRef @makeUniqlet:]

highlet ::= [:"[":] [:":":] atom atom? [:":":] [:"]":] ;
# result: makeCall [:@varRef @makeHighlet:] atom atom?

# Function that returns an appropriately-formed `call` node.
makeCall = { function actuals* ::
    <> [:@call [@function=function @actuals=actuals]:]
}
```
