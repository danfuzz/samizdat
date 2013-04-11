Samizdat
========

Watch this space!

Plan of attack
--------------

* `c-api/sam-data.h` &mdash; Public interface to Samizdat data model.

  Written in C.

* `libsam-data-0` &mdash; Data structure C library, "minimal" edition.

  Has functions to (a) build up low-layer data values piece by piece,
  and (b) access that data in various straightforward ways. Doesn't
  do memory management at all. (That is, it allocates but never GCs.)

  Written in C, implements API defined by `sam-data.h`.

* `samizdat-0` &mdash; Layer 0 (minimal) language implementation.

  Tool that accepts input in the form of minimalistic source code, and
  runs it. The language is meant to be a "parti" of final Samizdat,
  that is, it embodies *just* the main thrusts of the language with
  very little embellishment. The language it accepts is called
  *Samizdat Layer 0*.

  The associated core library includes a conditional (implementation
  of a cascading `if...else if...`), simple file I/O, and not much
  else.

  Written in C using `libsam-data-0`.

* `samizdat-1` &hellip; `samizdat-N` &mdash; Layers
  1 through *n* language implementations.

  For `M` and `N` where `N == M + 1`, `samizdat-N` is written in
  the language defined by *Samizdat Layer M*. `samizdat-N`
  accepts the language *Samizdat layer N*, translating it to the
  same underlying form that `samizdat-0` executes.

* `sam-comp` &mdash; Tool to compile Samizdat source code.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  code to be linked and run with an associated library (details
  TBD).

  Written in *Samizdat Layer N* (for the ultimate value of `N`).

* ?

* `samizdat-0.sam` &mdash; Samizdat Layer 0 interpreter.

  This is a maximally-reifying implementation of Samizdat Layer 0,
  meant to be both an "executable specification" for Samizdat Layer 0
  (to help enable the implementation of Samizdat Layer 0 in even yet
  other languages), as well as an implementation that can be relied
  upon for further iterative development of the other layers of the
  language.

  Written in Samizdat.

* Profit!

- - - - -

Samizdat Programming Language In N Shout-Outs
=============================================

Samizdat is a high-level programming language somewhere down the
family lineage from all of
[ALGOL](http://en.wikipedia.org/wiki/ALGOL),
[Lisp](http://en.wikipedia.org/wiki/LISP), and
[SNOBOL](http://en.wikipedia.org/wiki/SNOBOL) (really!). It is
intended that the language be an attractive target for use in
text processing, free-form data structure manipulation, and general
systems programming.

The language came about as a second-order effect of the author's
desire to explore the intersection of all of
[promise](http://en.wikipedia.org/wiki/Promise_%28programming%29)-based
[object-capability](http://en.wikipedia.org/wiki/Object-capability_model)
computation, the
[actor model](http://en.wikipedia.org/wiki/Actor_model) of
concurrency, maximally pure / immutable data models, and
[software transactional
memory](http://en.wikipedia.org/wiki/Software_transactional_memory)
(STM). The Samizdat language has a mostly-immutable data model and
generally has "capability nature", but it does not innately implement
promises, actors, or STM. However, it is intended to be a fertile
substrate on which to explore these topics, while still being a
practical and compelling language in its own right.

Samizdat's syntax bears the influences of
[C](http://en.wikipedia.org/wiki/C_%28programming_language%29),
[E](http://en.wikipedia.org/wiki/E_%28programming_language%29),
[Logo](http://en.wikipedia.org/wiki/Logo_%28programming_language%29),
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk),
[Java](http://en.wikipedia.org/wiki/Java_%28programming_language%29),
[JavaScript](http://en.wikipedia.org/wiki/JavaScript),
and traditional [Backus-Naur
Form](http://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form) (BNF).

The Samizdat core data model was influenced by Lisp,
[Haskell](http://en.wikipedia.org/wiki/Haskell_%28programming_language%29),
[Clojure](http://en.wikipedia.org/wiki/Clojure), and JavaScript.

Samizdat's parsing semantics were influenced by the recent work on
[Parsing Expression
Grammars](http://en.wikipedia.org/wiki/Parsing_expression_grammar) (PEGs),
particularly [OMeta](http://tinlizzie.org/ometa/), tempered by a lot
of practical experience with [ANTLR](http://en.wikipedia.org/wiki/ANTLR)
and [AWK](http://en.wikipedia.org/wiki/AWK).

The Samizdat core library was influenced by
[Scheme](http://en.wikipedia.org/wiki/Scheme_%28programming_language%29)
and AWK.

The implementation tactics used to build Samizdat were informed by
[Scheme-48](http://en.wikipedia.org/wiki/Scheme_48) and
the [Jikes RVM](http://en.wikipedia.org/wiki/Jikes_RVM)
(formerly a.k.a. Jalape&ntilde;o), with a tip o' the hat to each of
[PyPy](http://en.wikipedia.org/wiki/PyPy) and
[Chicken](http://en.wikipedia.org/wiki/Chicken_%28Scheme_implementation%29).
