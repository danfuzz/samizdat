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

The library is split into two layers, primitive and in-language.
From the perspective of code, there is no distinction between the
two layers, but from the perspective of implementation, there is.
In particular, an implementation of *Samizdat Layer 0* must
provide the primitive library, but it can rely on the canonical
in-language library source for the remainder.

Each function binding here is listed in a short-hand form suggestive
of how it would be called.

* The name of the function is first on the line.

* Next come the names of arguments in order, separated with spaces but
  no other punctuation.

* If the function has no arguments, the marker `()` is appended to
  the name.

* If an argument is optional, it is followed by a `?` (question mark).

* A "rest" argument at the end (capturing zero or more additional
  arguments) is indicated by following it by a `*` (star / asterisk).

* If the function returns a value, that is indicated at the end of the
  line with `<>` (a "diamond") and then the type name of the kind of
  value returned or `.` (a dot) indicating "any possible value".

* If the function does not return a value, that is indicated by ending
  the line with `<> ~.`.

* If a function might or might not return a value, that is indicated
  by ending the line with `<> . | ~.`. (Note, the return value stuff
  is is not actual *Samizdat Layer 0* syntax.)


<br><br>
### Primitive Library: Value Definitions

#### `false`

The boolean value false. It can also be written as `[:@"boolean" @0:]`.

#### `true`

The boolean value true. It can also be written as `[:@"boolean" @1:]`.

<br><br>
### Primitive Library: Conditionals

#### `ifTrue predicate thenFunc elseFunc? <> . | ~.`

Primitive boolean conditional. This calls the given predicate with no
arguments, expecting it to return a boolean.

If the predicate returns `true`, then the `thenFunc` is called with no
arguments. If the predicate returns `false`, then the `elseFunc` (if
any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called. If no consequent was called, this
returns no value.

#### `ifValue func valueFunc voidFunc? <> . | ~.`

Primitive value conditional. This calls the given function with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunc` is called with one
argument, namely the value returned from the original function. If the
function does not return a value, then the `voidFunc` (if any) is called
with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called. If no consequent was called, this
returns no value.

<br><br>
### Primitive Library: General Low-Order Values

#### `lowOrder value1 value2 <> intlet`

Returns the order of the two given values in the total order of
Samizdat low-layer values. This returns one of `@-1 @0 @1` indicating
how the two values sort with each other, using the standard meaning
of those values. Ordering is calculated as follows:

The major order is by type &mdash; `intlet < stringlet < listlet <
maplet < uniqlet < highlet` &mdash; and minor order is type-dependant.

* Intlets order by integer value.

* Listlets and stringlets order by pairwise corresponding-element
  comparison, with a strict prefix always ordering before its
  longer brethren.

* Maplets order by first comparing corresponding ordered lists
  of keys with the same rules as listlet comparison. If the key
  lists are identical, then the result is the comparison of
  corresponding lists of values, in key order.

* Any given uniqlet never compares as equal to anything but itself.
  Any two uniqlets have a consistent and transitive &mdash; but
  otherwise arbitrary &mdash; ordering.

* Highlets compare by type as primary, and value as secondary.
  With types equal, a highlets without a value order earlier than
  ones with a value.

#### `lowOrderIs value1 value2 check1 check2? <> boolean`

The two values are compared as with `lowOrder`. The intlet
result of that comparison are checked for equality with
the one or two check values. If the comparison result is equal
to either check value, this function returns `true`. Otherwise
it returns `false`.

*Note:* This function exists in order to provide a primitive
function that returns a boolean. Without it, there would be
no way to define boolean comparators in-language.

#### `lowSize value <> intlet`

Gets the "size" of the given value. Every low-layer value has
a size, defined as follows:

* `intlet` &mdash; the number of significant bits (*not* bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit.

* `stringlet` &mdash; the number of characters.

* `listlet` &mdash; the number of elements.

* `maplet` &mdash; the number of mappings (bindings).

* `uniqlet` &mdash; always `@0`.

* `highlet` &mdash; `@0` for a valueless highlet, or `@1` for a
  valued highlet.

#### `lowType value <> stringlet`

Gets the type name of the low-layer type of the given value. The
result will be one of: `@"intlet" @"stringlet" @"listlet" @"maplet"
@"uniqlet" @"highlet"`

<br><br>
### Primitive Library: Intlets

#### `iadd intlet1 intlet2 <> intlet`

Returns the sum of the given values.

#### `idiv intlet1 intlet2 <> intlet`

Returns the quotient of the given values (first over second).

#### `imod intlet1 intlet2 <> intlet`

Returns the modulus of the given values (first over second).

*Note:* This differs from the remainder in the treatment of
negative numbers.

#### `imul intlet1 intlet2 <> intlet`

Returns the product of the given values.

#### `ineg intlet <> intlet`

Returns the negation (same magnitude, opposite sign) of the given
value.

#### `isub intlet1 intlet2 <> intlet`

Returns the difference of the given values (first minus second).

<br><br>
### Primitive Library: Stringlets

#### `stringletAdd stringlet1 stringlet2 <> stringlet`

Returns a stringlet consisting of the concatenation of the two
argument stringlets, in argument order.

#### `stringletFromChar intlet <> stringlet`

Returns a single-character stringlet that consists of the character
code indicated by the given intlet argument, which must be in the
range for representation as an unsigned 32-bit quantity.

#### `stringletFromChars listlet <> stringlet`

Returns a stringlet that consists of the character codes indicated by
the elements of the given listlet argument. Each element must be an
intlet in the range for representation as an unsigned 32-bit quantity.

#### `stringletNth stringlet n <> intlet`

Returns the `n`th (zero-based) element of the given stringlet,
as an intlet.

#### `stringletToChars stringlet <> listlet`

Returns a listlet that consists of the character codes of the
given stringlet, each represented as an intlet in the result.


<br><br>

### In-Language Library

#### `null`

A value used when no other value is suitable, but when a value is
nonetheless required. It can also be written as `[:@"null":]`.

#### More TBD.
