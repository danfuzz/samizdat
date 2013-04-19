Samizdat Layer 0
================

This is the specification for the language known as "Samizdat Layer 0".
The language is meant to provide a syntactically and semantically
minimal way to build up and manipulate low layer Samizdat data.

Samizdat Layer 0 is run by translating it into low layer Samizdat
data, as specified here, and running it with the indicated library
bindings.


Data Syntax And Semantics In A Nutshell
---------------------------------------

This section provides a few quick examples of what *Samizdat
Layer 0* data types look like as text, and what they mean.

To avoid naming conflicts with the type names used in the final
language layer, in the low layer, type names get a suffix "let" (to
imply "little version", like "booklet" is to "book").

Similarly, to avoid syntactic conflicts, most low layer types
are represented using an initial `@` character.

Keep in mind that many of the restrictions and caveats mentioned
(such as, for example, what can be backslash-escaped in a string)
are specific to *Samizdat Layer 0*, with higher layers providing
much fuller expressivity.


### Comments

Comments start with `#` and continue to the end of a line.

```
#
# I am commentary.
```


### Intlet

An `intlet` is a signed arbitrary-precision integer value, a/k/a a
"bigint" or "BigInteger" (even though they aren't always actually
that big). In the C implementation, there is actually
a limitation that intlets only have a 32-bit signed range, with
out-of-range arithmetic results causing failure, not wraparound.

Intlets are written as an initial `@`, followed by an optional
`-` (minus sign), followed by one or more decimal digits, and the
digits are interpreted in base 10. (There is no hex representation.)

```
@0
@20
@-234452
```

### Stringlet

A `stringlet` is a sequence of zero or more Unicode code points.

Stringlets are written as an initial `@"`, followed by zero or
more character representations, followed by a final `"`.

Characters are self-representing, except that there are three
(and only three) backslash-quoted escapes:

* `\\` &mdash; backslash itself
* `\"` &mdash; a double quote
* `\n` &mdash; newline (Unicode U+0010)

```
@""                           # the empty string
@"Hello, Самиздат!"
@"\"blort\" -- potion that enables one to see in the dark.\n"
```


### Listlet

A `listlet` is a sequence of zero or more other values.

Listlets are written as an initial `@[`, followed by zero or
more value representations, followed by a final `]`.

```
@[]                           # the empty listlet
@[@1]
@[@"blort" @"fizmo" @"igram"]
@[@[@1] @2]
```


### Maplet

A `maplet` is a sequence of zero or more mappings (a/k/a bindings)
from arbitrary keys to arbitrary values. Keys and values are both
allowed to be any kind of other value. Notably, keys are *not*
restricted to only being strings (or string-like things).

Non-empty maplets are written as an initial `@[`, followed by one or
more mappings, followed by a final `]`. Mappings are written as
the key representation, followed by an `=`, followed by the value
representation.

To avoid ambiguity with the empty listlet, the empty maplet is
written as `@[=]`.

```
@[=]                          # the empty maplet
@[@1=@"one"]
@[@"blort" = @"potion; the ability to see in the dark"
  @"fizmo" = @"spell; unclogs pipes"
  @"igram" = @"spell; make purple things invisible"]
@[@[@"complex" @"data" @"as" @"key"] = @"handy!"]
```


### Uniqlet

A `uniqlet` is a bit of an odd duck. Uniqlets are opaque, except
that no uniqlet is equal to any other uniqlet. In practice, uniqlets
are used to help bridge the divide between data and not-data, with
one of the main not-data things in the system being functions.

Uniqlets are written as simply `@@`. Each mention of `@@` refers
to a different value.

```
@@
```


### Highlet

A `highlet` is a combination of a type tag value and an optional
payload value. Highlets are the bridge between low-layer data and
high-layer data (hence the name). Highlets are also used as the
low-layer type returned by parsing (including tokenization) functions.

Highlets are written as an initial `[:`, followed by a type tag
representation (an arbitrary value), optionally followed by a
payload value (another arbitrary value), followed by a final `:]`.

```
[:@"null":]                   # the value usually just written as `null`
[:@"boolean" @0:]             # the value usually just written as `false`
[:@"boolean" @1:]             # the value usually just written as `true`
[:
  @"spell"
  @[@"name"=@"frotz" @"purpose"=@"cause item to glow"]
:]
```


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
# result: [:@"call" @[@"function"=<atom> @"actuals"=<atom list>]:]
```


Library Bindings
----------------

```
null = [:@"null":];
false = [:@"boolean" @0:];
true = [:@"boolean" @1:];
```

More TBD.
