Samizdat Layer 0
================

Tree Syntax
-----------

The following is a BNF/PEG-like description of the node / tree syntax.
A program is parsed by matching the `program` rule, which yields a
`function` node. On the right-hand side of rules, a stringlet literal
indicates a token whose `type` is the literal value, and an identifier
indicates a tree syntax rule to match. All of `*` `+` `?` `(` `)` have
their usual PEG interpretations (similar to their interpretation in
regular expressions).

```
function ::= @"{" program @"}" ;
# result: <program>

program ::=
    (formals? yieldDef? @"::")?
    @";"* (statement @";"+)* (statement | nonlocalExit | yield)? ;
# result: [:
#             @function
#             @[
#                 (@formals=<formals>)?
#                 (@yieldDef=<yieldDef>)?
#                 @statements=<listlet of non-empty statements>
#                 (@yield=<yield>)?
#             ]
#         :]
# Note: nonLocalExit results in a statement.

formals ::= formal+ ;
# result: @[<formal> ...]

formal ::= @identifier (@"*" | @"?")? ;
# result: @[@name=(highletValue identifier) (@repeat=[:(@"*"|@"?"):])?]

yieldDef ::= @"<" @identifier @">" ;
# result: <identifier.value>

yield ::= @"<>" expression @";"* ;
# result: <expression>

nonlocalExit ::= @"<" @identifier @">" expression? @";"* ;
# result: makeCall <identifier> <expression>?

statement ::= varDef | expression ;
# result: <varDef> | <expression>

expression ::= call | unary ;
# result: <same as whatever was parsed>

unary ::= unaryCall | atom ;
# result: <same as whatever was parsed>

unaryCall ::= atom (@"(" @")")+ ;
# result: (... (makeCall (makeCall atom))
# Note: One `makeCall` per pair of parens.

atom ::=
    varRef | intlet | stringlet |
    emptyListlet | listlet | emptyMaplet | maplet |
    uniqlet | highlet | function | parenExpression ;
# result: <same as whatever was parsed>

parenExpression ::= @"(" expression @")";
# result: <expression>

varDef ::= @identifier @"=" expression ;
# result: [:@varDef @[@name=(highletValue identifier) @value=<expression>]:]

varRef ::= @identifier ;
# result: [:@varRef (highletValue <identifier>):]

intlet ::= @"@" @"-"? @integer ;
# result: [:@literal (imul (@1|@-1) (highletValue <integer>)):]

stringlet ::= @"@" (@string | @identifier);
# result: [:@literal (highletValue <string|identifier>):]

emptyListlet ::= @"@" @"[" @"]" ;
# result: [:@literal @[]:]

listlet ::= @"@" @"[" unary+ @"]" ;
# result: makeCall [:@varRef @makeListlet:] <unary>+

emptyMaplet ::= @"@" @"[" @"=" @"]" ;
# result: [:@literal @[=]:]

maplet ::= @"@" @"[" binding+ @"]" ;
# result: makeCall [:@varRef @makeMaplet:] (<binding key> <binding value>)+

binding ::= unary @"=" unary ;
# result: @[<key unary> <value unary>]

uniqlet ::= @"@@";
# result: makeCall [:@varRef @makeUniqlet:]

highlet ::= @"[" @":" unary unary? @":" @"]" ;
# result: makeCall [:@varRef @makeHighlet:] <type unary> <value unary>?

call ::= unary unary+ ;
# result: makeCall <function unary> <argument unary>+

# Function that returns an appropriately-formed `call` node.
makeCall = { function actuals* ::
    <> [:@call @[@function=function @actuals=actuals]:]
}
```
