Samizdat Layer 0: Core Library
==============================

core::Peg (parsing)
-------------------

*Samizdat* provides a module of "parsing expression grammar" (a.k.a.
"PEG") functions, for use in building parsers. *Samizdat Layer 1*
provides syntactic support for using these functions, and they can be
used directly in *Samizdat Layer 0*.

These functions can be used to build both tokenizers (that is, parsers of
strings / sequences of characters) and tree parsers (that is, parsers of
higher-level tokens with either simply a tree-like rule invocation or
to produce tree structures per se).

When building tokenizers, the input elements are taken to be in the form
of character-as-token items. Each element is a token whose type tag is
a single-string character (and whose value if any is irrelevant for the
parsing mechanism). There are helper functions which take strings and
automatically convert them into this form.

When building tree parsers, the input elements are expected to be
tokens per se, that is, tokens whose type tag is taken to indicate a
token type.

The output of the functions named `Peg::make*` are all parsing rules. These
are all transparent derived values with a type that binds the `parse`
generic. A `parse` method accepts at least two arguments, and may also
accept additional arguments:

* `box` &mdash; The first argument is a `box` into which the result of
  parsing is to be `store`d, or to which void is stored in case of
  parsing failure.

* `input` &mdash; The second argument is a generator of input tokens to
  be parsed (usually *partially* parsed).

* `items*` &mdash; Any further arguments are taken to be the context of
  items that have been parsed already, in order, in the context of a
  "sequence" being parsed. These can be used, in particular, in the
  implementation of "code" rules in order to produce a filtered sequence
  result.

A parsing function, upon success when called, must do two things: It must
call `store` on its yield `box` to indicate the non-void result of parsing.
Then, it must return a replacement `input` for use as the input to subsequent
parsers, that reflects the removal of whatever elements were consumed
by the parsing (including none). If the parsing function consumed all of
its given input, then it must return a voided generator (that is, one which
yields and returns void when called).

A parsing function, upon failure when called, must do two things: It must
call its yield `box` with no argument to indicate a void result of parsing.
Then, it must also return void.

In the following descriptions, code shorthands use the *Samizdat* parsing
syntax for explanatory purposes.

<br><br>
### Types

The following is a summary of the parser types used here, including what
data payload they use and what it means to call `parse` on them.

#### `@PegAny.parse(...)`

Consumes and yields a single token if available.

#### `@[PegChoice: [rules*]].parse(...)`

Tries to parse the input using each of the given `rules` in order.
Yields a value and returns replacement input based on the first one
that succeeds. Fails if none of the rules succeeds.

#### `@[PegCode: function].parse(...)`

Calls the given `function`, passing it the current sequential input
context. Yields whatever value it returns. Upon success (non-void
return), this returns the originally given input (consuming no tokens)
Upon failure, this returns void.

#### `@PegEmpty.parse(...)`

Always yields `null`, consuming no input.

#### `@PegEof.parse(...)`

Yields `null` if there is no input to consume. Otherwise, fails (yields
void and returns void).

#### `@PegFail.parse(...)`

Always yields void and returns void.

#### `@[PegLookaheadFailure: rule].parse(...)`

Tries to parse the input using `rule`. If it succeeds, then this rule
fails. If it fails, then this rule succeeds, consuming no input and
yielding `null`.

#### `@[PegLookaheadSuccess: rule].parse(...)`

Tries to parse the input using `rule`. If it succeeds, then this rule
also succeeds, yielding the same result but consuming no input. If it fails,
then this rule also fails.

#### `@[PegMain: rule].parse(...)`

Parses the given `rule` but does not hand it any `items*` that this rule
may have been passed. That is, this rule provides a new "main context"
for parsing. The yield and result of this rule is the same as that of
the embedded `rule`.

#### `@[PegOpt: rule].parse(...)`

Tries to parse the input using `rule`. If it succeeds, then this rule
also succeeds, yielding a single-element list of the result.
If it fails, then this rule succeeds, consuming no input and yielding
`[]`.

#### `@[PegRepeat: {rule: rule, (minSize: n)?}].parse(...)`

Tries to parse the input using `rule`, iterating until `rule` is no longer
able to be parsed. Yields a list of all the parsed results, and returns
the final input state. If `rule` was never found to apply, this yields
`[]` and returns the original input. If `minSize` is specified, then
this rule fails unless the size of the yielded list is at least `n`.

**Note:** This is used as the type underlying both `expr*` and `expr+`
expressions.

#### `@[PegResult: value].parse(...)`

Always yields the given `value`, consuming no input.

#### `@[PegSequence: [rules*]].parse(...)`

Tries to parse the given rules one after the other. Succeeds if all of
the rules were successfully applied, yielding the result of the *final*
rule and returning the final resulting new input value.

If any of the rules fail, then this rule also fails, consuming no input.

In addition to the original `items*` passed in to this rule, each sub-rule
receives the results of all the previous sub-rules as additional `items*`.

#### `@[PegTokenSet: {types*: null}].parse(...)`

If there is any input available, checks the type of the first input
token against the given set of types (map where only the keys matter).
If the type is found in the set, then yields and consumes the token.
Otherwise fails, yielding and returning void.

#### `@[PegTokenSetComplement: {types*: null}].parse(...)`

If there is any input available, checks the type of the first input
token against the given set of types (map where only the keys matter).
If the type is *not* found in the set, then yields and consumes the token.
Otherwise fails, yielding and returning void.


<br><br>
### Generic Function Definitions: `Parser` protocol

#### `parse(box, input, items*) <> newInput`

Performs a parse of `input` (a generator) with the trailing sequence
context of `[items*]`. If parsing is successful, stores into `box` the
parsed result and returns a replacement for `input` that reflects the
consumption of tokens that were used. If parsing fails, stores void
into `box` and returns void.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `apply(rule, input) <> . | void`

Applies a parser rule to the given input, yielding whatever result the
rule yields on the input.

`input` must be a generator (including possibly a collection).
If it is a string, this function automatically treats it as a generator of
character-as-token values.

#### `makeCharSet(strings*) <> rule`

Makes and returns a parser rule which matches any character of any of
the given strings, consuming it upon success. Each argument must be
a string. The result of successful parsing is a character-as-token of the
parsed character.

This function exists primarily to aid in "by hand" parser implementation
in *Samizdat Layer 0* (as opposed to using the higher layer syntax), as
it is merely a convenient wrapper for a call to `makeTokenSet` (see
which).

This is equivalent to the syntactic form `{/ ["string1" "string2" "etc"] /}`.

#### `makeCharSetComplement(string*) <> rule`

Makes and returns a parser rule which matches any character *except*
one found in any of the given strings, consuming it upon success.
Each argument must be a string. The result of successful parsing is a
character-as-token of the parsed character.

This function exists primarily to aid in "by hand" parser implementation
in *Samizdat Layer 0* (as opposed to using the higher layer syntax), as
it is merely a convenient wrapper for a call to `makeTokenSetComplement`
(see which).

This is equivalent to the syntactic form
`{/ [! "string1" "string2" "etc."] /}`.

#### `makeChoice(rules*) <> rule`

Makes and returns a parser rule which performs an ordered choice amongst
the given rules. Upon success, it passes back the yield and replacement
state of whichever alternate rule succeeded.

This is equivalent to the syntactic form `{/ rule1 | rule2 | etc /}`.

#### `makeCode(function) <> rule`

Makes and returns a parser rule which runs the given function. `function`
must be a function. When called, it is passed as arguments all the
in-scope matched results from the current sequence context. Whatever it
returns becomes the yielded value of the rule. If it returns void, then
the rule is considered to have failed. Code rules never consume any
input.

This is equivalent to the syntactic form `{/ ... { arg1 arg2 etc -> code } /}`.

#### `makeLookaheadFailure(rule) <> rule`

Makes and returns a parser rule which runs a given other rule, suppressing
its usual yield and state update behavior. Instead, if the other rule
succeeds, this rule fails. And if the other rule fails, this one succeeds,
yielding `null` and consuming no input.

This is equivalent to the syntactic form `{/ !rule /}`.

#### `makeLookaheadSuccess(rule) <> rule`

Makes and returns a parser rule which runs a given other rule, suppressing
its usual state update behavior. Instead, if the other rule succeeds, this
rule also succeeds, yielding the same value but *not* consuming any input.

This is equivalent to the syntactic form `{/ &rule /}`.

#### `makeMainChoice(rules*) <> rule`

Makes and returns a parser rule which tries the given rules in order until
one succeeds. This is identical to `makeChoice` (see which), except that
it provides a fresh (empty) parsed item scope.

This is equivalent to the syntactic form `{/ rule1 | rule2 | etc /}`.

#### `makeMainSequence(rules*) <> rule`

Makes and returns a parser rule which runs a sequence of given other rules
(in order). This is identical to `makeSequence` (see which), except that
it provides a fresh (empty) parsed item scope.

This is equivalent to the syntactic form `{/ rule1 rule2 etc /}`.

#### `makeOpt(rule) <> rule`

Makes and returns a parser rule which optionally matches a given rule.
When called, the given other rule is matched whenever possible. This
rule always succeeds. If the other rule succeeds, this rule yields a
single-element list of the successful yield of the other rule, and it
returns updated state to reflect the successful parse. If the other
rule fails, this one yields an empty list and does not consume any input.

This is equivalent to the syntactic form `{/ rule? /}`.

#### `makePlus(rule) <> rule`

Makes and returns a parser rule which matches a given rule repeatedly.
When called, the given other rule is matched as many times as possible.
It yields a list of all the matched results (in order), and it returns
updated state that reflects all the input consumed by these matches.
This rule will succeed only if the given rule is matched at least once.

This is equivalent to the syntactic form `{/ rule+ /}`.

#### `makeResult(value) <> rule`

Makes and returns a parser rule which always succeeds, yielding the
given result `value`, and never consuming any input.

This is equivalent to the syntactic form `{/ { <> value } /}` assuming
that `value` is a constant expression.

#### `makeSequence(rules*) <> rule`

Makes and returns a parser rule which runs a sequence of given other rules
(in order). This rule is successful only when all the given rules
also succeed. When successful, this rule yields the value yielded by the
last of its given rules, and returns updated state that reflects the
parsing in sequence of all the rules.

Each rule is passed a list of in-scope parsing results, starting with the
first result from the "closest" enclosing main sequence.

This is equivalent to the syntactic form `{/ ... (rule1 rule2 etc) ... /}`.

#### `makeStar(rule) <> rule`

Makes and returns a parser rule which matches a given rule repeatedly.
When called, the given other rule is matched as many times as possible.
It yields a list of all the matched results (in order), and it returns
updated state that reflects all the input consumed by these matches.
This rule always succeeds, including if the given rule is never matched,
in which case this rule yields the empty list.

This is equivalent to the syntactic form `{/ rule* /}`.

#### `makeString(string) <> rule`

Makes and returns a parser rule which matches a sequence of characters
exactly, consuming them from the input upon success. `string` must be a
string. The result of successful parsing is a valueless token with
`string` as its type tag.

This is equivalent to the syntactic form `{/ "string" /}`.

#### `makeToken(type) <> rule`

Makes and returns a parser rule which matches any token with the same
type as given. `type` is an arbitrary value, but is typically
a string. Upon success, the rule consumes and yields the matched token.

This is also used to match single characters in tokenizers.

This is equivalent to the syntactic form `{/ @token /}` or `{/ "ch" /}`
(where `ch` represents a single character).

#### `makeTokenSet(types*) <> rule`

Makes and returns a parser rule which matches a token whose type
matches that of any of the given types, consuming it upon success.
Each argument is taken to be a token type, which is typically
(but not necessarily) a string. The result of successful parsing is
whatever token was matched.

This is equivalent to the syntactic form `{/ [@token1 @token2 @etc] /}`.

#### `makeTokenSetComplement(types*) <> rule`

Makes and returns a parser rule which matches a token whose type
matches none of any of the given tokens, consuming it upon success.
Each argument is taken to be a token type, which is typically
(but not necessarily) a string. The result of successful parsing is
whatever token was matched.

This is equivalent to the syntactic form `{/ [! @token1 @token2 @etc] /}`.

#### Rule: `any`

Parser rule which matches any input item, consuming and yielding it. It
succeeds on any non-empty input.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ . /}`.

#### Rule: `empty`

Parser rule which always succeeds, and never consumes input. It always
yields `null`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ () /}`.

#### Rule: `eof`

Parser rule which succeeds only when the input is empty. When successful,
it always yields `null`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ !. /}`.

#### Rule: `fail`

Parser rule which always fails.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ !() /}` (that is, attempting
to find a lookahead failure for the empty rule, said rule which always
succeeds). It is also equivalent to the syntactic form `{/ [] /}` (that is,
the empty set of tokens or characters).
