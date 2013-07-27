Samizdat Layer 0: Core Library
==============================

Parsing
-------

*Samizdat Layer 0* provides a set of "parsing expression grammar" (a.k.a.
"PEG") functions, for use in building parsers. The language does not provide
any syntactic support for using these, though. (That is the job of a higher
layer of the language.)

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

The output of the functions named `pegMake*` are all parsing rules. These
are functions with specifically-defined behavior in terms of accepted
arguments and return values. In particular, every rule accepts at least
two arguments, `yield` and `state` (in that order).

* `yield` is a function that a rule calls to indicate the result of
  parsing. It behaves similarly to the `yield` functions defined as part
  of the `object` construct. A parser rule is expected to call yield
  both on success (with a value argument) and failure (with no argument).

* The `state` argument is a list of items (tokens per se or character-as-token
  elements) yet to be parsed.

In addition, every rule when called in the context of a "sequence" is passed
extra arguments which correspond to the parsed results that are in "scope"
of the rule (in order). This can be used, in particular, in the implementation
of "code" rules in order to produce a filtered result.

On success, a rule is expected to return a replacement for `state` to be
used as the `state` argument for subsequent parser rules. On failure, a
rule is expected to return void.

In the following descriptions, code shorthands use the Samizdat parsing
syntax for explanatory purposes. Keep in mind, however, that this is *not*
syntax that is built into *Samizdat Layer 0*.


<br><br>
### Primitive Definitions

(none)


<br><br>
### In-Language Definitions

#### `pegApply(rule, input) <> . | void`

Applies a parser rule to the given input, yielding whatever result the
rule yields on the input.

`input` must be a generator or a "genaratable" value. If it is a string,
this function automatically treats it as a generator of
character-as-token values.

#### `pegGenerator(rule, input) <> generator`

Creates a generator which, when iterated, tries to match the given
`rule` on the given `input`. The result basically acts as a single
step in matching the rule `rule*` (that is, zero or more matches
of `rule`), yielding each match in a separate iteration.

Applies a parser rule to the given input, yielding whatever result the
rule yields on the input.

`input` must be a generator or a "genaratable" value. If it is a string,
this function automatically treats it as a generator of
character-as-token values.

#### `pegMakeCharSet(strings*) <> rule`

Makes and returns a parser rule which matches any character of any of
the given strings, consuming it upon success. Each argument must be
a string. The result of successful parsing is a character-as-token of the
parsed character.

This function exists primarily to aid in "by hand" parser implementation
in *Samizdat Layer 0* (as opposed to using the higher layer syntax), as
it is merely a convenient wrapper for a call to `pegMakeTokenSet` (see
which).

This is equivalent to the syntactic form `{/ ["string1" "string2" "etc"] /}`.

#### `pegMakeCharSetComplement(string*) <> rule`

Makes and returns a parser rule which matches any character *except*
one found in any of the given strings, consuming it upon success.
Each argument must be a string. The result of successful parsing is a
character-as-token of the parsed character.

This function exists primarily to aid in "by hand" parser implementation
in *Samizdat Layer 0* (as opposed to using the higher layer syntax), as
it is merely a convenient wrapper for a call to `pegMakeTokenSetComplement`
(see which).

This is equivalent to the syntactic form
`{/ [! "string1" "string2" "etc."] /}`.

#### `pegMakeChoice(rules*) <> rule`

Makes and returns a parser rule which performs an ordered choice amongst
the given rules. Upon success, it passes back the yield and replacement
state of whichever alternate rule succeeded.

This is equivalent to the syntactic form `{/ rule1 | rule2 | etc /}`.

#### `pegMakeCode(function) <> rule`

Makes and returns a parser rule which runs the given function. `function`
must be a function. When called, it is passed as arguments all the
in-scope matched results from the current sequence context. Whatever it
returns becomes the yielded value of the rule. If it returns void, then
the rule is considered to have failed. Code rules never consume any
input.

This is equivalent to the syntactic form `{/ ... { arg1 arg2 etc :: code } /}`.

#### `pegMakeLookaheadFailure(rule) <> rule`

Makes and returns a parser rule which runs a given other rule, suppressing
its usual yield and state update behavior. Instead, if the other rule
succeeds, this rule fails. And if the other rule fails, this one succeeds,
yielding `null` and consuming no input.

This is equivalent to the syntactic form `{/ !rule /}`.

#### `pegMakeLookaheadSuccess(rule) <> rule`

Makes and returns a parser rule which runs a given other rule, suppressing
its usual state update behavior. Instead, if the other rule succeeds, this
rule also succeeds, yielding the same value but *not* consuming any input.

This is equivalent to the syntactic form `{/ &rule /}`.

#### `pegMakeMainChoice(rules*) <> rule`

Makes and returns a parser rule which tries the given rules in order until
one succeeds. This is identical to `pegMakeChoice` (see which), except that
it provides a fresh (empty) parsed item scope.

This is equivalent to the syntactic form `{/ rule1 | rule2 | etc /}`.

#### `pegMakeMainSequence(rules*) <> rule`

Makes and returns a parser rule which runs a sequence of given other rules
(in order). This is identical to `pegMakeSequence` (see which), except that
it provides a fresh (empty) parsed item scope.

This is equivalent to the syntactic form `{/ rule1 rule2 etc /}`.

#### `pegMakeOpt(rule) <> rule`

Makes and returns a parser rule which optionally matches a given rule.
When called, the given other rule is matched whenever possible. This
rule always succeeds. If the other rule succeeds, this rule yields a
single-element list of the successful yield of the other rule, and it
returns updated state to reflect the successful parse. If the other
rule fails, this one yields an empty list and does not consume any input.

This is equivalent to the syntactic form `{/ rule? /}`.

#### `pegMakePlus(rule) <> rule`

Makes and returns a parser rule which matches a given rule repeatedly.
When called, the given other rule is matched as many times as possible.
It yields a list of all the matched results (in order), and it returns
updated state that reflects all the input consumed by these matches.
This rule will succeed only if the given rule is matched at least once.

This is equivalent to the syntactic form `{/ rule+ /}`.

#### `pegMakeSequence(rules*) <> rule`

Makes and returns a parser rule which runs a sequence of given other rules
(in order). This rule is successful only when all the given rules
also succeed. When successful, this rule yields the value yielded by the
last of its given rules, and returns updated state that reflects the
parsing in sequence of all the rules.

Each rule is passed a list of in-scope parsing results, starting with the
first result from the "closest" enclosing main sequence.

This is equivalent to the syntactic form `{/ ... (rule1 rule2 etc) ... /}`.

#### `pegMakeStar(rule) <> rule`

Makes and returns a parser rule which matches a given rule repeatedly.
When called, the given other rule is matched as many times as possible.
It yields a list of all the matched results (in order), and it returns
updated state that reflects all the input consumed by these matches.
This rule always succeeds, including if the given rule is never matched,
in which case this rule yields the empty list.

This is equivalent to the syntactic form `{/ rule* /}`.

#### `pegMakeString(string) <> rule`

Makes and returns a parser rule which matches a sequence of characters
exactly, consuming them from the input upon success. `string` must be a
string. The result of successful parsing is a valueless token with
`string` as its type tag.

This is equivalent to the syntactic form `{/ "string" /}`.

#### `pegMakeToken(type) <> rule`

Makes and returns a parser rule which matches any token with the same
type as given. `type` is an arbitrary value, but is typically
a string. Upon success, the rule consumes and yields the matched token.

This is also used to match single characters in tokenizers.

This is equivalent to the syntactic form `{/ @token /}` or `{/ "ch" /}`
(where `ch` represents a single character).

#### `pegMakeTokenSet(types*) <> rule`

Makes and returns a parser rule which matches a token whose type
matches that of any of the given types, consuming it upon success.
Each argument is taken to be a token type, which is typically
(but not necessarily) a string. The result of successful parsing is
whatever token was matched.

This is equivalent to the syntactic form `{/ [@token1 @token2 @etc] /}`.

#### `pegMakeTokenSetComplement(types*) <> rule`

Makes and returns a parser rule which matches a token whose type
matches none of any of the given tokens, consuming it upon success.
Each argument is taken to be a token type, which is typically
(but not necessarily) a string. The result of successful parsing is
whatever token was matched.

This is equivalent to the syntactic form `{/ [! @token1 @token2 @etc] /}`.

#### Rule: `pegRuleAny`

Parser rule which matches any input item, consuming and yielding it. It
succeeds on any non-empty input.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ . /}`.

#### Rule: `pegRuleEmpty`

Parser rule which always succeeds, and never consumes input. It always
yields `null`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ () /}`.

#### Rule: `pegRuleEof`

Parser rule which succeeds only when the input is empty. When successful,
it always yields `null`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ !. /}`.

#### Rule: `pegRuleFail`

Parser rule which always fails.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ !() /}` (that is, attempting
to find a lookahead failure for the empty rule, said rule which always
succeeds). It is also equivalent to the syntactic form `{/ [] /}` (that is,
the empty set of tokens or characters).

#### Rule: `pegRuleLookaheadAny`

Parser rule which matches any input item, yielding it but not consuming it.
It succeeds on any non-empty input.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{/ &. /}`.
