Samizdat Layer 0
================

This is the specification for the language known as "Samizdat Layer 0".
The language is meant to provide a syntactically and semantically
minimal way to build up and manipulate low layer Samizdat data.

Samizdat Layer 0 is run by translating it into low layer Samizdat
data, as specified here, and running it with the indicated library
bindings.


Token Syntax
------------

BNF/PEG-like description of the tokens. A program is tokenized by
repeatedly matching the top `token` rule.

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
    "<>" | # result: [:@"<>":]
    "@"  | # result: [:@"@":]
    ":"  | # result: [:@":":]
    "*"  | # result: [:@"*":]
    ";"  | # result: [:@";":]
    "="  | # result: [:@"=":]
    "{"  | # result: [:@"{":]
    "}"  | # result: [:@"}":]
    "("  | # result: [:@"(":]
    ")"  | # result: [:@")":]
    "["  | # result: [:@"[":]
    "]"    # result: [:@"]":]
;

integer ::= "-"? ("0".."9")+ ;
# result: [:@"integer" <intlet>:]

string ::= "\"" (~("\\"|"\"") | ("\\" ("\\"|"\""|"n")))* "\"" ;
# result: [:@"string" <stringlet>:]

identifier ::= ("a".."z" | "A".."Z") ("a".."z" | "A".."Z" | "0".."9")* ;
# result: [:@"identifier" <stringlet>:]

whitespace ::= " " | "\n" | "#" (~("\n"))* "\n" ;
# result: none; automatically ignored.
```


Node / Tree Syntax
------------------

BNF/PEG-like description of the node / tree syntax. A program is
parsed by matching the top `program` rule, which yields a `function`
node. On the right-hand side of rules, a stringlet literal indicates a
token whose `type` is the literal value, and an identifier indicates a
tree syntax rule to match.

```
function ::= @"{" program @"}" ;
# result: <program>

program ::= formals block ;
# result: [:@"function" @[@"formals"=<formals> @"block"=<block>]:]

formals ::= (@"identifier"+ @"*"? @"::") | ~. ;
# result: [:
#             @"formals"
#             @[@[@"name"=<identifier.value> @"repeat"=@[@"type"=(@"."|@"*")]]
#               ...]
#         :]

block ::= statement* yield? ;
# result: [:
#             @"block"
#             @[@"statements"=<listlet of statements> (@"yield"=<yield>)?]
#         :]

yield ::= @"<>" expression @";" ;
# result: <expression>

statement ::= (varDef | expression) @";" ;
# result: <same as whatever was parsed>

expression ::= call | atom ;
# result: <same as whatever was parsed>

atom ::=
    varRef | intlet | integer | stringlet |
    emptyListlet | listlet | emptyMaplet | maplet |
    uniqlet | highlet | function | parenExpression
# result: <same as whatever was parsed>

parenExpression ::= @"(" expression @")";
# result: <expression>

varDef ::= @"identifier" @"=" expression ;
# result: [:@"varDef" @[@"name"=<identifier.value> @"value"=<expression>]:]

varRef ::= @"identifier" ;
# result: [:@"varRef" <identifier.value>:]

intlet ::= @"@" @"integer" ;
# result: [:@"literal" <integer.value>:]

integer ::= @"integer" ;
# result: [:@"literal" <integer>:]

stringlet ::= @"@" @"string" ;
# result: [:@"literal" <string.value>:]

emptyListlet ::= @"@" @"[" @"]" ;
# result: [:@"literal" @[]:]

listlet ::= @"@" @"[" atom+ @"]" ;
# result: [:@"listlet" <listlet of atoms>:]

emptyMaplet ::= @"@" @"[" @"=" @"]" ;
# result: [:@"literal" @[=]:]

maplet ::= @"@" @"[" binding+ @"]" ;
# result: [:@"maplet" <listlet of bindings>:]

binding ::= atom @"=" atom ;
# result: @[@"key"=<key atom> @"value"=<value atom>]

uniqlet ::= @"@@";
# result: [:@"uniqlet":]

highlet ::= @"[" @":" atom atom? @":" @"]";
# result: [:@"highlet" @[@"type"=<type atom> (@"value"=<value atom>)?]:]

call ::= atom (@"(" @")" | atom+) ;
# result: [:@"call" @"value"=@[@"function"=<atom> @"actuals"=<atom list>]:]
```


Library Bindings
----------------

```
null = [:@"null":];
false = [:@"boolean" @0:];
true = [:@"boolean" @1:];
```

More TBD.
