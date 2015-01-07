Old Changelog
=============

* 0.0.1 &mdash; 14-mar-2013 &mdash; "Let Them Eat Pi"

  * **Milestone**: First commit to new repo, and the beginning of the
    third-or-so attempt to get the idea for the system into a concrete form.

* 0.1.0 &mdash; 29-jul-2013 &mdash; "Stake In The Ground"

  * **Milestone**: End of three month sprint to get to some kind of
    proof-of-concept.

  * Low level language concepts mostly settled and stable. High level
    language concepts still very much in flux.

* 0.2.0 &mdash; 3-sep-2013 &mdash; "Method To The Madness"

  * **Milestone**: Method dispatch.

  * Implemented basic type system and method dispatch mechanism based
    on generic functions.

* 0.3.0 &mdash; 6-nov-2013 &mdash; "Now I'm Feeling Modularized"

  * **Milestone:** System is sufficiently stable to start work on a
    self-hosted compiler.

  * Major changes to punctuation tokens.

  * Implemented module system mechanism, though with barely any syntactic
    support.

  * Arranged all core functionality into modules.

* 0.4.0 &mdash; 18-dec-2013 &mdash; "A Tree Grows In SoMa"

  * **Milestone:** Version of runtime where library code is compiled into
    binary form.

  * Runtime now supports loading of "addon" libraries.

  * New `samtoc` compiler produces C code that builds parse trees, for
    interpretation using the original runtime interpreter. As such, it's
    not a "real" compiler, but it does vet the process of building and
    loading binary "addon" libraries.

  * Major revision to the mechanics of module loading.

  * Various rework and changes to syntax:
    * Added dot-based syntax for getter and setter methods.
    * Totally new derived value syntax.
    * Implemented postfix `?` in all layers.

* 0.4.1 &mdash; 19-dec-2013

  * Docs improvements, and minor cleanup.

