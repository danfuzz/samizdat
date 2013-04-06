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

statement ::= (varDeclaration | expression) semicolon ;
# result: <same as whatever was parsed>

varDef ::= var name equal expression ;
# result: @[@"type"=@"varDef" @"value"=@[@"name"=<name> @"value"=<expression>]]

expressionList ::= expression* ;
# result: @[@"type"=@"expressionList" @"value"=<listlet of expressions>]

expression ::=
    name | number | stringlet | listlet | maplet | emptyMaplet |
    uniqlet | function | call | openParen expression closeParen;
# result: <same as whatever was parsed>

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

function ::= openCurly argumentSpecs? program closeCurly ;
# result: @[@"type"=@"function"
#           @"value"=@[@"argumentSpecs"=<argument specs> @"program"=<program>]]

argumentSpecs ::= name+ doubleColon ;
# result: @[@"type"=@"argumentSpecs" @"value"=<listlet of names>]

call ::= expression openParen expressionList closeParen;
# result: @[@"type"=@"call"
#           @"value"=@[@"function"=<expression> @"arguments"=<expr list>]]


Library Bindings
----------------

TBD
