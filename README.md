Samizdat
========

This is the source for the Samizdat programming language, or "Sami" if
you want to be terse.

Watch this space!

- - - - -

Plan of attack
--------------

* `samizdat-0` &mdash; *Samizdat Layer 0* interpreter.

  Tool that accepts input in the form of minimalistic source code, and
  runs it. The language it accepts is meant to be a "parti" of final
  Samizdat, that is, it embodies *just* the main thrusts of the
  language with very little embellishment. This language is called
  *Samizdat Layer 0*.

  *Samizdat Layer 0* also defines a minimal core library. A subset
  of that library is implemented directly in `samizdat-0` as
  primitive functions. The remainder of the library is implemented
  in *Samizdat Layer 0* in terms of that library and the language
  syntax.

  Finally, *Samizdat Layer 0* defines an executable parse tree
  format. This format, along with API defined by the
  primitively-defined part of the core library, are the analog to a
  bytecode specification in systems that are bytecode-based.

  The language parser and runtime do the bare minimum of error
  checking, attempting to fail fast in the face of an error but not to
  provide much in the way of meaningful messages.

  `samizdat-0` is written in C in a style which is meant to be
  verifiably correct by inspection. That is, the implementation
  prefers clarity and obviousness over trickiness and efficiency. In
  addition, it uses a minimum of C library functionality and eschews
  the use of macros *except* as guards on header files and to
  reduce noisy boilerplate.

* `samizdat-0-lib` &mdash; *Samizdat Layer 0* in-language core library.

  This is a library of code which implements the parts of the
  *Samizdat Layer 0* core library which are possible to implement
  in terms of `samizdat-0`'s primitive functions.

  `samizdat-0-lib` is written in *Samizdat Layer 0*. It is
  incorporated into both `samizdat-0` and `samizdat-1` such that
  programs run with either of those tools automatically see the
  full core library definition.

* `samizdat-1` &mdash; *Samizdat Layer 1* interpreter.

  Tool that accepts input written in *Samizdat Layer 0*, but
  which provides more robust error reporting than `samizdat-0`.

  `samizdat-1` is itself written in *Samizdat Layer 0*, in a style
  which mostly "absorbs" the core data library and entirely "reifies"
  the token and tree parsers. In particular, `samizdat-1` translates
  input program text into the identical executable tree form as is
  used by `samizdat-0`.

  The point of `samizdat-1` is twofold:

  * Make for a much nicer experience developing and debugging
    programs written in *Samizdat Layer 0*, compared to `samizdat-0`.

  * Provide some validation (via correlation) of the `samizdat-0`
    token and tree parser implementations. In particular, the source
    code for `samizdat-1` should produce truly identical trees
    when compiled by either `samizdat-0` or by `samizdat-1` itself.

* `samizdat-2` &hellip; `samizdat-N` &mdash; *Samizdat Layers 2
  through N* interpreters.

  For `M` and `N` where `N == M + 1`, `samizdat-N` is written in the
  language implemented by *Samizdat Layer M*. `samizdat-N` accepts the
  language *Samizdat layer N*, translating it to the same underlying
  executable parse tree form that `samizdat-0` executes.

* `sam` &mdash; *Samizdat* interpreter.

  Tool that accepts input written in *Samizdat* (per se), and runs
  it.

  Written in *Samizdat Layer N* (for the ultimate value of `N`).

* `samc` &mdash; Tool to compile Samizdat source code.

  Compiler that accepts bona fide Samizdat syntax and outputs C
  code to be linked and run with an associated library (details
  TBD).

  Written in *Samizdat*. Used to produce (at least) two executables:

  * Run using interpreted `sam` and given `sam`'s source as input, in
    order to produce a compiled version of `sam`.

  * Run using compiled `sam` and given `samc`'s source as input, in
    order to produce a compiled version of `samc`.

* `samizdat-0.sam` &mdash; *Samizdat Layer 0* interpreter.

  This is a maximally-reifying implementation of *Samizdat Layer 0*,
  meant to be both an "executable specification" for *Samizdat Layer
  0* (to help enable the implementation of *Samizdat Layer 0* in even
  yet other languages), as well as an implementation that can be
  relied upon for further iterative development of the other layers of
  the language implementation.

  Written in Samizdat. Run and compiled-and-run with `sam` and `samc`
  (respectively), for purposes of verification, validation, and
  further language iteration.

* ?

* Profit!

- - - - -

Samizdat Programming Language In N Shout-Outs
=============================================

Samizdat is a high-level programming language somewhere down the
family lineage from all of
[ALGOL](http://en.wikipedia.org/wiki/ALGOL),
[Lisp](http://en.wikipedia.org/wiki/LISP), and
[Smalltalk](http://en.wikipedia.org/wiki/Smalltalk). It is
intended that the language be an attractive target for use in
text processing, free-form data structure manipulation, and general
systems programming.

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

Samizdat's syntax bears the influences of
[C](http://en.wikipedia.org/wiki/C_%28programming_language%29),
[E](http://en.wikipedia.org/wiki/E_%28programming_language%29),
[Logo](http://en.wikipedia.org/wiki/Logo_%28programming_language%29),
Smalltalk, [Java](http://en.wikipedia.org/wiki/Java_%28programming_language%29),
[JavaScript](http://en.wikipedia.org/wiki/JavaScript), and traditional
[Backus-Naur Form](http://en.wikipedia.org/wiki/Backus%E2%80%93Naur_Form)
(BNF).

The Samizdat core data model was influenced by Lisp,
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
the trail for languages that make parsing a first-class operation.

The Samizdat core library was influenced by
[Scheme](http://en.wikipedia.org/wiki/Scheme_%28programming_language%29)
and Awk.

The implementation tactics used to build Samizdat were informed by
[Scheme-48](http://en.wikipedia.org/wiki/Scheme_48),
the [Jikes RVM](http://en.wikipedia.org/wiki/Jikes_RVM)
(formerly a.k.a. Jalape&ntilde;o), and
[Squeak](http://en.wikipedia.org/wiki/Squeak), with a tip o' the
hat to each of [PyPy](http://en.wikipedia.org/wiki/PyPy) and
[Chicken](http://en.wikipedia.org/wiki/Chicken_%28Scheme_implementation%29).
