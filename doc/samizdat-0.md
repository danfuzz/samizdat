Samizdat Layer 0
================

This is the specification for the language known as "Samizdat Layer 0".
The language is meant to provide a syntactically and semantically
minimal way to build up and manipulate low-layer Samizdat data.

Samizdat Layer 0 is run by translating it into low-layer Samizdat
data, as specified here, and running it with the indicated library
bindings.


<br><br>
Data Syntax And Semantics In A Nutshell
---------------------------------------

This section provides a few quick examples of what *Samizdat
Layer 0* data types look like as text, and what they mean.

First and foremost on the topic of semantics, all data values are
immutable. For example, appending to or deleting from a list always
results in a new list, with the original remaining unchanged

With regards to naming, to avoid naming conflicts with the type names
used in the final language layer, in the low layer, type names get a
suffix "let" (to imply "little version", like "booklet" is to "book").

As with naming, to avoid higher layer syntactic conflicts, most low
layer types are represented using an initial `@` character.

Keep in mind that many of the restrictions and caveats mentioned
(such as, for example, what can be backslash-escaped in a string)
are specific to *Samizdat Layer 0*, with higher layers providing
much fuller expressivity. It may be useful to think of this layer
as being analogous to the bytecode layer of bytecode-based systems.


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

Intlets are written as an initial `@`, followed by an optional `-`
(minus sign), followed by one or more decimal digits. The digits are
interpreted in base 10 to form the number. (There is no hexadecimal
representation.)

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
allowed to be any type of value. Notably, keys are *not* restricted to
only being strings (or string-like things).

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

A `uniqlet` is a bit of an odd duck. Uniqlets are opaque, except that
no uniqlet is equal to any other uniqlet. In practice, uniqlets are
used to help bridge the divide between data and not-data, with some of
the main not-data sorts of things in the system being functions.

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
representation (an arbitrary value), optionally followed by a payload
value representation (another arbitrary value), followed by a final
`:]`.

```
[:@"null":]                   # the value usually just written as `null`
[:@"boolean" @0:]             # the value usually just written as `false`
[:@"boolean" @1:]             # the value usually just written as `true`
[:
  @"spell"
  @[@"name"=@"frotz" @"purpose"=@"cause item to glow"]
:]
```


<br><br>
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


<br><br>
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

program ::= formals statement* yield? ;
# result: [:
#             @"function"
#             @[
#                 @"formals"=<formals>
#                 @"statements"=<listlet of statements>
#                 (@"yield"=<yield>)?
#             ]
#         :]

formals ::= (@"identifier"+ @"*"? @"::") | ~. ;
# result: [:
#             @"formals"
#             @[@[@"name"=(highValue identifier)
#                 @"repeat"=@[@"type"=(@"."|@"*")]]
#               ...]
#         :]

yield ::= @"<>" expression @";" ;
# result: <expression>

statement ::= (varDef | expression) @";" ;
# result: <same as whatever was parsed>

expression ::= call | atom ;
# result: <same as whatever was parsed>

atom ::=
    varRef | intlet | stringlet |
    emptyListlet | listlet | emptyMaplet | maplet |
    uniqlet | highlet | function | parenExpression
# result: <same as whatever was parsed>

parenExpression ::= @"(" expression @")";
# result: <expression>

varDef ::= @"identifier" @"=" expression ;
# result: [:@"varDef" @[@"name"=(highValue identifier) @"value"=<expression>]:]

varRef ::= @"identifier" ;
# result: [:@"varRef" (highValue <identifier>):];

intlet ::= @"@" @"integer" ;
# result: [:@"literal" (highValue <integer>):]

stringlet ::= @"@" @"string" ;
# result: [:@"literal" (highValue <string>):]

emptyListlet ::= @"@" @"[" @"]" ;
# result: [:@"literal" @[]:]

listlet ::= @"@" @"[" atom+ @"]" ;
# result: makeCall [:@"varRef" @"makeListlet":] <atom>+

emptyMaplet ::= @"@" @"[" @"=" @"]" ;
# result: [:@"literal" @[=]:]

maplet ::= @"@" @"[" binding+ @"]" ;
# result: makeCall [:@"varRef" @"makeMaplet":]
#             (<binding key> <binding value>)+;

binding ::= atom @"=" atom ;
# result: @[<key atom> <value atom>]

uniqlet ::= @"@@";
# result: makeCall [:@"varRef" @"makeUniqlet":]

highlet ::= @"[" @":" atom atom? @":" @"]";
# result: makeCall [:@"varRef" @"makeHighlet":] <type atom> <value atom>?

call ::= atom (@"(" @")" | atom+) ;
# result: makeCall <function atom> <argument atom+>

makeCall = { function actuals* ::
    <> [:@"call" @[@"function"=function @"actuals"=actuals]:];
};
```


<br><br>
Node / Tree Semantics
---------------------

These are all the node types that can occur in a parse tree, such that
it can be compiled (or "compile-equivalent"ed) by `sam0Eval` (and the
like). These are presented in a form meant to be representative of how
one would construct them in the source syntax of *Samizdat Layer 0*.

<br><br>
### Expression Nodes

Each of these node types can appear anywhere an "expression"
is called for.

#### `call` &mdash; `[:@"call" @[@"function"=function @"actuals"=actuals]:]`

This represents a function call. In the data payload, `function` is an
arbitrary expression node, and `actuals` is a listlet of arbitrary
expression nodes.

When run, first `function` and then the elements of `actuals` (in order)
are evaluated. If `function` evaluates to something other than a
function, the call fails (terminating the runtime). If any of the
`actuals` evaluate without yielding a value, the call fails (terminating
the runtime).

If there are too few actual arguments for the function (e.g. the
function requires at least three arguments, but only two are passed),
then the call fails (terminating the runtime).

With all the above prerequisites passed, the function is applied to
the evaluated actuals as its arguments, and the result of evaluation
is the same as whatever was returned by the function call (including
nothing, if the function turned out to not to return anything).

#### `function` &mdash; `[:@"function" @[@"formals"=formals @"statements"=statements (@"yield"=yield)?:]`

This represents a function definition. In the data payload, `formals`
is a `formals` node (as defined below), `statements` must be a
listlet, with each of the elements being either an expression node or
a `varDef` node. The `yield` binding is optional, but if present must
be an expression node.

When run, a closure (representation of the function as an in-model
value) is created, which nascently binds as variables the names of all
the formals to all the incoming actual arguments, and binds all other
variable names to whatever they refer to in the static evaluation
context. This closure is the result of evaluation.

When the closure is actually called (e.g. by virtue of being the
designated `function` in a `call` node), a fresh execution context is
created, in which the actual arguments are bound to the formals. If
there are too few actual arguments, the call fails (terminating the
runtime). After that, the `statements` are evaluated in
order. Finally, if there is a `yield`, then that is evaluated. The
result of the call is the same as the result of the `yield` evaluation
(including possibly no value) if a `yield` was present, or no value if
there was no `yield` to evaluate.

#### `literal` &mdash; `[:@"literal" value:]`

This represents arbitrary literal data. The data payload is
an arbitrary value.

The data `value` is the result of evaluation, when a `literal`
node is run. Evaluation never fails.

#### `varRef` &mdash; `[:@"varRef" name:]`

This represents a by-name variable reference. `name` is an
arbitrary value, but is most typically a stringlet.

When run, this causes the `name` to be looked up in the current
execution context. If a binding is found for it, then the bound value
is the result of evaluation. If a binding is not found for it, then
evaluation fails (terminating the runtime).

<br><br>
### Other Nodes

These are node types that appear within the data payloads
of various expression nodes.

#### `formals` &mdash; `[:@"formals" declarations:]`

This represents the formal arguments to a function. `declarations`
must be a listlet, and each element of the listlet must be
a maplet that binds these two keys:

* `@"name"` &mdash; an arbitrary value (but typically a stringlet),
  which indicates the name of the variable to be bound for this
  argument.

* `@"repeat"` &mdash; indicates how many actual arguments are
  bound by this formal. It is one of:

  * `@"."` &mdash; indicates that this argument binds exactly one
    actual argument. The argument variable as bound is the same as the
    actual argument as passed.

  * `@"*"` &mdash; indicates that this argument binds as many actual
    arguments as are available. As such, this must only ever be the
    `repeat` of the last formal. The argument variable as bound is a
    listlet of all the passed actual arguments that were bound.

#### `varDef` &mdash; `[:@"varDef" @[@"name"=name @"value"=value]:]`

This represents a variable definition as part of a function body.
`name` is an arbitrary value (but typically a stringlet) representing
the name of the variable to define, and `value` must be an expression
node, indicating the value that the variable should be bound to.

When run, `value` is evaluated. If it does not evaluate to a value,
then evaluation fails (terminating the runtime). Otherwise, the
evaluated value is bound in the current (topmost) execution context to
the indicated `name`.


<br><br>
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
  line with `<>` (a "diamond") and then the type name of the type of
  value returned, or the specific value returned, or `.` (a dot)
  indicating "any possible value".

* If the function does not return a value, that is indicated by ending
  the line with `<> ~.`.

* If a function might or might not return a value, that is indicated
  by ending the line with `<> . | ~.`.

*Note:* The return value stuff is not actual *Samizdat Layer 0* syntax,
though it uses elements of it.


<br><br>
### Primitive Library: Value Definitions

#### `false`

The boolean value false. It can also be written as `[:@"boolean" @0:]`.

#### `true`

The boolean value true. It can also be written as `[:@"boolean" @1:]`.

<br><br>
### Primitive Library: Ultraprimitive Functions

This set of primitive functions are required in order to execute
parse trees as produced by `sam0Tree` (see which).

#### `makeHighlet type value? <> highlet`

Returns a highlet with the given type tag (an arbitrary value)
and optional data payload value (also an arbitrary value). These
equivalences hold for *Samizdat Layer 0* source code:

```
v = [:key:];         is equivalent to   v = makeHighlet key;
v = [:key value:];   is equivalent to   v = makeHighlet key value;
```

#### `makeListlet rest* <> listlet`

Returns a listlet with the given elements (in argument order).
These equivalences hold for *Samizdat Layer 0* source code:

```
v = @[v1];      is equivalent to   v = makeListlet v1;
v = @[v1 v2];   is equivalent to   v = makeListlet v1 v2;
[etc.]
```

*Note:* The equivalence requires at least one argument, even though
the function is happy to operate given zero arguments.

#### `makeMaplet rest* <> maplet`

Returns a maplet with the given key-value bindings (in argument
order), with each key-value pair represented as two consecutive
arguments. The number of arguments passed to this function must be
even. These equivalences hold for *Samizdat Layer 0* source code:

```
v = @[k1=v1];         is equivalent to   v = makeMaplet k1 v1;
v = @[k1=v1 k2=v2];   is equivalent to   v = makeMaplet k1 v1 k2 v2;
[etc.]
```

*Note:* The equivalence requires at least two arguments, even though
the function is happy to operate given zero arguments.

#### `makeUniqlet() <> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist). This equivalence holds for *Samizdat Layer 0* source
code:

```
v = @@;   is equivalent to   v = makeUniqlet();
```


<br><br>
### Primitive Library: Conditionals

#### `ifTrue predicate thenFunction elseFunction? <> . | ~.`

Primitive boolean conditional. This calls the given predicate with no
arguments, expecting it to return a boolean.

If the predicate returns `true`, then the `thenFunction` is called
with no arguments. If the predicate returns `false`, then the
`elseFunction` (if any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called. If no consequent was called, this
returns no value.

#### `ifValue function valueFunction voidFunction? <> . | ~.`

Primitive value conditional. This calls the given function with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunction` is called
with one argument, namely the value returned from the original
function. If the function does not return a value, then the
`voidFunction` (if any) is called with no arguments.

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
comparison function that returns a boolean. Without it (or something
like it), there would be no way to define boolean-returning
comparators in-language.

#### `lowSize value <> intlet`

Returns the "size" of the given value. Every low-layer value has
a size, defined as follows:

* `intlet` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an intlet is 1, which
  is the size of both `@0` and `@-1`.

* `stringlet` &mdash; the number of characters.

* `listlet` &mdash; the number of elements.

* `maplet` &mdash; the number of mappings (bindings).

* `uniqlet` &mdash; always `@0`.

* `highlet` &mdash; `@0` for a valueless highlet, or `@1` for a
  valued highlet.

#### `lowType value <> stringlet`

Returns the type name of the low-layer type of the given value. The
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

Returns a stringlet consisting of the concatenation of the contents
of the two argument stringlets, in argument order.

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
### Primitive Library: Listlets

#### `listletAdd listlet1 listlet2 <> listlet`

Returns a listlet consisting of the concatenation of the elements
of the two argument listlets, in argument order.

#### `listletAppend listlet value <> listlet`

Returns a listlet consisting of the elements of the given
listlet argument followed by the given additional value.

#### `listletDelNth listlet n <> listlet`

Returns a listlet just like the given one, except that the `n`th (zero-based)
element is deleted.

#### `listletPrepend value listlet <> listlet`

Returns a listlet consisting of the given first value followed by the
elements of the given listlet argument.

*Note:* The arguments are given in an order meant to reflect the
result (and not listlet-first).

#### `listletNth listlet n <> .`

Returns the `n`th (zero-based) element of the given listlet.

<br><br>
### Primitive Library: Maplets

#### `mapletAdd maplet1 mapet2 <> maplet`

Returns a maplet consisting of the combination of the mappings of the
two argument maplets. For any keys in common between the two maplets,
the second argument's value is the one that ends up in the result.

#### `mapletGet maplet key notFound? <> . | ~.`

Returns the value mapped to the given key (an arbitrary value) in
the given maplet. If there is no such mapping, then this
returns the `notFound` value (an arbitrary value) if supplied,
or simply returns no value at all if `notFound` was not supplied.

#### `mapletKeys maplet <> listlet`

Returns a listlet of all the keys in the given maplet, in sorted
order.

<br><br>
### Primitive Library: Highlets

#### `highletType highlet <> .`

Returns the type tag value (an arbitrary value) of the given highlet.

#### `highletValue highlet <> . | ~.`

Returns the payload data value (an arbitrary value) of the given
highlet, if any. Returns no value if the given highlet is valueless.

<br><br>
### Primitive Library: Functions and Code

#### `apply function listlet <> . | ~.`

Calls the given function with the given listlet as its arguments (that
is, each element of the listlet becomes a separate argument to the
function). This function returns whatever the called function returned
(including nothing).

#### `sam0Eval context expressionNode <> . | ~.`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document. Very notably, the
result of calling `sam0Tree` is valid as the `expressionNode` argument
here.

Evaluation is performed in an execution context that includes all of
the variable bindings indicated by `context`, which must be a maplet.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.

#### `sam0Tree stringlet <> functionNode`

Parses the given stringlet as a program written in *Samizdat Layer 0*.
Returns a `function` node (as defined by the parse tree semantics)
that represents the function.

<br><br>
### Primitive Library: Miscellaneous

#### `die stringlet <> (exits)`

Prints the given stringlet to the system console, and exits.

#### `readFile fileName <> stringlet`

Reads the named file (named by a stringlet) using the underlying
OS's functionality, interpreting the contents as UTF-8 encoded
text. Returns a stringlet of the read and decoded text.

#### `writeFile fileName text <> ~.`

Writes out the given text to the named file (named by a stringlet),
using the underlying OS's functionality, and encoding the text
(a stringlet) as a stream of UTF-8 bytes.

<br><br>
### In-Language Library: Value Definitions

#### `null`

A value used when no other value is suitable, but when a value is
nonetheless required. It can also be written as `[:@"null":]`.

<br><br>
### In-Language Library: Comparisons / Booleans

#### `eq value1 value2 <> boolean`

Checks for equality. Returns `true` iff the two given values are
identical.

#### `le value1 value2 <> boolean`

Checks for a less-than-or-equal relationship. Returns `true` iff the
first value orders before the second or is identical to it.

#### `lt value1 value2 <> boolean`

Checks for a less-than relationship. Returns `true` iff the first value
orders before the second.

#### `ge value1 value2 <> boolean`

Checks for a greater-than-or-equal relationship. Returns `true` iff the
first value orders after the second or is identical to it.

#### `gt value1 value2 <> boolean`

Checks for a greater-than relationship. Returns `true` iff the first value
orders after the second.

#### `ne value1 value2 <> boolean`

Checks for inequality. Returns `true` iff the two given values are not
identical.

#### `not boolean <> boolean`

Returns the opposite boolean to the one given.

*Note:* Only accepts boolean arguments.

<br><br>
### In-Language Library: Types

#### `isHighlet value <> boolean`

Returns `true` iff the given value is a highlet.

#### `isIntlet value <> boolean`

Returns `true` iff the given value is an intlet.

#### `isListlet value <> boolean`

Returns `true` iff the given value is a listlet.

#### `isMaplet value <> boolean`

Returns `true` iff the given value is a maplet.

#### `isStringlet value <> boolean`

Returns `true` iff the given value is a stringlet.

#### `isUniqlet value <> boolean`

Returns `true` iff the given value is a uniqlet.

<br><br>
### In-Language Library: Conditionals

#### `and predicate rest* <> boolean`

Short-circuit conjunction. Takes an arbitrary number of predicates,
each a no-argument function.  Calls each of them in turn until one of
them returns `false`, in which case this function also returns
`false`. If no predicate returns `false`, this function returns
`true`.

#### `else() <> true`

No-argument predicate that always returns `true`. Used idiomatically
when writing cascading `if` calls.

#### `if predicate consequent rest* <> . | ~.`

Cascading conditional. Takes an even number of arguments, alternating
predicates and consequents, each argument being a no-argument
function. The predicates are called in order until one returns
`true`. The consequent immediately after the `true` predicate then
gets called, and its return value becomes the result of this
function. If no predicate returns `true`, this function returns no
value.

#### `or predicate rest* <> boolean`

Short-circuit disjunction. Takes an arbitrary number of predicates,
each a no-argument function.  Calls each of them in turn until one of
them returns `true`, in which case this function also returns
`true`. If no predicate returns `true`, this function returns `false`.

<br><br>
### In-Language Library: Stringlets

#### `stringletCat stringlet rest* <> stringlet`

Concatenates one or more stringlets together into a single resulting
stringlet.

#### `stringletMap stringlet function <> listlet`

Maps each element of a stringlet using a mapping function, collecting
the results into a listlet (note, not into a stringlet). The given
function is called on each element (character as an intlet), with two
arguments, namely the element and its index number (zero-based).

*Note:* Unlike many other languages with similar functions, the
function argument is the *last* one and not the *first* one. This is
specifically done to make it natural to write a multi-line function
without losing track of the other two arguments.

#### `stringletReduce base stringlet function <> .`

Reduces a stringlet to a single value, given a base value and a
reducer function, operating in low-to-high index order (that is, this
is a left-reduce operation). The given function is called once per
stringlet element (character as an intlet), with three arguments: the
last (or base) reduction result, the element, and its index number
(zero-based).

See note on `stringletMap` about choice of argument order.

<br><br>
### In-Language Library: Listlets

#### `listletCat listlet rest* <> listlet`

Concatenates one or more listlets together into a single resulting
listlet.

#### `listletMap listlet function <> listlet`

Maps each element of a listlet using a mapping function, collecting
the results into a new listlet. The given function is called on each
listlet element, with two arguments, namely the element and its index
number (zero-based).

See note on `stringletMap` about choice of argument order.

#### `listletReduce base listlet function <> .`

Reduces a listlet to a single value, given a base value and a reducer
function, operating in low-to-high index order (that is, this is a
left-reduce operation). The given function is called on each listlet
element, with three arguments: the last (or base) reduction result,
the element, and its index number (zero-based).

See note on `stringletMap` about choice of argument order.

<br><br>
### In-Language Library: Miscellaneous

#### `sam0Library() <> maplet`

Returns a maplet of bindings of the entire *Samizdat Layer 0*
library. This is suitable for passing to `sam0Eval`, for example.

#### `sourceStringlet value <> stringlet`

Converts an arbitrary value into a stringlet representation form
that is meant to mimic the Samizdat source syntax.

*Note:* The output differs from Samizdat Layer 0 syntax in that
stringlet forms may get represented using hex escapes, which is not
defined syntax in *Samizdat Layer 0*.
