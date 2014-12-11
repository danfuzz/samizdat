Samizdat Layer 0: Core Library
==============================

core.Peg (parsing)
------------------

Samizdat provides a module of "parsing expression grammar" (a.k.a.
"PEG") functions, for use in building parsers. Samizdat Layer 1
provides syntactic support for using these functions, and they can be
used directly in Samizdat Layer 0.

These functions can be used to build both tokenizers (that is, parsers of
strings / sequences of characters) and tree parsers (that is, parsers of
higher-level tokens with either simply a tree-like rule invocation or
to produce tree structures per se).

When building tokenizers, the input elements are taken to be in the form of
character-as-token items. Each element is a token whose name (tag) is a
single-string character (and whose payload value if any is irrelevant for the
parsing mechanism). There are helper functions which take strings and
automatically convert them into this form.

When building tree parsers, the input elements are expected to be tokens per
se, that is, records whose name (tag) is taken to indicate a token type.

Almost all of the classes in this module are for parser rules. A parser rule
is an object that binds the `.parse()` method, taking as a single argument
a `ParserState` value and returning either another `ParserState` or void.
A `ParserState` encapsulates a generator of input values (e.g. but not
necessarily a list) and a list (per se) containing the "trailing context" of
matched items.

In the descriptions of the various parsers in this module, code shorthands
use the Samizdat parsing syntax (`{: ... :}`) for explanatory purposes.


<br><br>
### Classes

* [BasicState](BasicState.md)
* [CacheState](CacheState.md)
* [PegAny](PegAny.md)
* [PegChoice](PegChoice.md)
* [PegCode](PegCode.md)
* [PegEof](PegEof.md)
* [PegFail](PegFail.md)
* [PegLookaheadFailure](PegLookaheadFailure.md)
* [PegLookaheadSuccess](PegLookaheadSuccess.md)
* [PegMain](PegMain.md)
* [PegRepeat](PegRepeat.md)
* [PegResult](PegResult.md)
* [PegSet](PegSet.md)
* [PegSetComplement](PegSetComplement.md)
* [PegSequence](PegSequence.md)
* [PegThunk](PegThunk.md)


<br><br>
### Constants

#### Rule: `any`

Parser rule which matches any input item, consuming and yielding it. It
succeeds on any non-empty input. It is an instance of `PegAny`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{: . :}`.

#### Rule: `empty`

Parser rule which always succeeds, and never consumes input. It always
yields `null`. It is an instance of `PegResult`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{: () :}`.

#### Rule: `eof`

Parser rule which succeeds only when the input is empty. When successful,
it always yields `null`. It is an instance of `PegEof`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{: !. :}`.

#### Rule: `fail`

Parser rule which always fails. It is an instance of `PegFail`.

This is a direct parser rule, meant to be referred to by value instead of
called directly.

This is equivalent to the syntactic form `{: !() :}` (that is, attempting
to find a lookahead failure for the empty rule, said rule which always
succeeds). It is also equivalent to the syntactic form `{: [] :}` (that is,
the empty set of tokens or characters).


<br><br>
### Method Definitions: `Parser` protocol

#### `.parse(state) -> isa ParserState | void`

Performs a parse of `state` (a parser state object). If successful, returns a
new state that represents the successful parse. If unsuccessful, returns void.

A successful call to this method is expected to "consume" zero or more
items from the input generator and add *exactly* one item to the trailing
context.


<br><br>
### Method Definitions: `ParserState` protocol

#### `.addContext(value) -> isa ParserState`

Returns an instance just like `this`, except with `value` appended to the
end of its trailing context.

#### `.applyRule(rule) -> isa ParserState | void`

Applies the given `rule`, either by invoking its `.parse()` method, passing
`this` as the state argument, or by doing (in some way) the equivalent
thereof. Returns whatever `rule.parse()` returns (or equivalently would
have returned).

#### `.get_context() -> isa List`

Gets the trailing context of `this`.

#### `.shiftInput() -> isa ParserState | void`

Returns an instance just like `this`, except with the first element of the
input "consumed" and appended to the trailing context.

#### `.withContext(list) -> isa ParserState`

Returns an instance just like `this`, except with its trailing context
replaced with `list`, which must be a list.

<br><br>
### Functions

#### `apply(rule, input) -> . | void`

Applies a parser rule to the given input, yielding whatever result the rule
yields on the input.

`input` must be a generator (including possibly a collection). If it is a
string, this function automatically treats it as a generator of
character-as-token values.

This function creates a `BasicState` parser state object to wrap `input`, and
then in turn uses that to apply the `rule`.

#### `stringFromTokenList(tokens) -> isa String`

Takes a list of tokenizer-style character tokens (that is, records whose
names are each a single-character-long symbols), returning the result of
concatenating all the characters together in order, as a string.

This function is intended to aid in the building of tokenizers.

#### `symbolFromTokenList(tokens) -> isa Symbol`

Like `stringFromTokenList`, except returns an interned symbol instead of
a string.
