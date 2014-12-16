Samizdat Programming Language In N Shout-Outs
=============================================

Samizdat wouldn't exist without a lot of inspiring language work
embodied in dozens of systems developed over the better part of the
preceding century.

The language came about as a second-order effect of Danfuzz's
ongoing desire to explore the intersection of all of
[promise](http://en.wikipedia.org/wiki/Promise_%28programming%29)-based
[object-capability](http://en.wikipedia.org/wiki/Object-capability_model)
computation, the [actor model](http://en.wikipedia.org/wiki/Actor_model) of
concurrency, maximally pure / immutable data models, and [software
transactional
memory](http://en.wikipedia.org/wiki/Software_transactional_memory) (STM),
having first learned of the power of this combination when working on
[E](https://en.wikipedia.org/wiki/E_%28programming_language%29). The
Samizdat language has a mostly-immutable data model and generally has
"capability nature," but it does not innately implement "real" promises,
actors, or STM (yet). It is intended to be a fertile substrate on which to
explore these topics, while still being a practical and compelling language
in its own right.

In the context of project sponsor [Nextbit Systems](http://nextbit.com),
Samizdat is an exploration of programming language design in
the context of a multi-device "personal cloud" operating system.

Samizdat's syntax can be traced in part to
[ALGOL](http://en.wikipedia.org/wiki/ALGOL), through
[C](http://en.wikipedia.org/wiki/C_%28programming_language%29),
[Java](http://en.wikipedia.org/wiki/Java_%28programming_language%29),
[JavaScript](http://en.wikipedia.org/wiki/JavaScript), and E.
Its other syntactic influences include
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk),
[Scala](https://en.wikipedia.org/wiki/Scala_%28programming_language%29),
and traditional
[Backus-Naur Form](http://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form)
(BNF), with an honorable mention to
[Logo](http://en.wikipedia.org/wiki/Logo_%28programming_language%29) for
early inspiration.

Samizdat's data and execution models probably owe their biggest debts to
Smalltalk, JavaScript,
[Scheme](http://en.wikipedia.org/wiki/Scheme_%28programming_language%29),
[Clojure](http://en.wikipedia.org/wiki/Clojure), and
[Lisp](http://en.wikipedia.org/wiki/LISP) in general.
[Icon](http://en.wikipedia.org/wiki/Icon_%28programming_language%29),
[Haskell](http://en.wikipedia.org/wiki/Haskell_%28programming_language%29),
[Erlang](http://en.wikipedia.org/wiki/Erlang_%28programming_language%29),
and [Python](http://en.wikipedia.org/wiki/Python_%28programming_language%29)
all also provided a fair amount of grist for the mill.

Samizdat's parsing semantics were influenced by the recent work on
[Parsing Expression
Grammars](http://en.wikipedia.org/wiki/Parsing_expression_grammar) (PEGs),
particularly [OMeta](http://tinlizzie.org/ometa/), tempered by a lot
of practical experience with [ANTLR](http://en.wikipedia.org/wiki/ANTLR),
[Awk](http://en.wikipedia.org/wiki/AWK),
[Lex](http://en.wikipedia.org/wiki/Lex_%28software%29) /
[Flex](http://en.wikipedia.org/wiki/Flex_lexical_analyser),
and [Yacc](http://en.wikipedia.org/wiki/Yacc) /
[Bison](http://en.wikipedia.org/wiki/GNU_bison). Additionally,
[SNOBOL](http://en.wikipedia.org/wiki/SNOBOL) deserves credit for blazing
the trail for general-purpose languages that make parsing a first-class
operation.

The implementation tactics used to build Samizdat were / will be informed by
[Scheme-48](http://en.wikipedia.org/wiki/Scheme_48),
the [Jikes RVM](http://en.wikipedia.org/wiki/Jikes_RVM)
(formerly a.k.a. Jalape&ntilde;o),
[Squeak](http://en.wikipedia.org/wiki/Squeak), and [Objects In
C](http://en.wikipedia.org/wiki/John_Wainwright_%28computer_scientist%29);
with a tip o' the hat to each of [PyPy](http://en.wikipedia.org/wiki/PyPy) and
[Chicken](http://en.wikipedia.org/wiki/Chicken_%28Scheme_implementation%29).
