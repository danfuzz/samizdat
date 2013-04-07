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
    (punctuation | integerToken | stringToken | identifier)
    whitespace*
;
# result: same as the non-whitespace payload.

punctuation ::=
    "@" | # result: @[@"type"=@"@"]
    ":" | # result: @[@"type"=@":"]
    ";" | # result: @[@"type"=@";"]
    "=" | # result: @[@"type"=@"="]
    "^" | # result: @[@"type"=@"^"]
    "{" | # result: @[@"type"=@"{"]
    "}" | # result: @[@"type"=@"}"]
    "(" | # result: @[@"type"=@"("]
    ")" | # result: @[@"type"=@")"]
    "[" | # result: @[@"type"=@"["]
    "]"   # result: @[@"type"=@"]"]
;

integerToken ::= "-"? ("0".."9")+ ;
# result: @[@"type"=@"integer" @"value"=<intlet>].

stringToken ::= "\"" (~("\\"|"\"") | ("\\" ("\\"|"\""|"n")))* "\"" ;
# result: @[@"type"=@"string" @"value"=<stringlet>].

identifier ::= ("a".."z" | "A".."Z") ("a".."z" | "A".."Z" | "0".."9")* ;
# result: @[@"type"=@"identifier" @"value"=<stringlet>].

whitespace ::= " " | "\n" | "#" (~("\n"))* "\n" ;
# result: none; automatically ignored.
```


Tree Syntax
-----------

BNF/PEG-like description of the tree syntax. A program is parsed by
matching the top `program` rule. On the right-hand side of rules,
a stringlet literal indicates a token whose `type` is the literal
value, and an identifier indicates a tree syntax rule to match.

```
program ::= statement* ;
# result: @[@"type"=@"program" @"value"=<listlet of statements>]

statement ::= (varDef | expression | returnStatement) @";" ;
# result: <same as whatever was parsed>

returnStatement ::= @"^" expression ;
# result: @[@"type"=@"return" @"value"=<expression>]

varDef ::= @"identifier" @"=" expression ;
# result: @[@"type"=@"varDef"
#           @"value"=@[@"name"=<identifier.value> @"value"=<expression>]]

expressionList ::= expression* ;
# result: @[@"type"=@"expressionList" @"value"=<listlet of expressions>]

expression ::=
    varRef | intlet | integer | stringlet | listlet |
    maplet | emptyMaplet | uniqlet | function | callExpression ;
# result: <same as whatever was parsed>

varRef ::= @"identifier" ;
# result: @[@"type"=@"varRef" @"value"=<identifier.value>]

intlet ::= @"@" @"integerToken" ;
# result: @[@"type"=@"literal" @"value"=<integerToken.value>]

integer ::= @"integerToken" ;
# result: @[@"type"=@"literal" @"value"=<integerToken>]

stringlet ::= @"@" @"string" ;
# result: @[@"type"=@"literal" @"value"=<stringToken.value>]

listlet ::= @"@" @"[" expressionList @"]" ;
# result: @[@"type"=@"listlet" @"value"=<listlet of expressions>]

maplet ::= @"@" @"[" binding+ @"]" ;
# result: @[@"type"=@"maplet" @"value"=<listlet of bindings>]

binding ::= expression @"=" expression ;
# result: @[@"type"=@"binding"
            @"value"=@[@"key"=<key expression> @"value"=<value expression>]]

emptyMaplet ::= @"[" @"=" @"]" ;
# result: @[@"type"=@"literal" @"value"=@[=]]

uniqlet ::= @"@" @"@";
# result: @[@"type"=@"uniqlet"]

function ::= @"{" argumentSpecs? (program | expression) @"}" ;
# result: @[@"type"=@"function"
#           @"value"=@[@"argumentSpecs"=<argument specs> @"program"=<program>]]
# Note: If the "expression" variant, the program is the same as
#   `return <expression>;`.

argumentSpecs ::= @"identifier"+ @":" @":" ;
# result: @[@"type"=@"argumentSpecs" @"value"=<listlet of identifier.values>]

callExpression ::= @"(" expression expressionList @")" ;
# result: @[@"type"=@"call"
#           @"value"=@[@"function"=<expression> @"arguments"=<expr list>]]
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
