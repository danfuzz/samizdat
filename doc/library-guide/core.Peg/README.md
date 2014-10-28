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

When building tokenizers, the input elements are taken to be in the form
of character-as-token items. Each element is a token whose name (tag) is
a single-string character (and whose value if any is irrelevant for the
parsing mechanism). There are helper functions which take strings and
automatically convert them into this form.

When building tree parsers, the input elements are expected to be
tokens per se, that is, records whose name tag is taken to indicate a
token type.

The output of the functions named `$Peg::make*` are all parsing rules. These
are all objects with a class that binds the `parse` method. A `parse` method
accepts at least two arguments, and will also accept additional arguments
depending on context:

* `box` &mdash; The first argument is a `box` into which a successful
  result of parsing is to be `store`d.

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

A parsing function, upon failure when called, must do one thing, namely
return void. It must *not*, upon failure, call `store` on its given `box`.

In the following descriptions, code shorthands use the Samizdat parsing
syntax for explanatory purposes.


<br><br>
### Classes

* [PegAny](PegAny.sam)
* [PegChoice](PegChoice.sam)
* [PegCode](PegCode.sam)
* [PegEof](PegEof.sam)
* [PegFail](PegFail.sam)
* [PegLookaheadFailure](PegLookaheadFailure.sam)
* [PegLookaheadSuccess](PegLookaheadSuccess.sam)
* [PegMain](PegMain.sam)
* [PegRepeat](PegRepeat.sam)
* [PegResult](PegResult.sam)
* [PegSequence](PegSequence.sam)
* [PegThunk](PegThunk.sam)
* [PegTokenSet](PegTokenSet.sam)
* [PegTokenSetComplement](PegTokenSetComplement.sam)


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

#### `.parse(box, input, items*) -> newInput`

Performs a parse of `input` (a generator) with the trailing sequence
context of `[items*]`. If parsing is successful, stores into `box` the
parsed result and returns a replacement for `input` that reflects the
consumption of tokens that were used. If parsing fails, does not
store anything into `box` and returns void.


<br><br>
### Functions

#### `apply(rule, input) -> . | void`

Applies a parser rule to the given input, yielding whatever result the
rule yields on the input.

`input` must be a generator (including possibly a collection).
If it is a string, this function automatically treats it as a generator of
character-as-token values.

#### `stringFromTokenList(tokens) -> string`

Takes a list of tokenizer-style character tokens (that is, records whose
names are each a single-character-long symbols), returning the result of
concatenating all the characters together in order, as a string.

This function is intended to aid in the building of tokenizers.

#### `symbolFromTokenList(tokens) -> symbol`

Like `stringFromTokenList`, except returns an interned symbol instead of
a string.
