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

Its syntax is inspired by
[C](http://en.wikipedia.org/wiki/C_%28programming_language%29),
[E](http://en.wikipedia.org/wiki/E_%28programming_language%29),
[Java](http://en.wikipedia.org/wiki/Java_%28programming_language%29),
[JavaScript](http://en.wikipedia.org/wiki/JavaScript),
[Logo](http://en.wikipedia.org/wiki/Logo_%28programming_language%29),
and traditional [Backus-Naur
Form](http://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form) (BNF).
Its core data model is inspired by Lisp,
[Haskell](http://en.wikipedia.org/wiki/Haskell_%28programming_language%29),
[Clojure](http://en.wikipedia.org/wiki/Clojure), and JavaScript. Its
parsing semantics are inspired by the recent work on [Parsing
Expression
Grammars](http://en.wikipedia.org/wiki/Parsing_expression_grammar) (PEGs),
particularly [OMeta](http://tinlizzie.org/ometa/), tempered by a lot
of practical experience with [ANTLR](http://en.wikipedia.org/wiki/ANTLR).

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
generally has "capability nature",
but it does not innately implement promises, actors, or STM. However,
it is intended to be a fertile substrate on which to explore these
topics, while still being a practical language in its own right.
