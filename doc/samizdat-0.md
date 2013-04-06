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

BNF/PEG-like description of the tokens:

doubleAt    ::= "@@" ;
doubleColon ::= "::" ;
semicolon   ::= ";" ;
equal       ::= "=" ;
at          ::= "@" ;
openBrace   ::= "{" ;
closeBrace  ::= "}" ;
openParen   ::= "(" ;
closeParen  ::= ")" ;
openSquare  ::= "[" ;
closeSquare ::= "]" ;

if     ::= "if" ;
var    ::= "var" ;
return ::= "return" ;

number     ::= "-"? ("0".."9")+ ;
name       ::= ("a".."z" | "A".."Z") ("a".."z" | "A".."Z" | "0".."9")* ;
string     ::= "\"" (~("\\"|"\"") | ("\\" ("\\"|"\""|"n")))* "\"" ;

# Whitespace is ignored.
whitespace ::= " " | "\n" | "#" (~("\n"))* "\n" ;


Tree Syntax
-----------

program ::= statement* ;
# result: @[@"type"=@"program" @"value"=<listlet of statements>]

statement ::= (varDeclaration | expression | returnStatement) semicolon ;
# result: <same as whatever was parsed>

returnStatement ::= return expression ;
# result: @[@"type"=@"return" @"value"=<expression>]

varDef ::= var name equal expression ;
# result: @[@"type"=@"varDef" @"value"=@[@"name"=<name> @"value"=<expression>]]

expressionList ::= expression* ;
# result: @[@"type"=@"expressionList" @"value"=<listlet of expressions>]

expression ::=
    nameReference | number | stringlet | listlet | maplet | emptyMaplet |
    uniqlet | function | callExpression ;
# result: <same as whatever was parsed>

nameReference ::= name ;
# result: @[@"type"=@"nameReference" @"value"=<stringlet of name>]

intlet ::= number ;
# result: @[@"type"=@"literal" @"value"=<intlet of number>]

stringlet ::= at string ;
# result: @[@"type"=@"literal" @"value"=<listlet of characters>]

listlet ::= at openSquare expressionList closeSquare ;
# result: @[@"type"=@"listlet" @"value"=<listlet of expressions>]

maplet ::= at openSquare binding+ closeSquare ;
# result: @[@"type"=@"maplet" @"value"=<listlet of bindings>]

binding ::= expression equal expression ;
# result: @[@"type"=@"binding"
            @"value"=@[@"key"=<key expression> @"value"=<value expression>]]

emptyMaplet ::= openSquare equal closeSquare ;
# result: @[@"type"=@"literal" @"value"=@[=]]

uniqlet ::= doubleAt ;
# result: @[@"type"=@"uniqlet"]

function ::= openCurly argumentSpecs? (program | expression) closeCurly ;
# result: @[@"type"=@"function"
#           @"value"=@[@"argumentSpecs"=<argument specs> @"program"=<program>]]
# Note: If the "expression" variant, the program is the same as
#   `return <expression>;`.

argumentSpecs ::= name+ doubleColon ;
# result: @[@"type"=@"argumentSpecs" @"value"=<listlet of names>]

callExpression ::= openParen expression expressionList closeParen;
# result: @[@"type"=@"call"
#           @"value"=@[@"function"=<expression> @"arguments"=<expr list>]]


Library Bindings
----------------

```
var false = @[@"type"=@"boolean" @"value"=0];
var true = @[@"type"=@"boolean" @"value"=1];

var if = @@; # Bound via magic as a primitive function.
# Note: `if` takes two or more arguments, all of which must be functions that
#   take no arguments. The first argument is called. If it returns `true`,
#   then the result of the `if` is the result of calling the second argument.
#   Otherwise, the third argument is called, with `true` indicating that
#   the fourth is to be the result. And so on. If there is an unpaired
#   argument at the end, it is taken to provide an `else` result.
```

More TBD.
