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
    "@@" | # result: @[@"type"=@"@@"]
    "::" | # result: @[@"type"=@"::"]
    "@"  | # result: @[@"type"=@"@"]
    ":"  | # result: @[@"type"=@":"]
    ";"  | # result: @[@"type"=@";"]
    "="  | # result: @[@"type"=@"="]
    "^"  | # result: @[@"type"=@"^"]
    "{"  | # result: @[@"type"=@"{"]
    "}"  | # result: @[@"type"=@"}"]
    "("  | # result: @[@"type"=@"("]
    ")"  | # result: @[@"type"=@")"]
    "["  | # result: @[@"type"=@"["]
    "]"    # result: @[@"type"=@"]"]
;

integer ::= "-"? ("0".."9")+ ;
# result: @[@"type"=@"integer" @"value"=<intlet>].

string ::= "\"" (~("\\"|"\"") | ("\\" ("\\"|"\""|"n")))* "\"" ;
# result: @[@"type"=@"string" @"value"=<stringlet>].

identifier ::= ("a".."z" | "A".."Z") ("a".."z" | "A".."Z" | "0".."9")* ;
# result: @[@"type"=@"identifier" @"value"=<stringlet>].

whitespace ::= " " | "\n" | "#" (~("\n"))* "\n" ;
# result: none; automatically ignored.
```


Node / Tree Syntax
------------------

BNF/PEG-like description of the node / tree syntax. A program is
parsed by matching the top `statements` rule. On the right-hand side
of rules, a stringlet literal indicates a token whose `type` is the
literal value, and an identifier indicates a tree syntax rule to
match.

```
statements ::= statement* ;
# result: @[@"type"=@"statements" @"value"=<listlet of statements>]

statement ::= (varDef | expression | return) @";" ;
# result: <same as whatever was parsed>

return ::= @"^" expression ;
# result: @[@"type"=@"return" @"value"=<expression>]

expressions ::= expression* ;
# result: @[@"type"=@"expressions" @"value"=<listlet of expressions>]

expression ::=
    varRef | intlet | integer | stringlet | listlet |
    maplet | emptyMaplet | uniqlet | function | call |
    parenExpression
# result: <same as whatever was parsed>

parenExpression ::= @"(" (call | expression) @")";
# result: <same as whatever was parsed>
# Note: This is to make parenthesized expressions "prefer" to be calls.

varDef ::= @"identifier" @"=" expression ;
# result: @[@"type"=@"varDef"
#           @"value"=@[@"name"=<identifier.value> @"value"=<expression>]]

varRef ::= @"identifier" ;
# result: @[@"type"=@"varRef" @"value"=<identifier.value>]

intlet ::= @"@" @"integer" ;
# result: @[@"type"=@"literal" @"value"=<integer.value>]

integer ::= @"integer" ;
# result: @[@"type"=@"literal" @"value"=<integer>]

stringlet ::= @"@" @"string" ;
# result: @[@"type"=@"literal" @"value"=<string.value>]

listlet ::= @"@" @"[" expressions @"]" ;
# result: @[@"type"=@"listlet" @"value"=<listlet of expressions>]

maplet ::= @"@" @"[" binding+ @"]" ;
# result: @[@"type"=@"maplet" @"value"=<listlet of bindings>]

binding ::= expression @"=" expression ;
# result: @[@"type"=@"binding"
            @"value"=@[@"key"=<key expression> @"value"=<value expression>]]

emptyMaplet ::= @"@" @"[" @"=" @"]" ;
# result: @[@"type"=@"literal" @"value"=@[=]]

uniqlet ::= @"@@";
# result: @[@"type"=@"uniqlet"]

function ::= @"{" formals statements @"}" ;
# result: @[@"type"=@"function"
#           @"value"=@[@"formals"=<formals> @"statements"=<statements>]]

formals ::= (@"identifier"+ @"::") | ~. ;
# result: @[@"type"=@"formals" @"value"=<listlet of identifier.values>]

call ::= expression expressions ;
# result: @[@"type"=@"call"
#           @"value"=@[@"function"=<expression> @"actuals"=<expr list>]]
```


Library Bindings
----------------

```
false = @[@"type"=@"boolean" @"value"=0];
true = @[@"type"=@"boolean" @"value"=1];

if = @@; # Bound via magic as a primitive function.
# Note: `if` takes two or more arguments, all of which must be functions that
#   take no arguments. The first argument is called. If it returns `true`,
#   then the result of the `if` is the result of calling the second argument.
#   Otherwise, the third argument is called, with `true` indicating that
#   the fourth is to be the result. And so on. If there is an unpaired
#   argument at the end, it is taken to provide an `else` result.
```

More TBD.
