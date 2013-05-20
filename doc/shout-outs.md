Samizdat Programming Language In N Shout-Outs
=============================================

Samizdat wouldn't exist without a lot of inspiring language work
embodied in dozens of systems developed over the better part of the
preceding century.

The language came about as a second-order effect of the author's
desire to explore the intersection of all of
[promise](http://en.wikipedia.org/wiki/Promise_%28programming%29)-based
[object-capability](http://en.wikipedia.org/wiki/Object-capability_model)
computation, the [actor model](http://en.wikipedia.org/wiki/Actor_model) of
concurrency, maximally pure / immutable data models, and [software transactional
memory](http://en.wikipedia.org/wiki/Software_transactional_memory)
(STM). The Samizdat language has a mostly-immutable data model and
generally has "capability nature", but it does not innately implement
promises, actors, or STM. However, it is intended to be a fertile
substrate on which to explore these topics, while still being a
practical and compelling language in its own right.

Samizdat's syntax can be traced in part to
[ALGOL](http://en.wikipedia.org/wiki/ALGOL), through
[C](http://en.wikipedia.org/wiki/C_%28programming_language%29),
[Java](http://en.wikipedia.org/wiki/Java_%28programming_language%29),
[JavaScript](http://en.wikipedia.org/wiki/JavaScript), and
[E](http://en.wikipedia.org/wiki/E_%28programming_language%29). Its
other syntactic influences include
[Logo](http://en.wikipedia.org/wiki/Logo_%28programming_language%29),
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk), and traditional
[Backus-Naur Form](http://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form)
(BNF).

Samizdat's execution model probably owes its biggest debt to Smalltalk.

The Samizdat core data model was influenced by
[Lisp](http://en.wikipedia.org/wiki/LISP),
[Haskell](http://en.wikipedia.org/wiki/Haskell_%28programming_language%29),
[Clojure](http://en.wikipedia.org/wiki/Clojure), and JavaScript.

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

The Samizdat core library was influenced by
[Scheme](http://en.wikipedia.org/wiki/Scheme_%28programming_language%29),
Logo, and Awk.

The implementation tactics used to build Samizdat were informed by
[Scheme-48](http://en.wikipedia.org/wiki/Scheme_48),
the [Jikes RVM](http://en.wikipedia.org/wiki/Jikes_RVM)
(formerly a.k.a. Jalape&ntilde;o), and
[Squeak](http://en.wikipedia.org/wiki/Squeak), with a tip o' the
hat to each of [PyPy](http://en.wikipedia.org/wiki/PyPy) and
[Chicken](http://en.wikipedia.org/wiki/Chicken_%28Scheme_implementation%29).
